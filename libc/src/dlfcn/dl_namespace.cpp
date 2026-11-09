//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linker namespaces
///
//===----------------------------------------------------------------------===//

#include "src/dlfcn/dl_namespace.h"

#include "hdr/func/malloc.h"
#include "hdr/limits_macros.h"
#include "include/llvm-libc-macros/android-dlext-macros.h"
#include "src/__support/OSUtil/linux/realpath.h"
#include "src/__support/elf/passive_abi.h"
#include "src/__support/macros/config.h"
#include "src/dlfcn/dl_internal.h"

namespace LIBC_NAMESPACE_DECL {
namespace dl {

namespace {

Namespace default_ns = {const_cast<char *>("default"),
                        false,
                        true,
                        nullptr,
                        nullptr,
                        nullptr,
                        nullptr,
                        nullptr,
                        nullptr};
// Every namespace made, the default first, so that a pointer a caller gives
// can be checked before it is followed.
Namespace *registry_head = &default_ns;
Namespace *registry_tail = &default_ns;
Namespace *anonymous = nullptr;

size_t length_of(const char *s) {
  size_t n = 0;
  while (s[n] != '\0')
    ++n;
  return n;
}

bool equal(const char *a, const char *b) {
  if (a == nullptr || b == nullptr)
    return false;
  for (; *a == *b; ++a, ++b)
    if (*a == '\0')
      return true;
  return false;
}

// A copy of `text` that is never freed, or null for a null or empty text.
char *copy(const char *text, bool &failed) {
  if (text == nullptr || text[0] == '\0')
    return nullptr;
  const size_t size = length_of(text) + 1;
  char *out = static_cast<char *>(malloc(size));
  if (out == nullptr) {
    failed = true;
    return nullptr;
  }
  for (size_t i = 0; i < size; ++i)
    out[i] = text[i];
  return out;
}

// `a` and `b` joined with a colon, where either may be empty.
char *join(const char *a, const char *b, bool &failed) {
  const size_t la = a != nullptr ? length_of(a) : 0;
  const size_t lb = b != nullptr ? length_of(b) : 0;
  if (la + lb == 0)
    return nullptr;
  char *out = static_cast<char *>(malloc(la + lb + 2));
  if (out == nullptr) {
    failed = true;
    return nullptr;
  }
  size_t n = 0;
  for (size_t i = 0; i < la; ++i)
    out[n++] = a[i];
  if (la != 0 && lb != 0)
    out[n++] = ':';
  for (size_t i = 0; i < lb; ++i)
    out[n++] = b[i];
  out[n] = '\0';
  return out;
}

// Calls `visit` with each colon separated entry of `list`, and its length,
// until it returns true.
template <typename F> bool any_entry(const char *list, F visit) {
  if (list == nullptr)
    return false;
  for (const char *p = list; *p != '\0';) {
    const char *end = p;
    while (*end != '\0' && *end != ':')
      ++end;
    if (end != p && visit(p, static_cast<size_t>(end - p)))
      return true;
    p = *end == '\0' ? end : end + 1;
  }
  return false;
}

// Whether `path` is `dir`, less any trailing slashes, followed by a slash and
// a rest which has a further slash in it only when `recursive`.
bool inside(const char *path, const char *dir, size_t dir_size,
            bool recursive) {
  while (dir_size > 1 && dir[dir_size - 1] == '/')
    --dir_size;
  size_t i = 0;
  for (; i < dir_size; ++i)
    if (path[i] != dir[i])
      return false;
  if (dir_size == 1 && dir[0] == '/')
    i = 0;
  else if (path[i] != '/')
    return false;
  ++i;
  if (path[i] == '\0')
    return false;
  if (!recursive)
    for (const char *p = path + i; *p != '\0'; ++p)
      if (*p == '/')
        return false;
  return true;
}

} // anonymous namespace

Namespace *default_namespace() { return &default_ns; }

Namespace *anonymous_namespace() { return anonymous; }

bool valid_namespace(const Namespace *ns) {
  for (const Namespace *n = registry_head; n != nullptr; n = n->next)
    if (n == ns)
      return true;
  return false;
}

Namespace *namespace_of_address(ElfW(Addr) address, bool or_anonymous) {
  elf::ModuleSet *set = elf::process_modules();
  if (set != nullptr && set->linked)
    for (size_t i = 0; i < set->count; ++i)
      if (set->modules[i].contains(address))
        return canonical(static_cast<Namespace *>(set->mappings[i].ns));
  return or_anonymous && anonymous != nullptr ? anonymous : &default_ns;
}

bool namespace_sees_loaded(const Namespace *from, const Namespace *owner) {
  from = canonical(const_cast<Namespace *>(from));
  owner = canonical(const_cast<Namespace *>(owner));
  for (const Namespace *n = from; n != nullptr; n = n->shared_from)
    if (n == owner)
      return true;
  return false;
}

Namespace *find_namespace(const char *name) {
  for (Namespace *n = registry_head; n != nullptr; n = n->next)
    if (equal(n->name, name))
      return n;
  return nullptr;
}

Namespace *find_exported_namespace(const char *name) {
  Namespace *n = find_namespace(name);
  return n != nullptr && n->exported ? n : nullptr;
}

void export_namespace(Namespace *ns) { ns->exported = true; }

static Namespace *make(const char *name, bool &failed) {
  Namespace *ns = static_cast<Namespace *>(malloc(sizeof(Namespace)));
  if (ns == nullptr) {
    failed = true;
    return nullptr;
  }
  *ns = Namespace{};
  ns->name = copy(name, failed);
  return ns;
}

static void add(Namespace *ns) {
  registry_tail->next = ns;
  registry_tail = ns;
}

bool link_allows(const NamespaceLink &link, const char *soname) {
  if (link.sonames == nullptr)
    return true;
  return any_entry(link.sonames, [&](const char *entry, size_t size) {
    size_t i = 0;
    for (; i < size && soname[i] == entry[i]; ++i)
      ;
    return i == size && soname[i] == '\0';
  });
}

bool namespace_provides(const Namespace *from, const Namespace *owner,
                        const char *soname) {
  from = canonical(const_cast<Namespace *>(from));
  owner = canonical(const_cast<Namespace *>(owner));
  if (namespace_sees_loaded(from, owner))
    return true;
  for (const Namespace *n = from; n != nullptr; n = n->shared_from)
    for (const NamespaceLink *l = n->links; l != nullptr; l = l->next)
      if (l->target == owner && link_allows(*l, soname))
        return true;
  return false;
}

Namespace *create_namespace(const char *name, const char *ld_library_path,
                            const char *default_library_path, uint64_t type,
                            const char *permitted_paths, Namespace *parent) {
  if (name == nullptr || name[0] == '\0') {
    set_error("a namespace needs a name");
    return nullptr;
  }
  if (find_namespace(name) != nullptr) {
    set_error(name, "a namespace of that name already exists");
    return nullptr;
  }
  parent = canonical(parent);
  const bool shared = (type & ANDROID_NAMESPACE_TYPE_SHARED) != 0;

  bool failed = false;
  Namespace *ns = make(name, failed);
  if (ns != nullptr) {
    ns->isolated = (type & ANDROID_NAMESPACE_TYPE_ISOLATED) != 0;
    if (shared) {
      // What it can be found in and linked to is the parent's as well.
      ns->shared_from = parent;
      ns->ld_library_path =
          join(ld_library_path, parent->ld_library_path, failed);
      ns->default_library_path =
          join(default_library_path, parent->default_library_path, failed);
      ns->permitted_paths =
          join(permitted_paths, parent->permitted_paths, failed);
    } else {
      ns->ld_library_path = copy(ld_library_path, failed);
      ns->default_library_path = copy(default_library_path, failed);
      ns->permitted_paths = copy(permitted_paths, failed);
    }
  }
  if (failed || ns == nullptr) {
    set_error("cannot make room for a namespace");
    return nullptr;
  }
  add(ns);
  if ((type & ANDROID_NAMESPACE_TYPE_ALSO_USED_AS_ANONYMOUS) != 0)
    anonymous = ns;
  return ns;
}

bool link_namespaces(Namespace *from, Namespace *to, const char *sonames) {
  from = canonical(from);
  to = canonical(to);
  if (!valid_namespace(from) || !valid_namespace(to)) {
    set_error("not a namespace");
    return false;
  }
  if (from == to) {
    set_error("a namespace cannot be linked to itself");
    return false;
  }
  bool failed = false;
  auto *link = static_cast<NamespaceLink *>(malloc(sizeof(NamespaceLink)));
  if (link == nullptr) {
    set_error("cannot make room for a namespace link");
    return false;
  }
  link->target = to;
  link->sonames = copy(sonames, failed);
  link->next = nullptr;
  // An empty list lets nothing through, which is not the same as none.
  if (sonames != nullptr && link->sonames == nullptr && !failed) {
    link->sonames = static_cast<char *>(malloc(1));
    if (link->sonames == nullptr)
      failed = true;
    else
      link->sonames[0] = '\0';
  }
  if (failed) {
    set_error("cannot make room for a namespace link");
    return false;
  }
  NamespaceLink **at = &from->links;
  while (*at != nullptr)
    at = &(*at)->next;
  *at = link;
  return true;
}

bool init_anonymous_namespace(const char *shared_libs_sonames,
                              const char *library_search_path) {
  if (anonymous != nullptr) {
    set_error("the anonymous namespace was already made");
    return false;
  }
  Namespace *ns =
      create_namespace("(anonymous)", library_search_path, nullptr,
                       ANDROID_NAMESPACE_TYPE_ISOLATED, nullptr, nullptr);
  if (ns == nullptr)
    return false;
  if (!link_namespaces(ns, &default_ns, shared_libs_sonames))
    return false;
  anonymous = ns;
  return true;
}

bool path_accessible(const Namespace &ns, const char *path) {
  if (!ns.isolated)
    return true;
  char resolved[PATH_MAX];
  if (!internal::realpath_impl(path, resolved))
    return false;
  // A directory may itself be reached through a link, so what it comes to
  // counts as well as how it was written.
  auto in_list = [&](const char *list, bool recursive) {
    return any_entry(list, [&](const char *dir, size_t size) {
      if (inside(resolved, dir, size, recursive))
        return true;
      char text[PATH_MAX];
      char real[PATH_MAX];
      if (size >= sizeof(text))
        return false;
      for (size_t i = 0; i < size; ++i)
        text[i] = dir[i];
      text[size] = '\0';
      return internal::realpath_impl(text, real).has_value() &&
             inside(resolved, real, length_of(real), recursive);
    });
  };
  return in_list(ns.ld_library_path, false) ||
         in_list(ns.default_library_path, false) ||
         in_list(ns.permitted_paths, true);
}

} // namespace dl
} // namespace LIBC_NAMESPACE_DECL
