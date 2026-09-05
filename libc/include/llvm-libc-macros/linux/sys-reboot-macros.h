//===-- Macros defined in sys/reboot.h header file ------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_LINUX_SYS_REBOOT_MACROS_H
#define LLVM_LIBC_MACROS_LINUX_SYS_REBOOT_MACROS_H

// The magic numbers the kernel wants as the first two arguments, so that a
// stray call cannot bring the machine down.
#define LINUX_REBOOT_MAGIC1 0xfee1dead
#define LINUX_REBOOT_MAGIC2 672274793
#define LINUX_REBOOT_MAGIC2A 85072278
#define LINUX_REBOOT_MAGIC2B 369367448
#define LINUX_REBOOT_MAGIC2C 537993216

// What to do, in the kernel's own spelling.
#define LINUX_REBOOT_CMD_RESTART 0x01234567
#define LINUX_REBOOT_CMD_HALT 0xCDEF0123
#define LINUX_REBOOT_CMD_CAD_ON 0x89ABCDEF
#define LINUX_REBOOT_CMD_CAD_OFF 0x00000000
#define LINUX_REBOOT_CMD_POWER_OFF 0x4321FEDC
#define LINUX_REBOOT_CMD_RESTART2 0xA1B2C3D4
#define LINUX_REBOOT_CMD_SW_SUSPEND 0xD000FCE2
#define LINUX_REBOOT_CMD_KEXEC 0x45584543

// The names the reboot wrapper takes, which are the ones programs use.
#define RB_AUTOBOOT LINUX_REBOOT_CMD_RESTART
#define RB_HALT_SYSTEM LINUX_REBOOT_CMD_HALT
#define RB_ENABLE_CAD LINUX_REBOOT_CMD_CAD_ON
#define RB_DISABLE_CAD LINUX_REBOOT_CMD_CAD_OFF
#define RB_POWER_OFF LINUX_REBOOT_CMD_POWER_OFF
#define RB_SW_SUSPEND LINUX_REBOOT_CMD_SW_SUSPEND
#define RB_KEXEC LINUX_REBOOT_CMD_KEXEC

#endif // LLVM_LIBC_MACROS_LINUX_SYS_REBOOT_MACROS_H
