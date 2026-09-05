//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Tests that dl_iterate_phdr reports the shared objects a loader linked, and
/// those opened with dlopen, in a program started by its loader. The program
/// returns nonzero if any check fails.
///
//===----------------------------------------------------------------------===//

#include "check.h"

#include <dlfcn.h>
#include <link.h>

namespace {

struct Search {
  const char *name;
  unsigned long address;
  bool found_name;
  bool address_inside;
  int count;
};

bool ends_with(const char *text, const char *suffix) {
  unsigned long n = 0, m = 0;
  while (text[n] != '\0')
    ++n;
  while (suffix[m] != '\0')
    ++m;
  if (m > n)
    return false;
  for (unsigned long i = 0; i < m; ++i)
    if (text[n - m + i] != suffix[i])
      return false;
  return true;
}

int visit(struct dl_phdr_info *info, unsigned long, void *data) {
  Search *search = static_cast<Search *>(data);
  ++search->count;
  if (info->dlpi_name && ends_with(info->dlpi_name, search->name))
    search->found_name = true;
  for (int i = 0; i < info->dlpi_phnum; ++i) {
    const ElfW(Phdr) &p = info->dlpi_phdr[i];
    if (p.p_type != PT_LOAD)
      continue;
    unsigned long start = info->dlpi_addr + p.p_vaddr;
    if (search->address >= start && search->address < start + p.p_memsz)
      search->address_inside = true;
  }
  return 0;
}

} // namespace

static void the_libc_and_the_program_are_reported() {
  Search search = {"libc.so", reinterpret_cast<unsigned long>(&dlopen), false,
                   false, 0};
  dl_iterate_phdr(visit, &search);
  CHECK(search.count >= 2);
  CHECK(search.address_inside);
}

static void an_opened_object_is_reported_while_it_is_open() {
  const char *path = LIBC_TEST_DL_OBJECT;
  const char *base = path;
  for (const char *p = path; *p; ++p)
    if (*p == '/')
      base = p + 1;

  void *handle = dlopen(path, RTLD_NOW);
  CHECK(handle != nullptr);
  void *add = dlsym(handle, "llvm_libc_dl_add");
  CHECK(add != nullptr);

  Search search = {base, reinterpret_cast<unsigned long>(add), false, false, 0};
  dl_iterate_phdr(visit, &search);
  CHECK(search.found_name);
  // The bias reported for it is the one its symbols were relocated by.
  CHECK(search.address_inside);

  CHECK(dlclose(handle) == 0);
  Search after = {base, 0, false, false, 0};
  dl_iterate_phdr(visit, &after);
  CHECK(!after.found_name);
}

extern "C" int main() {
  the_libc_and_the_program_are_reported();
  an_opened_object_is_reported_while_it_is_open();
  return failures == 0 ? 0 : 1;
}
