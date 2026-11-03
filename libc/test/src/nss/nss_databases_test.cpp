//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the tests of the databases answering through service
/// modules.
///
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/netdb_macros.h"
#include "hdr/sys_socket_macros.h"
#include "hdr/types/struct_addrinfo.h"
#include "hdr/types/struct_group.h"
#include "hdr/types/struct_hostent.h"
#include "hdr/types/struct_netent.h"
#include "hdr/types/struct_passwd.h"
#include "hdr/types/struct_protoent.h"
#include "hdr/types/struct_servent.h"
#include "hdr/types/struct_sockaddr_in.h"
#include "src/__support/CPP/string_view.h"
#include "src/__support/nss/config.h"
#include "src/__support/nss/module.h"
#include "src/grp/getgrgid.h"
#include "src/grp/getgrnam.h"
#include "src/grp/getgrouplist.h"
#include "src/netdb/freeaddrinfo.h"
#include "src/netdb/getaddrinfo.h"
#include "src/netdb/gethostbyaddr.h"
#include "src/netdb/gethostbyname.h"
#include "src/netdb/getnetbyname.h"
#include "src/netdb/getprotobyname.h"
#include "src/netdb/getprotobynumber.h"
#include "src/netdb/getservbyname.h"
#include "src/netdb/getservbyport.h"
#include "src/pwd/endpwent.h"
#include "src/pwd/getpwent.h"
#include "src/pwd/getpwnam.h"
#include "src/pwd/getpwnam_r.h"
#include "src/pwd/getpwuid.h"
#include "src/pwd/setpwent.h"
#include "src/string/memcpy.h"
#include "src/string/string_utils.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"
#include "test/src/pwd/pwd_test_utils.h"

using LIBC_NAMESPACE::cpp::string_view;
using LIBC_NAMESPACE::nss::Status;

namespace {

// A module named "fake" which knows one user, one group and a pair of
// entries to enumerate, written the way glibc's modules are.
int pwent_position = 0;
int setpwent_calls = 0;
int endpwent_calls = 0;
int too_small_once = 0;

Status fake_getpwnam_r(const char *name, struct passwd *pw, char *buf,
                       size_t len, int *errnop) {
  if (string_view(name) != "ghost")
    return LIBC_NAMESPACE::nss::NOTFOUND;
  if (too_small_once-- > 0 || len < 64) {
    *errnop = ERANGE;
    return LIBC_NAMESPACE::nss::TRYAGAIN;
  }
  LIBC_NAMESPACE::memcpy(buf, "ghost\0/home/ghost\0", 18);
  pw->pw_name = buf;
  pw->pw_passwd = buf + 5;
  pw->pw_uid = 4242;
  pw->pw_gid = 4242;
  pw->pw_gecos = buf + 5;
  pw->pw_dir = buf + 6;
  pw->pw_shell = buf + 5;
  return LIBC_NAMESPACE::nss::SUCCESS;
}

Status fake_getpwuid_r(uid_t uid, struct passwd *pw, char *buf, size_t len,
                       int *errnop) {
  if (uid != 4242)
    return LIBC_NAMESPACE::nss::NOTFOUND;
  return fake_getpwnam_r("ghost", pw, buf, len, errnop);
}

Status fake_setpwent(int) {
  ++setpwent_calls;
  pwent_position = 0;
  return LIBC_NAMESPACE::nss::SUCCESS;
}

Status fake_endpwent(void) {
  ++endpwent_calls;
  return LIBC_NAMESPACE::nss::SUCCESS;
}

Status fake_getpwent_r(struct passwd *pw, char *buf, size_t len, int *errnop) {
  static const char *names[] = {"first", "second"};
  if (pwent_position >= 2)
    return LIBC_NAMESPACE::nss::NOTFOUND;
  const char *n = names[pwent_position++];
  size_t size = LIBC_NAMESPACE::internal::string_length(n) + 1;
  if (len < size + 1) {
    *errnop = ERANGE;
    return LIBC_NAMESPACE::nss::TRYAGAIN;
  }
  LIBC_NAMESPACE::memcpy(buf, n, size);
  buf[size] = '\0';
  pw->pw_name = buf;
  pw->pw_passwd = buf + size;
  pw->pw_uid = static_cast<uid_t>(5000 + pwent_position);
  pw->pw_gid = 5000;
  pw->pw_gecos = buf + size;
  pw->pw_dir = buf + size;
  pw->pw_shell = buf + size;
  return LIBC_NAMESPACE::nss::SUCCESS;
}

Status fake_getgrnam_r(const char *name, struct group *gr, char *buf,
                       size_t len, int *errnop) {
  if (string_view(name) != "phantoms")
    return LIBC_NAMESPACE::nss::NOTFOUND;
  if (len < 32) {
    *errnop = ERANGE;
    return LIBC_NAMESPACE::nss::TRYAGAIN;
  }
  LIBC_NAMESPACE::memcpy(buf, "phantoms\0", 9);
  gr->gr_name = buf;
  gr->gr_passwd = buf + 8;
  gr->gr_gid = 7777;
  auto **members = reinterpret_cast<char **>(
      (reinterpret_cast<unsigned long>(buf + 9) + 7) & ~7ul);
  members[0] = nullptr;
  gr->gr_mem = members;
  return LIBC_NAMESPACE::nss::SUCCESS;
}

Status fake_getgrgid_r(gid_t gid, struct group *gr, char *buf, size_t len,
                       int *errnop) {
  if (gid != 7777)
    return LIBC_NAMESPACE::nss::NOTFOUND;
  return fake_getgrnam_r("phantoms", gr, buf, len, errnop);
}

Status fake_initgroups_dyn(const char *user, gid_t, long *start, long *,
                           gid_t **groups, long, int *) {
  if (string_view(user) != "ghost")
    return LIBC_NAMESPACE::nss::NOTFOUND;
  (*groups)[(*start)++] = 7777;
  return LIBC_NAMESPACE::nss::SUCCESS;
}

struct ModuleAddress {
  ModuleAddress *next;
  char *name;
  int family;
  uint32_t address[4];
  uint32_t scope;
};

Status fake_gethostbyname4_r(const char *name, ModuleAddress **pat, char *buf,
                             size_t len, int *errnop, int *, int32_t *ttl) {
  if (string_view(name) != "fake.test")
    return LIBC_NAMESPACE::nss::NOTFOUND;
  if (len < sizeof(ModuleAddress) + 16) {
    *errnop = ERANGE;
    return LIBC_NAMESPACE::nss::TRYAGAIN;
  }
  auto *tuple = reinterpret_cast<ModuleAddress *>(buf);
  char *text = buf + sizeof(ModuleAddress);
  LIBC_NAMESPACE::memcpy(text, "fake.test", 10);
  tuple->next = nullptr;
  tuple->name = text;
  tuple->family = AF_INET;
  tuple->address[0] = 0x0100000a; // 10.0.0.1 in network order.
  tuple->address[1] = tuple->address[2] = tuple->address[3] = 0;
  tuple->scope = 0;
  *pat = tuple;
  *ttl = 30;
  return LIBC_NAMESPACE::nss::SUCCESS;
}

Status fake_gethostbyname3_r(const char *name, int af, struct hostent *host,
                             char *buf, size_t len, int *errnop, int *,
                             int32_t *, char **canon) {
  if (string_view(name) != "old.test" || af != AF_INET)
    return LIBC_NAMESPACE::nss::NOTFOUND;
  if (len < 64) {
    *errnop = ERANGE;
    return LIBC_NAMESPACE::nss::TRYAGAIN;
  }
  LIBC_NAMESPACE::memcpy(buf, "old.test", 9);
  host->h_name = buf;
  host->h_aliases = reinterpret_cast<char **>(buf + 16);
  host->h_aliases[0] = nullptr;
  host->h_addrtype = AF_INET;
  host->h_length = 4;
  auto *list = reinterpret_cast<char **>(buf + 32);
  unsigned char *address = reinterpret_cast<unsigned char *>(buf + 48);
  address[0] = 10;
  address[1] = 0;
  address[2] = 0;
  address[3] = 2;
  list[0] = reinterpret_cast<char *>(address);
  list[1] = nullptr;
  host->h_addr_list = list;
  if (canon)
    *canon = buf;
  return LIBC_NAMESPACE::nss::SUCCESS;
}

Status fake_gethostbyaddr2_r(const void *addr, socklen_t len, int af,
                             struct hostent *host, char *buf, size_t buflen,
                             int *errnop, int *, int32_t *) {
  const auto *b = static_cast<const unsigned char *>(addr);
  if (af != AF_INET || len != 4 || b[0] != 10 || b[3] != 1)
    return LIBC_NAMESPACE::nss::NOTFOUND;
  if (buflen < 16) {
    *errnop = ERANGE;
    return LIBC_NAMESPACE::nss::TRYAGAIN;
  }
  LIBC_NAMESPACE::memcpy(buf, "fake.test", 10);
  host->h_name = buf;
  return LIBC_NAMESPACE::nss::SUCCESS;
}

Status fake_getservbyname_r(const char *name, const char *, struct servent *s,
                            char *buf, size_t, int *) {
  if (string_view(name) != "ghostsvc")
    return LIBC_NAMESPACE::nss::NOTFOUND;
  LIBC_NAMESPACE::memcpy(buf, "ghostsvc\0tcp", 13);
  s->s_name = buf;
  s->s_aliases = reinterpret_cast<char **>(buf + 16);
  s->s_aliases[0] = nullptr;
  s->s_port = 0x0f27; // 9999 in network order.
  s->s_proto = buf + 9;
  return LIBC_NAMESPACE::nss::SUCCESS;
}

Status fake_getprotobyname_r(const char *name, struct protoent *p, char *buf,
                             size_t, int *) {
  if (string_view(name) != "ghostproto")
    return LIBC_NAMESPACE::nss::NOTFOUND;
  LIBC_NAMESPACE::memcpy(buf, "ghostproto", 11);
  p->p_name = buf;
  p->p_aliases = reinterpret_cast<char **>(buf + 16);
  p->p_aliases[0] = nullptr;
  p->p_proto = 200;
  return LIBC_NAMESPACE::nss::SUCCESS;
}

Status fake_getprotobynumber_r(int number, struct protoent *p, char *buf,
                               size_t len, int *errnop) {
  if (number != 200)
    return LIBC_NAMESPACE::nss::NOTFOUND;
  return fake_getprotobyname_r("ghostproto", p, buf, len, errnop);
}

Status fake_getnetbyname_r(const char *name, struct netent *n, char *buf,
                           size_t, int *, int *) {
  if (string_view(name) != "ghostnet")
    return LIBC_NAMESPACE::nss::NOTFOUND;
  LIBC_NAMESPACE::memcpy(buf, "ghostnet", 9);
  n->n_name = buf;
  n->n_aliases = reinterpret_cast<char **>(buf + 16);
  n->n_aliases[0] = nullptr;
  n->n_addrtype = AF_INET;
  n->n_net = 0x0a0000;
  return LIBC_NAMESPACE::nss::SUCCESS;
}

void *resolver(const char *service, const char *function) {
  string_view f(function);
  if (string_view(service) == "old")
    return f == "gethostbyname3_r"
               ? reinterpret_cast<void *>(&fake_gethostbyname3_r)
               : nullptr;
  if (string_view(service) != "fake")
    return nullptr;
  if (f == "getpwnam_r")
    return reinterpret_cast<void *>(&fake_getpwnam_r);
  if (f == "getpwuid_r")
    return reinterpret_cast<void *>(&fake_getpwuid_r);
  if (f == "setpwent")
    return reinterpret_cast<void *>(&fake_setpwent);
  if (f == "endpwent")
    return reinterpret_cast<void *>(&fake_endpwent);
  if (f == "getpwent_r")
    return reinterpret_cast<void *>(&fake_getpwent_r);
  if (f == "getgrnam_r")
    return reinterpret_cast<void *>(&fake_getgrnam_r);
  if (f == "getgrgid_r")
    return reinterpret_cast<void *>(&fake_getgrgid_r);
  if (f == "gethostbyname4_r")
    return reinterpret_cast<void *>(&fake_gethostbyname4_r);
  if (f == "gethostbyaddr2_r")
    return reinterpret_cast<void *>(&fake_gethostbyaddr2_r);
  if (f == "getservbyname_r")
    return reinterpret_cast<void *>(&fake_getservbyname_r);
  if (f == "getprotobyname_r")
    return reinterpret_cast<void *>(&fake_getprotobyname_r);
  if (f == "getprotobynumber_r")
    return reinterpret_cast<void *>(&fake_getprotobynumber_r);
  if (f == "getnetbyname_r")
    return reinterpret_cast<void *>(&fake_getnetbyname_r);
  if (f == "initgroups_dyn")
    return reinterpret_cast<void *>(&fake_initgroups_dyn);
  return nullptr;
}

class LlvmLibcNssDatabasesTest : public LlvmLibcPwdTest {
protected:
  void SetUp() override {
    LIBC_NAMESPACE::nss::TESTONLY_set_config_path("/nonexistent/nsswitch.conf");
    LIBC_NAMESPACE::nss::TESTONLY_set_resolver(resolver);
    ASSERT_TRUE(LIBC_NAMESPACE::nss::configure("passwd", "files fake"));
    ASSERT_TRUE(LIBC_NAMESPACE::nss::configure("group", "files fake"));
    ASSERT_TRUE(LIBC_NAMESPACE::nss::configure("hosts", "fake old"));
    ASSERT_TRUE(LIBC_NAMESPACE::nss::configure("services", "fake"));
    ASSERT_TRUE(LIBC_NAMESPACE::nss::configure("protocols", "fake"));
    ASSERT_TRUE(LIBC_NAMESPACE::nss::configure("networks", "fake"));
    pwent_position = setpwent_calls = endpwent_calls = too_small_once = 0;
  }
  void TearDown() override {
    LIBC_NAMESPACE::nss::TESTONLY_set_resolver(nullptr);
    LIBC_NAMESPACE::nss::TESTONLY_reset_config_path();
    LlvmLibcPwdTest::TearDown();
  }
};

} // namespace

TEST_F(LlvmLibcNssDatabasesTest, FileFirstThenModule) {
  ScopedPasswdFile file(libc_make_test_file_path("nss_passwd.test"),
                        "root:x:0:0:root:/root:/bin/sh\n");
  struct passwd *root = LIBC_NAMESPACE::getpwnam("root");
  ASSERT_TRUE(root != nullptr);
  ASSERT_EQ(root->pw_uid, static_cast<uid_t>(0));
  struct passwd *ghost = LIBC_NAMESPACE::getpwnam("ghost");
  ASSERT_TRUE(ghost != nullptr);
  ASSERT_EQ(ghost->pw_uid, static_cast<uid_t>(4242));
  ASSERT_STREQ(ghost->pw_dir, "/home/ghost");
  ASSERT_TRUE(LIBC_NAMESPACE::getpwnam("nobody-at-all") == nullptr);
  struct passwd *by_id = LIBC_NAMESPACE::getpwuid(4242);
  ASSERT_TRUE(by_id != nullptr);
  ASSERT_STREQ(by_id->pw_name, "ghost");
  ASSERT_TRUE(LIBC_NAMESPACE::getpwuid(31337) == nullptr);
}

TEST_F(LlvmLibcNssDatabasesTest, ReentrantLookupReportsASmallBuffer) {
  ScopedPasswdFile file(libc_make_test_file_path("nss_passwd2.test"),
                        "root:x:0:0:root:/root:/bin/sh\n");
  struct passwd pw;
  struct passwd *result;
  char small[16];
  ASSERT_EQ(
      LIBC_NAMESPACE::getpwnam_r("ghost", &pw, small, sizeof(small), &result),
      ERANGE);
  char big[256];
  ASSERT_EQ(LIBC_NAMESPACE::getpwnam_r("ghost", &pw, big, sizeof(big), &result),
            0);
  ASSERT_TRUE(result == &pw);
}

TEST_F(LlvmLibcNssDatabasesTest, TheBufferOfTheOrdinaryCallGrows) {
  ScopedPasswdFile file(libc_make_test_file_path("nss_passwd3.test"),
                        "root:x:0:0:root:/root:/bin/sh\n");
  too_small_once = 3;
  struct passwd *ghost = LIBC_NAMESPACE::getpwnam("ghost");
  ASSERT_TRUE(ghost != nullptr);
  ASSERT_STREQ(ghost->pw_name, "ghost");
}

TEST_F(LlvmLibcNssDatabasesTest, ActionsDecideWhetherToGoOn) {
  ScopedPasswdFile file(libc_make_test_file_path("nss_passwd4.test"),
                        "root:x:0:0:root:/root:/bin/sh\n");
  ASSERT_TRUE(
      LIBC_NAMESPACE::nss::configure("passwd", "files [NOTFOUND=return] fake"));
  ASSERT_TRUE(LIBC_NAMESPACE::getpwnam("ghost") == nullptr);
}

TEST_F(LlvmLibcNssDatabasesTest, EnumerationVisitsEverySource) {
  ScopedPasswdFile file(libc_make_test_file_path("nss_passwd5.test"),
                        "root:x:0:0:root:/root:/bin/sh\n"
                        "daemon:x:1:1:d:/:/bin/false\n");
  LIBC_NAMESPACE::setpwent();
  char seen[8];
  int n = 0;
  while (struct passwd *pw = LIBC_NAMESPACE::getpwent()) {
    ASSERT_TRUE(n < 8);
    seen[n++] = pw->pw_name[0];
  }
  ASSERT_EQ(n, 4);
  // The buffer is reused, so only the first letters are kept: root, daemon,
  // then the two of the module.
  ASSERT_TRUE(seen[0] == 'r' && seen[1] == 'd');
  ASSERT_TRUE(seen[2] == 'f' && seen[3] == 's');
  ASSERT_EQ(setpwent_calls, 1);
  ASSERT_EQ(endpwent_calls, 1);
  LIBC_NAMESPACE::endpwent();
}

TEST_F(LlvmLibcNssDatabasesTest, GroupsByNameAndNumber) {
  struct group *g = LIBC_NAMESPACE::getgrnam("phantoms");
  ASSERT_TRUE(g != nullptr);
  ASSERT_EQ(g->gr_gid, static_cast<gid_t>(7777));
  g = LIBC_NAMESPACE::getgrgid(7777);
  ASSERT_TRUE(g != nullptr);
  ASSERT_STREQ(g->gr_name, "phantoms");
  ASSERT_TRUE(LIBC_NAMESPACE::getgrnam("no-such-group") == nullptr);
}

TEST_F(LlvmLibcNssDatabasesTest, ModulesAddToTheGroupsOfAUser) {
  gid_t groups[8];
  int n = 8;
  ASSERT_EQ(LIBC_NAMESPACE::getgrouplist("ghost", 100, groups, &n), 2);
  ASSERT_EQ(groups[0], static_cast<gid_t>(100));
  ASSERT_EQ(groups[1], static_cast<gid_t>(7777));
}

TEST_F(LlvmLibcNssDatabasesTest, GetaddrinfoAsksTheModules) {
  struct addrinfo hints = {};
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  struct addrinfo *res = nullptr;
  ASSERT_EQ(LIBC_NAMESPACE::getaddrinfo("fake.test", nullptr, &hints, &res), 0);
  ASSERT_TRUE(res != nullptr);
  ASSERT_EQ(res->ai_family, AF_INET);
  auto *in = reinterpret_cast<struct sockaddr_in *>(res->ai_addr);
  ASSERT_EQ(in->sin_addr.s_addr, 0x0100000au);
  LIBC_NAMESPACE::freeaddrinfo(res);
}

TEST_F(LlvmLibcNssDatabasesTest, GetaddrinfoFallsBackToTheOlderFunction) {
  struct addrinfo hints = {};
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  struct addrinfo *res = nullptr;
  ASSERT_EQ(LIBC_NAMESPACE::getaddrinfo("old.test", nullptr, &hints, &res), 0);
  auto *in = reinterpret_cast<struct sockaddr_in *>(res->ai_addr);
  ASSERT_EQ(in->sin_addr.s_addr, 0x0200000au);
  LIBC_NAMESPACE::freeaddrinfo(res);
  ASSERT_EQ(LIBC_NAMESPACE::getaddrinfo("nowhere.test", nullptr, &hints, &res),
            EAI_NONAME);
}

TEST_F(LlvmLibcNssDatabasesTest, GethostbynameAndGethostbyaddr) {
  struct hostent *h = LIBC_NAMESPACE::gethostbyname("fake.test");
  ASSERT_TRUE(h != nullptr);
  ASSERT_EQ(h->h_length, 4);
  ASSERT_EQ(static_cast<int>(static_cast<unsigned char>(h->h_addr_list[0][3])),
            1);
  unsigned char address[4] = {10, 0, 0, 1};
  h = LIBC_NAMESPACE::gethostbyaddr(address, 4, AF_INET);
  ASSERT_TRUE(h != nullptr);
  ASSERT_STREQ(h->h_name, "fake.test");
}

TEST_F(LlvmLibcNssDatabasesTest, ServicesProtocolsAndNetworks) {
  struct servent *s = LIBC_NAMESPACE::getservbyname("ghostsvc", "tcp");
  ASSERT_TRUE(s != nullptr);
  ASSERT_EQ(s->s_port, 0x0f27);
  ASSERT_STREQ(s->s_proto, "tcp");
  ASSERT_TRUE(LIBC_NAMESPACE::getservbyname("nosuchsvc", "tcp") == nullptr);
  struct protoent *p = LIBC_NAMESPACE::getprotobyname("ghostproto");
  ASSERT_TRUE(p != nullptr);
  ASSERT_EQ(p->p_proto, 200);
  p = LIBC_NAMESPACE::getprotobynumber(200);
  ASSERT_TRUE(p != nullptr);
  ASSERT_STREQ(p->p_name, "ghostproto");
  struct netent *n = LIBC_NAMESPACE::getnetbyname("ghostnet");
  ASSERT_TRUE(n != nullptr);
  ASSERT_EQ(n->n_net, 0x0a0000u);
}
