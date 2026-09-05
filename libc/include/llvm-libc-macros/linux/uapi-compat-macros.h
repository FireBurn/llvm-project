//===-- Telling the kernel headers what this libc already defines ---------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// The kernel's UAPI headers define a good deal of what a libc has to define
/// too: the address structures, the protocol numbers, the interface request
/// structures. <linux/libc-compat.h> exists so that the two do not fight.
/// Each contested definition has a guard named __UAPI_DEF_something; the
/// kernel sets it to 1 unless it is already set, and skips its own
/// definition where it is 0. It recognises glibc by that libc's include
/// guards, and leaves the guards to the libc otherwise.
///
/// So each header here which defines one of them decides, from the guard and
/// from whether the kernel header which would define it has been read, which
/// of the two definitions to take, and then sets the guard to zero to say
/// the question is settled.
///
/// Nothing is set here in advance: a program which reaches for a kernel
/// header on its own still gets the kernel's definitions. The guards live
/// beside the definitions they are about, which is what makes the answer the
/// same whichever header is read first.
///
//===----------------------------------------------------------------------===//
