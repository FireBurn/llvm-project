//===-- Unittests for gai_strerror and herror -----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/netdb/__h_errno_location.h"
#include "src/netdb/gai_strerror.h"
#include "src/netdb/herror.h"
#include "src/string/strlen.h"
#include "test/UnitTest/Test.h"

#include <netdb.h>

// Every code the lookup calls report has a message of its own, and one that
// is not a code still has something to say rather than nothing.
TEST(LlvmLibcGaiStrerrorTest, NamesEveryCode) {
  constexpr int CODES[] = {EAI_BADFLAGS, EAI_NONAME,   EAI_AGAIN,   EAI_FAIL,
                           EAI_FAMILY,   EAI_SOCKTYPE, EAI_SERVICE, EAI_MEMORY,
                           EAI_SYSTEM,   EAI_OVERFLOW};
  for (int code : CODES) {
    const char *message = LIBC_NAMESPACE::gai_strerror(code);
    ASSERT_FALSE(message == nullptr);
    ASSERT_GT(LIBC_NAMESPACE::strlen(message), size_t(0));
  }

  const char *success = LIBC_NAMESPACE::gai_strerror(0);
  ASSERT_FALSE(success == nullptr);
  ASSERT_GT(LIBC_NAMESPACE::strlen(success), size_t(0));

  const char *unknown = LIBC_NAMESPACE::gai_strerror(12345);
  ASSERT_FALSE(unknown == nullptr);
  ASSERT_GT(LIBC_NAMESPACE::strlen(unknown), size_t(0));
}

// herror writes to standard error and reports nothing back, so what a test
// can say is that it runs over every value h_errno takes, and over a null
// prefix, without reaching for anything that is not there.
TEST(LlvmLibcHerrorTest, WritesAMessageForEveryValue) {
  constexpr int VALUES[] = {0,           HOST_NOT_FOUND, TRY_AGAIN,
                            NO_RECOVERY, NO_DATA,        12345};
  for (int value : VALUES) {
    *LIBC_NAMESPACE::__h_errno_location() = value;
    LIBC_NAMESPACE::herror("llvm-libc test");
    LIBC_NAMESPACE::herror(nullptr);
  }
  *LIBC_NAMESPACE::__h_errno_location() = 0;
}
