//===-- Implementation of dlinfo ------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "dlinfo.h"

#include "hdr/stdint_proxy.h"
#include "src/__support/CPP/mutex.h"
#include "src/__support/common.h"
#include "src/__support/elf/passive_abi.h"
#include "src/__support/macros/config.h"
#include "src/dlfcn/dl_address.h"
#include "src/dlfcn/dl_internal.h"

namespace LIBC_NAMESPACE_DECL {

namespace {

// The requests, with the values <dlfcn.h> gives them.
constexpr int DI_LMID = 1;
constexpr int DI_LINKMAP = 2;
constexpr int DI_ORIGIN = 6;

} // anonymous namespace

// Answers the requests which describe a loaded object: its link map, its
// namespace, which is always the first, and the directory it came from.
LLVM_LIBC_FUNCTION(int, dlinfo,
                   (void *__restrict handle, int request,
                    void *__restrict info)) {
  elf::ModuleSet *modules = elf::process_modules();
  cpp::lock_guard lock(dl::dl_mutex);

  if (modules == nullptr || !modules->linked) {
    dl::set_error("dlinfo is only available in a dynamically linked process");
    return -1;
  }
  size_t index = 0;
  if (!dl::index_for(handle, index)) {
    dl::set_error("dlinfo was given a handle that dlopen did not return");
    return -1;
  }
  if (info == nullptr) {
    dl::set_error("dlinfo was given nowhere to write the answer");
    return -1;
  }

  if (request == DI_LMID) {
    *static_cast<long *>(info) = 0;
    return 0;
  }

  struct link_map *map = dl::link_map_for(modules->modules[index].load_bias());
  if (map == nullptr) {
    dl::set_error("dlinfo could not find the object");
    return -1;
  }

  if (request == DI_LINKMAP) {
    *static_cast<struct link_map **>(info) = map;
    return 0;
  }

  if (request == DI_ORIGIN) {
    // The directory of the path, without the final slash; "/" for the root.
    // The caller's buffer is a path's length, as it is for every dlinfo.
    const char *name = map->l_name;
    if (name == nullptr || name[0] == '\0') {
      dl::set_error("dlinfo does not know where the program came from");
      return -1;
    }
    size_t last = 0;
    for (size_t i = 0; name[i] != '\0'; ++i)
      if (name[i] == '/')
        last = i;
    char *out = static_cast<char *>(info);
    if (last == 0) {
      out[0] = name[0] == '/' ? '/' : '.';
      out[1] = '\0';
      return 0;
    }
    for (size_t i = 0; i < last; ++i)
      out[i] = name[i];
    out[last] = '\0';
    return 0;
  }

  dl::set_error("dlinfo was given a request it does not support");
  return -1;
}

} // namespace LIBC_NAMESPACE_DECL
