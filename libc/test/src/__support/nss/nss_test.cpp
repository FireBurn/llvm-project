//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the tests of the nsswitch.conf reader and of the walk
/// through the sources.
///
//===----------------------------------------------------------------------===//

#include "src/__support/CPP/string_view.h"
#include "src/__support/nss/config.h"
#include "src/__support/nss/dispatch.h"
#include "src/__support/nss/module.h"
#include "test/UnitTest/Test.h"

using namespace LIBC_NAMESPACE::nss;
using LIBC_NAMESPACE::cpp::string_view;

namespace {

string_view name_of(const Source *s) { return string_view(s->name); }

} // namespace

TEST(LlvmLibcNssConfigTest, ReadsServicesInOrder) {
  Configuration *c = parse("passwd: files systemd\n"
                           "hosts:\tfiles   mdns4_minimal dns\n");
  ASSERT_TRUE(c != nullptr);
  const Source *s = c->sources[PASSWD];
  ASSERT_TRUE(name_of(s) == "files");
  ASSERT_TRUE(name_of(s->next) == "systemd");
  ASSERT_TRUE(s->next->next == nullptr);
  s = c->sources[HOSTS];
  ASSERT_TRUE(name_of(s) == "files");
  ASSERT_TRUE(name_of(s->next) == "mdns4_minimal");
  ASSERT_TRUE(name_of(s->next->next) == "dns");
}

TEST(LlvmLibcNssConfigTest, DefaultsForWhatIsNotMentioned) {
  Configuration *c = parse("# nothing\n\n");
  ASSERT_TRUE(c != nullptr);
  ASSERT_TRUE(name_of(c->sources[GROUP]) == "files");
  ASSERT_TRUE(c->sources[GROUP]->next == nullptr);
  ASSERT_TRUE(name_of(c->sources[HOSTS]->next) == "dns");
}

TEST(LlvmLibcNssConfigTest, CommentsContinuationsAndUnknownDatabases) {
  Configuration *c = parse("passwd: files # the file\n"
                           "automount: files\n"
                           "group: files \\\n"
                           "   systemd\n"
                           "  Shadow :  files\n");
  ASSERT_TRUE(c != nullptr);
  ASSERT_TRUE(c->sources[PASSWD]->next == nullptr);
  ASSERT_TRUE(name_of(c->sources[GROUP]->next) == "systemd");
  ASSERT_TRUE(name_of(c->sources[SHADOW]) == "files");
}

TEST(LlvmLibcNssConfigTest, LastLineForADatabaseWins) {
  Configuration *c = parse("hosts: files\nhosts: dns\n");
  ASSERT_TRUE(c != nullptr);
  ASSERT_TRUE(name_of(c->sources[HOSTS]) == "dns");
  ASSERT_TRUE(c->sources[HOSTS]->next == nullptr);
}

TEST(LlvmLibcNssConfigTest, ActionsDefaultAndGiven) {
  Configuration *c = parse("hosts: files [NOTFOUND=return] "
                           "mdns [!UNAVAIL=return] dns [success=continue]\n");
  ASSERT_TRUE(c != nullptr);
  const Source *files = c->sources[HOSTS];
  ASSERT_TRUE(files->action(SUCCESS) == Action::RETURN);
  ASSERT_TRUE(files->action(NOTFOUND) == Action::RETURN);
  ASSERT_TRUE(files->action(UNAVAIL) == Action::CONTINUE);
  ASSERT_TRUE(files->action(TRYAGAIN) == Action::CONTINUE);
  const Source *mdns = files->next;
  ASSERT_TRUE(mdns->action(SUCCESS) == Action::RETURN);
  ASSERT_TRUE(mdns->action(NOTFOUND) == Action::RETURN);
  ASSERT_TRUE(mdns->action(TRYAGAIN) == Action::RETURN);
  ASSERT_TRUE(mdns->action(UNAVAIL) == Action::CONTINUE);
  ASSERT_TRUE(mdns->next->action(SUCCESS) == Action::CONTINUE);
}

TEST(LlvmLibcNssConfigTest, DatabaseNames) {
  Database db;
  ASSERT_TRUE(database_from_name("PASSWD", db) && db == PASSWD);
  ASSERT_FALSE(database_from_name("automount", db));
  ASSERT_STREQ(database_name(NETGROUP), "netgroup");
}

namespace {

int calls[8];
int answers[8];

// Stands for three modules: "a", "b" and "c", each answering as the test says.
void *resolver(const char *service, const char *) {
  if (service[0] == 'x')
    return nullptr;
  return &calls[service[0] - 'a'];
}

Status ask(void *fn) {
  int i = static_cast<int *>(fn) - calls;
  ++calls[i];
  return static_cast<Status>(answers[i]);
}

Status none() { return UNAVAIL; }

void reset() {
  for (int i = 0; i < 8; ++i)
    calls[i] = 0;
  TESTONLY_set_resolver(resolver);
}

} // namespace

TEST(LlvmLibcNssDispatchTest, StopsAtASuccess) {
  reset();
  ASSERT_TRUE(configure("group", "a b c"));
  answers[0] = NOTFOUND;
  answers[1] = SUCCESS;
  answers[2] = SUCCESS;
  ASSERT_EQ(lookup(GROUP, "getgrnam_r", none, none, ask), SUCCESS);
  ASSERT_EQ(calls[0], 1);
  ASSERT_EQ(calls[1], 1);
  ASSERT_EQ(calls[2], 0);
  TESTONLY_set_resolver(nullptr);
}

TEST(LlvmLibcNssDispatchTest, NotFoundContinuesUnlessToldOtherwise) {
  reset();
  ASSERT_TRUE(configure("group", "a [NOTFOUND=return] b"));
  answers[0] = NOTFOUND;
  answers[1] = SUCCESS;
  ASSERT_EQ(lookup(GROUP, "getgrnam_r", none, none, ask), NOTFOUND);
  ASSERT_EQ(calls[1], 0);
  TESTONLY_set_resolver(nullptr);
}

TEST(LlvmLibcNssDispatchTest, AMergedSuccessSurvivesSourcesWithNothingToAdd) {
  reset();
  ASSERT_TRUE(configure("group", "a [SUCCESS=merge] b"));
  answers[0] = SUCCESS;
  answers[1] = NOTFOUND;
  ASSERT_EQ(lookup(GROUP, "getgrnam_r", none, none, ask), SUCCESS);
  ASSERT_EQ(calls[0], 1);
  ASSERT_EQ(calls[1], 1);
  // Without the merge the first success ends the walk.
  ASSERT_TRUE(configure("group", "a b"));
  reset();
  answers[0] = SUCCESS;
  ASSERT_EQ(lookup(GROUP, "getgrnam_r", none, none, ask), SUCCESS);
  ASSERT_EQ(calls[1], 0);
  // A source with nothing for the name does not turn a miss into a hit.
  ASSERT_TRUE(configure("group", "a [NOTFOUND=continue] b"));
  reset();
  answers[0] = NOTFOUND;
  answers[1] = NOTFOUND;
  ASSERT_EQ(lookup(GROUP, "getgrnam_r", none, none, ask), NOTFOUND);
  TESTONLY_set_resolver(nullptr);
}

TEST(LlvmLibcNssDispatchTest, MissingModulesAreUnavailable) {
  reset();
  ASSERT_TRUE(configure("group", "x a"));
  answers[0] = SUCCESS;
  ASSERT_EQ(lookup(GROUP, "getgrnam_r", none, none, ask), SUCCESS);
  ASSERT_TRUE(configure("group", "x"));
  ASSERT_EQ(lookup(GROUP, "getgrnam_r", none, none, ask), UNAVAIL);
  TESTONLY_set_resolver(nullptr);
}

TEST(LlvmLibcNssDispatchTest, FilesAndDnsAreAnsweredByLibc) {
  reset();
  ASSERT_TRUE(configure("hosts", "files a dns"));
  int files_calls = 0, dns_calls = 0;
  answers[0] = NOTFOUND;
  Status r = lookup(
      HOSTS, "gethostbyname2_r",
      [&] {
        ++files_calls;
        return NOTFOUND;
      },
      [&] {
        ++dns_calls;
        return SUCCESS;
      },
      ask);
  ASSERT_EQ(r, SUCCESS);
  ASSERT_EQ(files_calls, 1);
  ASSERT_EQ(calls[0], 1);
  ASSERT_EQ(dns_calls, 1);
  TESTONLY_set_resolver(nullptr);
}

TEST(LlvmLibcNssDispatchTest, ModuleSaysReturn) {
  reset();
  ASSERT_TRUE(configure("group", "a b"));
  answers[0] = RETURN;
  answers[1] = SUCCESS;
  ASSERT_EQ(lookup(GROUP, "getgrnam_r", none, none, ask), RETURN);
  ASSERT_EQ(calls[1], 0);
  TESTONLY_set_resolver(nullptr);
}

namespace {

int pending[3];
int position;

Status entry(void *fn, bool control) {
  int i = static_cast<int *>(fn) - calls;
  ++calls[i];
  if (control)
    return SUCCESS;
  return pending[position] > 0 ? (--pending[position], SUCCESS) : NOTFOUND;
}

} // namespace

TEST(LlvmLibcNssDispatchTest, WalksTheEntriesOfEverySource) {
  reset();
  ASSERT_TRUE(configure("passwd", "files a b"));
  pending[0] = 2;
  Cursor cursor;
  int files_left = 1;
  auto files = [&] { return files_left-- > 0 ? SUCCESS : NOTFOUND; };
  int seen = 0;
  position = 0;
  while (next_entry(cursor, PASSWD, "setpwent", "getpwent_r", "endpwent", files,
                    entry) == SUCCESS)
    ++seen;
  // One from the file, two from module a and none from module b.
  ASSERT_EQ(seen, 3);
  TESTONLY_set_resolver(nullptr);
}
