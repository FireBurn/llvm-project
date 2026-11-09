//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linker namespaces, as Android's loader has them: a set of directories a
/// library may be found in, whether anything outside them may be, and which
/// other namespaces it may borrow libraries from.
///
/// Every function here expects dl_mutex to be held.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_DLFCN_DL_NAMESPACE_H
#define LLVM_LIBC_SRC_DLFCN_DL_NAMESPACE_H

#include "hdr/stdint_proxy.h"
#include "hdr/types/size_t.h"
#include "src/__support/elf/passive_abi.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace dl {

struct Namespace;

// What a namespace may take from another one it is linked to.
struct NamespaceLink {
  Namespace *target;
  // Colon separated sonames, or null when every library may be taken.
  char *sonames;
  NamespaceLink *next;
};

struct Namespace {
  char *name;
  // Libraries are only taken from the search paths and the permitted paths.
  bool isolated;
  // Whether android_get_exported_namespace hands it out.
  bool exported;
  // Whatever is loaded in this namespace is also seen as loaded in the one it
  // was made from.
  Namespace *shared_from;
  // Colon separated directory lists. A library has to be directly inside one
  // of the first two, and anywhere below one of the third, to be taken into
  // an isolated namespace.
  char *ld_library_path;
  char *default_library_path;
  char *permitted_paths;
  NamespaceLink *links;
  Namespace *next;
};

// The namespace the program and everything it was started with is in. Modules
// that record no namespace are in this one.
Namespace *default_namespace();

// The namespace for code that is not in any module, or null until
// android_init_anonymous_namespace made one.
Namespace *anonymous_namespace();

// The namespace of the module that `address` is inside. Where there is no such
// module, which is code that was not loaded by this, it is the anonymous
// namespace when `or_anonymous` and there is one, and the default one
// otherwise.
Namespace *namespace_of_address(ElfW(Addr) address, bool or_anonymous);

// Whether `ns` is one that this created, which is all a caller can know
// about a pointer it was handed.
bool valid_namespace(const Namespace *ns);

// Null stands for the default namespace.
LIBC_INLINE Namespace *canonical(Namespace *ns) {
  return ns != nullptr ? ns : default_namespace();
}

// Whether a library loaded in `owner` counts as loaded in `from`.
bool namespace_sees_loaded(const Namespace *from, const Namespace *owner);

// Makes a namespace. `type` is the ANDROID_NAMESPACE_TYPE_ bits. Returns null,
// with the error set, when there is no memory or the name is taken.
Namespace *create_namespace(const char *name, const char *ld_library_path,
                            const char *default_library_path, uint64_t type,
                            const char *permitted_paths, Namespace *parent);

// Lets `from` take the libraries named in `sonames` (colon separated, null for
// all of them) from `to`.
bool link_namespaces(Namespace *from, Namespace *to, const char *sonames);

// Makes the anonymous namespace: isolated, with `library_search_path`, and
// linked to the default one for `shared_libs_sonames`.
bool init_anonymous_namespace(const char *shared_libs_sonames,
                              const char *library_search_path);

// Marks it as one android_get_exported_namespace finds by name.
void export_namespace(Namespace *ns);
Namespace *find_exported_namespace(const char *name);
Namespace *find_namespace(const char *name);

// Whether `link` lets the library called `soname` through.
bool link_allows(const NamespaceLink &link, const char *soname);

// Whether `owner`'s libraries are reachable from `from` under the name
// `soname`: it is `from` itself, one it is shared from, or one it is linked to
// for that name.
bool namespace_provides(const Namespace *from, const Namespace *owner,
                        const char *soname);

// Whether an isolated namespace may load the file at `path`. Always true for
// a namespace that is not isolated.
bool path_accessible(const Namespace &ns, const char *path);

} // namespace dl
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_DLFCN_DL_NAMESPACE_H
