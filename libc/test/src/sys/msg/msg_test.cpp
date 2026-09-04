//===-- Unittests for the System V message queue calls --------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/sys_ipc_macros.h"
#include "hdr/sys_msg_macros.h"
#include "hdr/types/struct_msqid_ds.h"
#include "src/__support/CPP/string_view.h"
#include "src/sys/msg/msgctl.h"
#include "src/sys/msg/msgget.h"
#include "src/sys/msg/msgrcv.h"
#include "src/sys/msg/msgsnd.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using namespace LIBC_NAMESPACE::testing::ErrnoSetterMatcher;
using LlvmLibcMsgTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using string_view = LIBC_NAMESPACE::cpp::string_view;

namespace {

// A message is a type the receiver can select on, then whatever it holds.
struct Note {
  long mtype;
  char text[32];
};

void fill(Note &note, long type, const char *text) {
  note.mtype = type;
  size_t i = 0;
  for (; text[i] != '\0' && i < sizeof(note.text) - 1; ++i)
    note.text[i] = text[i];
  note.text[i] = '\0';
}

} // anonymous namespace

TEST_F(LlvmLibcMsgTest, SendAndReceive) {
  int queue = LIBC_NAMESPACE::msgget(IPC_PRIVATE, IPC_CREAT | 0600);
  ASSERT_GE(queue, 0);
  ASSERT_ERRNO_SUCCESS();

  Note out;
  fill(out, 7, "first");
  ASSERT_THAT(LIBC_NAMESPACE::msgsnd(queue, &out, sizeof(out.text), 0),
              Succeeds(0));
  fill(out, 9, "second");
  ASSERT_THAT(LIBC_NAMESPACE::msgsnd(queue, &out, sizeof(out.text), 0),
              Succeeds(0));

  // A type picks out a message rather than taking the oldest one.
  Note in{};
  ASSERT_EQ(LIBC_NAMESPACE::msgrcv(queue, &in, sizeof(in.text), 9, 0),
            static_cast<ssize_t>(sizeof(in.text)));
  EXPECT_EQ(in.mtype, 9L);
  EXPECT_TRUE(string_view(in.text) == "second");

  // A type of zero takes whatever is at the front.
  ASSERT_EQ(LIBC_NAMESPACE::msgrcv(queue, &in, sizeof(in.text), 0, 0),
            static_cast<ssize_t>(sizeof(in.text)));
  EXPECT_EQ(in.mtype, 7L);
  EXPECT_TRUE(string_view(in.text) == "first");

  ASSERT_THAT(LIBC_NAMESPACE::msgctl(queue, IPC_RMID, nullptr), Succeeds(0));
}

TEST_F(LlvmLibcMsgTest, ReportsWhatIsOnTheQueue) {
  int queue = LIBC_NAMESPACE::msgget(IPC_PRIVATE, IPC_CREAT | 0600);
  ASSERT_GE(queue, 0);

  struct msqid_ds info;
  ASSERT_THAT(LIBC_NAMESPACE::msgctl(queue, IPC_STAT, &info), Succeeds(0));
  EXPECT_EQ(info.msg_qnum, size_t(0));
  EXPECT_EQ(info.msg_perm.mode & 0777, mode_t(0600));

  Note out;
  fill(out, 1, "one");
  ASSERT_THAT(LIBC_NAMESPACE::msgsnd(queue, &out, sizeof(out.text), 0),
              Succeeds(0));

  ASSERT_THAT(LIBC_NAMESPACE::msgctl(queue, IPC_STAT, &info), Succeeds(0));
  EXPECT_EQ(info.msg_qnum, size_t(1));

  ASSERT_THAT(LIBC_NAMESPACE::msgctl(queue, IPC_RMID, nullptr), Succeeds(0));
}

TEST_F(LlvmLibcMsgTest, AnEmptyQueueDoesNotBlockWithNowait) {
  int queue = LIBC_NAMESPACE::msgget(IPC_PRIVATE, IPC_CREAT | 0600);
  ASSERT_GE(queue, 0);

  Note in{};
  EXPECT_THAT(
      LIBC_NAMESPACE::msgrcv(queue, &in, sizeof(in.text), 0, IPC_NOWAIT),
      Fails(ENOMSG, ssize_t(-1)));

  ASSERT_THAT(LIBC_NAMESPACE::msgctl(queue, IPC_RMID, nullptr), Succeeds(0));
}

TEST_F(LlvmLibcMsgTest, AMessageTooLargeForTheBuffer) {
  int queue = LIBC_NAMESPACE::msgget(IPC_PRIVATE, IPC_CREAT | 0600);
  ASSERT_GE(queue, 0);

  Note out;
  fill(out, 1, "a message which does not fit");
  ASSERT_THAT(LIBC_NAMESPACE::msgsnd(queue, &out, sizeof(out.text), 0),
              Succeeds(0));

  struct Small {
    long mtype;
    char text[4];
  } in{};
  // Without MSG_NOERROR a message which will not fit is left where it is.
  EXPECT_THAT(LIBC_NAMESPACE::msgrcv(queue, &in, sizeof(in.text), 0, 0),
              Fails(E2BIG, ssize_t(-1)));
  // With it, what fits is taken and the rest thrown away.
  EXPECT_EQ(LIBC_NAMESPACE::msgrcv(queue, &in, sizeof(in.text), 0, MSG_NOERROR),
            static_cast<ssize_t>(sizeof(in.text)));

  ASSERT_THAT(LIBC_NAMESPACE::msgctl(queue, IPC_RMID, nullptr), Succeeds(0));
}

TEST_F(LlvmLibcMsgTest, ABadQueueIsReported) {
  EXPECT_THAT(LIBC_NAMESPACE::msgctl(-1, IPC_RMID, nullptr), Fails(EINVAL, -1));
  Note in{};
  EXPECT_THAT(LIBC_NAMESPACE::msgrcv(-1, &in, sizeof(in.text), 0, IPC_NOWAIT),
              Fails(EINVAL, ssize_t(-1)));
}
