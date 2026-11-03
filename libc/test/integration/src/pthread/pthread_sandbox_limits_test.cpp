#include "hdr/types/struct_rlimit.h"
#include "src/__support/OSUtil/syscall.h"
#include "src/compat/__llvm_libc_init_main_thread.h"
#include "src/pthread/pthread_attr_destroy.h"
#include "src/pthread/pthread_create.h"
#include "src/pthread/pthread_getattr_np.h"
#include "src/pthread/pthread_join.h"
#include "src/pthread/pthread_self.h"
#include "src/sys/resource/getrlimit.h"
#include "src/sys/resource/setrlimit.h"
#include "src/unistd/sysconf.h"
#include "test/IntegrationTest/test.h"

// Firefox's and Chromium's sandboxes let a process read its own limits with
// getrlimit and kill it for prlimit64, which reaches other processes. Firefox's
// utility processes refuse getrlimit as well, once they are running, so the
// stack limit has to have been read at startup for threads to be made and
// described after that.

TEST_MAIN() {
  // The loader does this before the initialisers of what it loaded, and
  // with it the stack limit is read.
  LIBC_NAMESPACE::__llvm_libc_init_main_thread();

  refuse(SYS_prlimit64);

  struct rlimit limit;
  ASSERT_EQ(LIBC_NAMESPACE::getrlimit(RLIMIT_CORE, &limit), 0);
  // Setting a limit to what it already is needs no privilege.
  ASSERT_EQ(LIBC_NAMESPACE::setrlimit(RLIMIT_CORE, &limit), 0);
  ASSERT_TRUE(LIBC_NAMESPACE::sysconf(_SC_OPEN_MAX) != 0);
  ASSERT_TRUE(LIBC_NAMESPACE::sysconf(_SC_CHILD_MAX) != 0);
  ASSERT_TRUE(LIBC_NAMESPACE::sysconf(_SC_ARG_MAX) != 0);

  // Now no limit may be read at all.
  refuse(SYS_getrlimit);

  pthread_attr_t attr;
  ASSERT_EQ(
      LIBC_NAMESPACE::pthread_getattr_np(LIBC_NAMESPACE::pthread_self(), &attr),
      0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_attr_destroy(&attr), 0);

  // Creating a thread needs the stack limit for its default size.
  pthread_t thread;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_create(&thread, nullptr, idle, nullptr), 0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_join(thread, nullptr), 0);
  return 0;
}
