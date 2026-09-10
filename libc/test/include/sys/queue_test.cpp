//===-- Unittests for queue -----------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/__support/CPP/string.h"
#include "src/__support/char_vector.h"
#include "src/__support/macros/config.h"
#include "test/UnitTest/Test.h"

#include "include/llvm-libc-macros/sys-queue-macros.h"

using LIBC_NAMESPACE::CharVector;
using LIBC_NAMESPACE::cpp::string;

namespace LIBC_NAMESPACE_DECL {

TEST(LlvmLibcQueueTest, SList) {
  struct Entry {
    char c;
    SLIST_ENTRY(Entry) entries;
  };

  SLIST_HEAD(Head, Entry);

  Head head = SLIST_HEAD_INITIALIZER(head);

  struct Contains : public testing::Matcher<Head> {
    string s;
    Contains(string s) : s(s) {}
    bool match(Head head) {
      Entry *e;
      CharVector v;
      SLIST_FOREACH(e, &head, entries) { v.append(e->c); }
      return s == v.c_str();
    }
  };

  Entry e1 = {'a', {NULL}};
  SLIST_INSERT_HEAD(&head, &e1, entries);

  ASSERT_THAT(head, Contains("a"));

  Entry e2 = {'b', {NULL}};
  SLIST_INSERT_AFTER(&e1, &e2, entries);

  ASSERT_THAT(head, Contains("ab"));

  Head head2 = SLIST_HEAD_INITIALIZER(head);

  Entry e3 = {'c', {NULL}};
  SLIST_INSERT_HEAD(&head2, &e3, entries);

  ASSERT_THAT(head2, Contains("c"));

  SLIST_SWAP(&head, &head2, Entry);

  ASSERT_THAT(head2, Contains("ab"));

  SLIST_CONCAT(&head2, &head, Entry, entries);

  ASSERT_THAT(head2, Contains("abc"));

  SLIST_CONCAT(&head, &head2, Entry, entries);

  ASSERT_THAT(head, Contains("abc"));

  Entry *e = NULL, *tmp = NULL;
  SLIST_FOREACH_SAFE(e, &head, entries, tmp) {
    if (e == &e2) {
      SLIST_REMOVE(&head, e, Entry, entries);
    }
  }

  ASSERT_THAT(head, Contains("ac"));

  while (!SLIST_EMPTY(&head)) {
    e = SLIST_FIRST(&head);
    SLIST_REMOVE_HEAD(&head, entries);
  }

  ASSERT_TRUE(SLIST_EMPTY(&head));
}

TEST(LlvmLibcQueueTest, List) {
  struct Entry {
    char c;
    LIST_ENTRY(Entry) entries;
  };

  LIST_HEAD(Head, Entry);

  Head head = LIST_HEAD_INITIALIZER(head);

  struct Contains : public testing::Matcher<Head> {
    string s;
    Contains(string s) : s(s) {}
    bool match(Head head) {
      Entry *e;
      CharVector v;
      LIST_FOREACH(e, &head, entries) { v.append(e->c); }
      return s == v.c_str();
    }
  };

  Entry a{'a', {}};
  Entry b{'b', {}};
  Entry c{'c', {}};

  LIST_INSERT_HEAD(&head, &c, entries);
  LIST_INSERT_HEAD(&head, &a, entries);
  LIST_INSERT_AFTER(&a, &b, entries);

  ASSERT_THAT(head, Contains("abc"));

  // A list is walked backwards as well, which is what the second link is for.
  ASSERT_EQ(LIST_FIRST(&head), &a);
  ASSERT_EQ(LIST_NEXT(&a, entries), &b);
  ASSERT_EQ(LIST_PREV(&b, &head, Entry, entries), &a);

  Entry d{'d', {}};
  LIST_INSERT_BEFORE(&b, &d, entries);
  ASSERT_THAT(head, Contains("adbc"));

  LIST_REMOVE(&d, entries);
  ASSERT_THAT(head, Contains("abc"));

  // Removing while walking needs the safe form, which reads the next link
  // before the entry it is in goes away.
  Entry *e, *tmp;
  LIST_FOREACH_SAFE(e, &head, entries, tmp) {
    if (e->c == 'b')
      LIST_REMOVE(e, entries);
  }
  ASSERT_THAT(head, Contains("ac"));

  Head other = LIST_HEAD_INITIALIZER(other);
  Entry x{'x', {}};
  LIST_INSERT_HEAD(&other, &x, entries);

  LIST_SWAP(&head, &other, Entry, entries);
  ASSERT_THAT(head, Contains("x"));
  ASSERT_THAT(other, Contains("ac"));

  LIST_CONCAT(&head, &other, Entry, entries);
  ASSERT_THAT(head, Contains("xac"));
  ASSERT_TRUE(LIST_EMPTY(&other));

  LIST_INIT(&head);
  ASSERT_TRUE(LIST_EMPTY(&head));
}

TEST(LlvmLibcQueueTest, TailQ) {
  struct Entry {
    char c;
    TAILQ_ENTRY(Entry) entries;
  };

  TAILQ_HEAD(Head, Entry);

  Head head = TAILQ_HEAD_INITIALIZER(head);

  struct Contains : public testing::Matcher<Head> {
    string s;
    Contains(string s) : s(s) {}
    bool match(Head head) {
      Entry *e;
      CharVector v;
      TAILQ_FOREACH(e, &head, entries) { v.append(e->c); }
      return s == v.c_str();
    }
  };

  struct ContainsReverse : public testing::Matcher<Head> {
    string s;
    ContainsReverse(string s) : s(s) {}
    bool match(Head head) {
      Entry *e;
      CharVector v;
      TAILQ_FOREACH_REVERSE(e, &head, Head, entries) { v.append(e->c); }
      return s == v.c_str();
    }
  };

  Entry a{'a', {}};
  Entry b{'b', {}};
  Entry c{'c', {}};

  TAILQ_INIT(&head);
  TAILQ_INSERT_HEAD(&head, &b, entries);
  TAILQ_INSERT_TAIL(&head, &c, entries);
  TAILQ_INSERT_BEFORE(&b, &a, entries);

  ASSERT_THAT(head, Contains("abc"));
  // Reaching the end without walking is what the tail pointer is for.
  ASSERT_THAT(head, ContainsReverse("cba"));
  ASSERT_EQ(TAILQ_LAST(&head, Head), &c);
  ASSERT_EQ(TAILQ_PREV(&b, Head, entries), &a);

  Entry d{'d', {}};
  TAILQ_INSERT_AFTER(&head, &b, &d, entries);
  ASSERT_THAT(head, Contains("abdc"));

  TAILQ_REMOVE(&head, &d, entries);
  ASSERT_THAT(head, Contains("abc"));

  Entry *e, *tmp;
  TAILQ_FOREACH_SAFE(e, &head, entries, tmp) {
    if (e->c == 'b')
      TAILQ_REMOVE(&head, e, entries);
  }
  ASSERT_THAT(head, Contains("ac"));

  // Removing the last entry has to put the tail pointer back, or a later
  // insertion at the tail would write through a link that is gone.
  TAILQ_REMOVE(&head, &c, entries);
  TAILQ_INSERT_TAIL(&head, &c, entries);
  ASSERT_THAT(head, Contains("ac"));
  ASSERT_EQ(TAILQ_LAST(&head, Head), &c);

  Head other = TAILQ_HEAD_INITIALIZER(other);
  TAILQ_INIT(&other);
  Entry x{'x', {}};
  TAILQ_INSERT_TAIL(&other, &x, entries);

  TAILQ_SWAP(&head, &other, Entry, entries);
  ASSERT_THAT(head, Contains("x"));
  ASSERT_THAT(other, Contains("ac"));

  TAILQ_CONCAT(&head, &other, entries);
  ASSERT_THAT(head, Contains("xac"));
  ASSERT_TRUE(TAILQ_EMPTY(&other));
  ASSERT_EQ(TAILQ_LAST(&head, Head), &c);
}

TEST(LlvmLibcQueueTest, STailQ) {
  struct Entry {
    char c;
    STAILQ_ENTRY(Entry) entries;
  };

  STAILQ_HEAD(Head, Entry);

  Head head = STAILQ_HEAD_INITIALIZER(head);

  struct Contains : public testing::Matcher<Head> {
    string s;
    Contains(string s) : s(s) {}
    bool match(Head head) {
      Entry *e;
      CharVector v;
      STAILQ_FOREACH(e, &head, entries) { v.append(e->c); }
      return s == v.c_str();
    }
  };

  STAILQ_INIT(&head);
  ASSERT_TRUE(STAILQ_EMPTY(&head));

  Entry e1 = {'a', {NULL}};
  STAILQ_INSERT_HEAD(&head, &e1, entries);

  ASSERT_THAT(head, Contains("a"));

  Entry e2 = {'b', {NULL}};
  STAILQ_INSERT_TAIL(&head, &e2, entries);

  ASSERT_THAT(head, Contains("ab"));

  Entry e3 = {'c', {NULL}};
  STAILQ_INSERT_AFTER(&head, &e2, &e3, entries);

  ASSERT_THAT(head, Contains("abc"));

  Head head2 = STAILQ_HEAD_INITIALIZER(head);

  Entry e4 = {'d', {NULL}};
  STAILQ_INSERT_HEAD(&head2, &e4, entries);

  ASSERT_THAT(head2, Contains("d"));

  STAILQ_SWAP(&head, &head2, Entry);

  ASSERT_THAT(head2, Contains("abc"));

  STAILQ_CONCAT(&head2, &head, Entry, entries);

  ASSERT_EQ(STAILQ_FIRST(&head2), &e1);
  ASSERT_EQ(STAILQ_LAST(&head2, Entry, entries), &e4);

  ASSERT_THAT(head2, Contains("abcd"));

  STAILQ_CONCAT(&head, &head2, Entry, entries);

  ASSERT_EQ(STAILQ_FIRST(&head), &e1);
  ASSERT_EQ(STAILQ_LAST(&head, Entry, entries), &e4);

  ASSERT_THAT(head, Contains("abcd"));

  Entry *e = NULL, *tmp = NULL;
  STAILQ_FOREACH_SAFE(e, &head, entries, tmp) {
    if (e == &e2) {
      STAILQ_REMOVE(&head, e, Entry, entries);
    }
  }

  ASSERT_THAT(head, Contains("acd"));

  while (!STAILQ_EMPTY(&head)) {
    e = STAILQ_FIRST(&head);
    STAILQ_REMOVE_HEAD(&head, entries);
  }

  ASSERT_TRUE(STAILQ_EMPTY(&head));
}

} // namespace LIBC_NAMESPACE_DECL
