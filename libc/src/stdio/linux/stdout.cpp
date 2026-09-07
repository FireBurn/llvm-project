//===--- Definition of Linux stdout ---------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdio/stdout.h"

#include "hdr/types/FILE.h"

#ifdef LIBC_FULL_BUILD

#include "src/__support/File/linux/file.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

constexpr size_t STDOUT_BUFFER_SIZE = 1024;
uint8_t stdout_buffer[STDOUT_BUFFER_SIZE];
// C leaves this fully buffered unless it is over an interactive device. Which
// it is over cannot be known here, so the mode is settled on the first write.
static LinuxFile StdOut(1, stdout_buffer, STDOUT_BUFFER_SIZE, _IOFBF, false,
                        File::ModeFlags(File::OpenMode::APPEND),
                        /*static_stream=*/true,
                        /*settle_buffer_mode=*/true);

LLVM_LIBC_VARIABLE(FILE *, stdout) = reinterpret_cast<FILE *>(&StdOut);

} // namespace LIBC_NAMESPACE_DECL

#else // overlay mode

extern "C" FILE *stdout;

#endif // LIBC_FULL_BUILD
