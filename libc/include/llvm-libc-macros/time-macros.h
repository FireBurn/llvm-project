#ifndef LLVM_LIBC_MACROS_TIME_MACROS_H
#define LLVM_LIBC_MACROS_TIME_MACROS_H

// A GPU target is the library's own only when there is no operating system.
// Code for a device that is compiled alongside the host's, as HIP and CUDA
// are, has __linux__ defined and reads the host's headers.
#if defined(__linux__)
#include "linux/time-macros.h"
#elif defined(__AMDGPU__) || defined(__NVPTX__) || defined(__SPIRV__)
#include "gpu/time-macros.h"
#elif defined(__ELF__)
#include "baremetal/time-macros.h"
#elif defined(__APPLE__)
#include "darwin/time-macros.h"
#else
#define CLOCKS_PER_SEC 1000000
#endif

#define CLK_TCK CLOCKS_PER_SEC

#define TIME_UTC 1
#define TIME_MONOTONIC 2
#define TIME_ACTIVE 3
#define TIME_THREAD_ACTIVE 4

#endif // LLVM_LIBC_MACROS_TIME_MACROS_H
