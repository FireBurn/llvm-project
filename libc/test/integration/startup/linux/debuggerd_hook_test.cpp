//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Integration test for the debuggerd handler installed at startup.
///
//===----------------------------------------------------------------------===//

#include "src/android/android_crash_detail_register.h"
#include "src/android/android_set_abort_message.h"
#include "src/android/debuggerd_hook.h"
#include "test/IntegrationTest/test.h"

namespace dh = LIBC_NAMESPACE::debuggerd_hook;

static dh::Callbacks *received = nullptr;
static int calls = 0;

// What AOSP's libdebuggerd_handler provides. The startup code finds it by
// name, as it does in a program that links the real one.
extern "C" void debuggerd_init(dh::Callbacks *callbacks) {
  received = callbacks;
  ++calls;
}

TEST_MAIN() {
  // Startup has already run by here.
  ASSERT_EQ(calls, 1);
  ASSERT_TRUE(received != nullptr);
  ASSERT_TRUE(received->get_process_info != nullptr);

  // Nothing has been set yet, so there is no abort message to report.
  dh::ProcessInfo before = received->get_process_info();
  ASSERT_TRUE(before.abort_msg == nullptr);

  static const char name[] = "key", value[] = "value";
  ASSERT_TRUE(LIBC_NAMESPACE::android_crash_detail_register(
                  name, sizeof(name) - 1, value, sizeof(value) - 1) != nullptr);
  LIBC_NAMESPACE::android_set_abort_message("reason");

  dh::ProcessInfo after = received->get_process_info();
  ASSERT_TRUE(after.crash_detail_page != nullptr);
  ASSERT_TRUE(after.abort_msg != nullptr);
  ASSERT_EQ(*static_cast<size_t *>(after.abort_msg), sizeof(size_t) + 7);
  return 0;
}
