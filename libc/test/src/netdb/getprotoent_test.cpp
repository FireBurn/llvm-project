//===-- Unittests for the protocols database ------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/types/struct_protoent.h"
#include "src/netdb/endprotoent.h"
#include "src/netdb/getprotobyname.h"
#include "src/netdb/getprotobynumber.h"
#include "src/netdb/getprotoent.h"
#include "src/netdb/setprotoent.h"
#include "src/string/strcmp.h"
#include "test/UnitTest/Test.h"

// What is read comes from /etc/protocols, whose contents are the system's to
// decide. The tests below say what has to hold of whatever it names, rather
// than assuming any particular entry is there.

TEST(LlvmLibcGetProtoEntTest, NoNameIsNoAnswer) {
  ASSERT_TRUE(LIBC_NAMESPACE::getprotobyname(nullptr) == nullptr);
  ASSERT_TRUE(LIBC_NAMESPACE::getprotobyname("no such protocol") == nullptr);
}

// A number the file does not name is not found. The file is not limited to
// what an IP header can carry, but nothing reaches this far.
TEST(LlvmLibcGetProtoEntTest, NumbersTheFileDoesNotNameAreNotFound) {
  ASSERT_TRUE(LIBC_NAMESPACE::getprotobynumber(-1) == nullptr);
  ASSERT_TRUE(LIBC_NAMESPACE::getprotobynumber(1 << 20) == nullptr);
}

// Where the file names a protocol, the two ways of asking about it agree.
TEST(LlvmLibcGetProtoEntTest, NameAndNumberAgree) {
  struct protoent *by_name = LIBC_NAMESPACE::getprotobyname("tcp");
  if (by_name == nullptr)
    return;
  ASSERT_STREQ(by_name->p_name, "tcp");
  const int number = by_name->p_proto;

  struct protoent *by_number = LIBC_NAMESPACE::getprotobynumber(number);
  ASSERT_TRUE(by_number != nullptr);
  ASSERT_EQ(by_number->p_proto, number);
  ASSERT_STREQ(by_number->p_name, "tcp");
}

// The names a protocol also goes by are reachable, and asking by one of them
// gives the same entry.
TEST(LlvmLibcGetProtoEntTest, AliasesLeadBackToTheSameEntry) {
  struct protoent *entry = LIBC_NAMESPACE::getprotobyname("tcp");
  if (entry == nullptr || entry->p_aliases[0] == nullptr)
    return;
  const int number = entry->p_proto;
  // The array ends in a null pointer, so walking it terminates.
  size_t count = 0;
  for (; entry->p_aliases[count] != nullptr; ++count)
    ;
  ASSERT_GT(count, size_t(0));

  struct protoent *again = LIBC_NAMESPACE::getprotobyname("TCP");
  if (again != nullptr)
    ASSERT_EQ(again->p_proto, number);
}

// Reading the file through gives entries until it runs out, and starting
// again gives the same first one.
TEST(LlvmLibcGetProtoEntTest, EnumerationStartsAgainFromTheFront) {
  LIBC_NAMESPACE::setprotoent(1);
  struct protoent *first = LIBC_NAMESPACE::getprotoent();
  if (first == nullptr) {
    LIBC_NAMESPACE::endprotoent();
    return;
  }
  const int first_number = first->p_proto;

  // Whatever comes next, the walk ends rather than running on.
  int seen = 1;
  while (LIBC_NAMESPACE::getprotoent() != nullptr && seen < 4096)
    ++seen;
  ASSERT_LT(seen, 4096);
  LIBC_NAMESPACE::endprotoent();

  LIBC_NAMESPACE::setprotoent(0);
  struct protoent *again = LIBC_NAMESPACE::getprotoent();
  ASSERT_TRUE(again != nullptr);
  ASSERT_EQ(again->p_proto, first_number);
  LIBC_NAMESPACE::endprotoent();
}

// Ending an enumeration that was never started is harmless.
TEST(LlvmLibcGetProtoEntTest, EndingWithoutStartingIsHarmless) {
  LIBC_NAMESPACE::endprotoent();
  LIBC_NAMESPACE::endprotoent();
}
