//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Entry point of the startup dynamic linker
///
//===----------------------------------------------------------------------===//

#include "loader/linux/loader_main.h"

#include "src/__support/OSUtil/io.h"
#include "src/__support/OSUtil/syscall.h"

#include "src/__support/elf/load_module.h"
#include "src/__support/elf/startup_linker.h"
#include "src/__support/elf/startup_stack.h"
#include "src/__support/macros/config.h"
#include <sys/syscall.h>

namespace LIBC_NAMESPACE_DECL {

namespace {

// The kernel always supplies AT_PAGESZ, but a sane value costs nothing.
constexpr size_t FALLBACK_PAGE_SIZE = 4096;

// The loader deliberately depends on nothing outside itself, so this is the
// syscall rather than libc's exit: there is no libc yet.
[[noreturn]] void die(const char *message) {
  write_to_stderr(message);
  for (;;)
    syscall_impl<long>(SYS_exit_group, 127);
}

} // anonymous namespace

// Supplied by the linker: where the loader itself was mapped.
extern "C" const char __ehdr_start[];

namespace {

// Called when the loader was run as a command, `ld.so program args`, so the
// kernel described the loader to itself instead of a program. Maps the named
// program and rewrites the stack to look as it would if the kernel had started
// that program through this loader: argv without the loader's own name, and an
// auxiliary vector that describes the program.
elf::StartupStack run_program_directly(void *stack_pointer, size_t page_size) {
  elf::StartupStack stack = elf::StartupStack::from(stack_pointer);
  if (stack.argc() < 2)
    die("usage: ld-llvm-libc-<arch>.so.1 program [arguments...]\n");

  const char *path = stack.argv()[1];
  auto program = elf::map_module(path, page_size, /*shared_object_only=*/true);
  if (!program.has_value())
    die("loader: cannot load the program, which must be a position "
        "independent executable\n");

  // Move everything after argv[0] down over it, so the stack pointer the
  // kernel gave stays the one the program is entered with. Only the last word
  // is left behind, past the end of the vector.
  auto *words = reinterpret_cast<uintptr_t *>(stack_pointer);
  const uintptr_t *end = reinterpret_cast<const uintptr_t *>(stack.auxv());
  while (end[0] != AT_NULL)
    end += 2;
  end += 2;
  for (uintptr_t *word = words + 2; word < end; ++word)
    word[-1] = word[0];
  words[0] = stack.argc() - 1;

  stack = elf::StartupStack::from(stack_pointer);
  const elf::LoadedModule &loaded = program.value();
  for (ElfW(auxv_t) *entry = stack.auxv(); entry->a_type != AT_NULL; ++entry) {
    switch (entry->a_type) {
    case AT_PHDR:
      entry->a_un.a_val = reinterpret_cast<uintptr_t>(loaded.phdrs);
      break;
    case AT_PHNUM:
      entry->a_un.a_val = loaded.phnum;
      break;
    case AT_ENTRY:
      entry->a_un.a_val = loaded.entry;
      break;
    case AT_BASE:
      entry->a_un.a_val = reinterpret_cast<uintptr_t>(__ehdr_start);
      break;
    case AT_EXECFN:
      entry->a_un.a_val = reinterpret_cast<uintptr_t>(stack.argv()[0]);
      break;
    }
  }
  return stack;
}

} // anonymous namespace

extern "C" {
// Debuggers break on this, and look for it in the loader by name.
[[gnu::noinline]] void __llvm_libc_debug_state() {
  asm volatile("" ::: "memory");
}
[[gnu::alias("__llvm_libc_debug_state"), gnu::visibility("default")]] void
_dl_debug_state();
}

uintptr_t loader_link(void *stack_pointer) {
  elf::StartupStack stack = elf::StartupStack::from(stack_pointer);
  const size_t page_size =
      static_cast<size_t>(stack.auxval(AT_PAGESZ).value_or(FALLBACK_PAGE_SIZE));

  // Only a loader which is the program has no interpreter base.
  if (stack.auxval(AT_BASE).value_or(0) == 0)
    stack = run_program_directly(stack_pointer, page_size);

  auto image = elf::executable_from(stack);
  if (!image)
    die("loader: the kernel did not describe the executable\n");

  // A program run directly rather than through this loader has no entry for
  // us to hand control to.
  if (image->entry == 0)
    die("loader: no entry point\n");

  elf::StartupLinker linker(page_size, stack.envp(), &__llvm_libc_debug_state);
  if (!linker.link(stack, *image))
    die("loader: linking failed\n");

  return image->entry;
}

} // namespace LIBC_NAMESPACE_DECL
