//===-- Implementation of dlopen ------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "dlopen.h"

#include "hdr/fcntl_macros.h"
#include "hdr/func/free.h"
#include "hdr/func/malloc.h"
#include "hdr/types/struct_stat.h"
#include "src/__support/CPP/mutex.h"
#include "src/__support/CPP/optional.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/newfstatat.h"
#include "src/__support/common.h"
#include "src/__support/elf/bind.h"
#include "src/__support/elf/load_module.h"
#include "src/__support/elf/passive_abi.h"
#include "src/__support/elf/scope.h"
#include "src/__support/elf/search.h"
#include "src/__support/macros/config.h"
#include "src/dlfcn/dl_internal.h"
#include "src/dlfcn/dl_namespace.h"
#include "src/dlfcn/lazy_bind.h"
#include "src/link/link_maps.h"
#include "src/unistd/environ.h"

namespace LIBC_NAMESPACE_DECL {

using dl::canonical;
using dl::default_namespace;
using dl::link_allows;
using dl::Namespace;
using dl::namespace_of_address;
using dl::namespace_sees_loaded;
using dl::NamespaceLink;
using dl::path_accessible;

namespace {

bool same_name(const char *a, const char *b) {
  if (a == nullptr || b == nullptr)
    return false;
  for (; *a == *b; ++a, ++b)
    if (*a == '\0')
      return true;
  return false;
}

constexpr size_t PATH_BUFFER = 4096;

// The path with the "." and ".." components and the doubled slashes taken
// out, without asking the filesystem. Returns false when it does not fit.
bool normalise(const char *path, char *out) {
  size_t length = 0;
  const bool absolute = path[0] == '/';
  if (absolute)
    out[length++] = '/';
  const size_t root = length;
  for (const char *p = path; *p != '\0';) {
    while (*p == '/')
      ++p;
    const char *start = p;
    while (*p != '\0' && *p != '/')
      ++p;
    const size_t size = static_cast<size_t>(p - start);
    if (size == 0 || (size == 1 && start[0] == '.'))
      continue;
    if (size == 2 && start[0] == '.' && start[1] == '.') {
      // Back over the last component, unless it is one that stays.
      size_t end = length;
      if (end > root && out[end - 1] == '/')
        --end;
      size_t begin = end;
      while (begin > root && out[begin - 1] != '/')
        --begin;
      if (begin < end &&
          !(end - begin == 2 && out[begin] == '.' && out[begin + 1] == '.')) {
        length = begin;
        continue;
      }
      if (absolute)
        continue;
    }
    if (length + size + 2 >= PATH_BUFFER)
      return false;
    if (length > root && out[length - 1] != '/')
      out[length++] = '/';
    for (size_t i = 0; i < size; ++i)
      out[length++] = start[i];
  }
  out[length] = '\0';
  return true;
}

const char *last_component(const char *path) {
  const char *last = path;
  for (const char *p = path; *p != '\0'; ++p)
    if (*p == '/')
      last = p + 1;
  return last;
}

// Whether two paths name the same file: the same once their "." and ".."
// are taken out, or, when they end in the same name, the same device and
// inode, which is what a symbolic link comes to. An object reached by two
// paths has to be one object, as it holds the program's state of whatever it
// defines.
bool same_file(const char *known, const char *wanted) {
  if (known == nullptr || wanted == nullptr)
    return false;
  bool slash = false;
  for (const char *p = wanted; *p != '\0'; ++p)
    slash = slash || *p == '/';
  if (!slash || known[0] != '/')
    return false;
  char a[PATH_BUFFER];
  char b[PATH_BUFFER];
  if (!normalise(known, a) || !normalise(wanted, b))
    return false;
  if (same_name(a, b))
    return true;
  if (!same_name(last_component(a), last_component(b)))
    return false;
  struct stat one;
  struct stat two;
  if (!linux_syscalls::newfstatat(AT_FDCWD, a, &one, 0) ||
      !linux_syscalls::newfstatat(AT_FDCWD, b, &two, 0))
    return false;
  return one.st_dev == two.st_dev && one.st_ino == two.st_ino;
}

Namespace *owner_of(const elf::ModuleSet &set, size_t index) {
  return canonical(static_cast<Namespace *>(set.mappings[index].ns));
}

// Calls `visit` with each path the object `name` stands for could be at in
// `ns`, until it returns true. In the default namespace that is where a
// loader looks without namespaces: the asking object's run path, the
// environment's path, the system's path file and the default directories. In
// any other it is the namespace's own directories and the asking object's run
// path. A path is not offered where an isolated namespace may not load it.
template <typename Visit>
bool visit_candidates_in(const Namespace &ns, const char *name,
                         const elf::Module *requester, const char *origin,
                         const char *library_path,
                         elf::SystemPaths *system_paths, Visit &&visit) {
  if (name == nullptr)
    return false;
  const bool is_default = &ns == default_namespace();
  auto allowed = [&](const char *path) {
    return path_accessible(ns, path) && visit(path);
  };
  for (const char *p = name; *p != '\0'; ++p)
    if (*p == '/')
      return allowed(name);

  if (requester != nullptr && origin == nullptr)
    origin = requester->name();
  if (requester != nullptr &&
      elf::visit_path_list(elf::rpath_of(*requester), name, origin, allowed))
    return true;
  if (elf::visit_path_list(ns.ld_library_path, name, nullptr, allowed))
    return true;
  if (is_default && elf::visit_path_list(library_path, name, nullptr, allowed))
    return true;
  if (requester != nullptr &&
      elf::visit_path_list(elf::runpath_of(*requester), name, origin, allowed))
    return true;
  if (is_default && system_paths != nullptr &&
      elf::visit_path_list(system_paths->list(), name, nullptr, allowed))
    return true;
  if (elf::visit_path_list(ns.default_library_path, name, nullptr, allowed))
    return true;
  if (is_default)
    for (const char *dir : elf::DEFAULT_SEARCH_PATHS) {
      char path[elf::MAX_SEARCH_PATH];
      if (elf::join_path(dir, name, path, sizeof(path)) && allowed(path))
        return true;
    }
  return false;
}

// Loads the object from the first place in the namespace it is found.
cpp::optional<elf::LoadedModule>
find_and_load_in(const Namespace &ns, const char *name,
                 const elf::Module *requester, const char *origin,
                 const char *library_path, elf::SystemPaths *system_paths,
                 size_t page_size) {
  elf::LoadedModule found;
  const bool ok = visit_candidates_in(ns, name, requester, origin, library_path,
                                      system_paths, [&](const char *path) {
                                        auto loaded =
                                            elf::load_module(path, page_size);
                                        if (!loaded.has_value())
                                          return false;
                                        found = loaded.value();
                                        return true;
                                      });
  if (!ok)
    return cpp::nullopt;
  return found;
}

// The index of a module which `ns` counts as loaded and which is `name`: by
// the path it was opened under, the file that path comes to, or the name it
// says it goes by. Failing that, a module which is the file `name` would be
// loaded from, found by device and inode so that a link to a library that is
// already open, or a second name for it, is the library and not another copy
// of it. An object reached by two paths has to be one object, as it holds the
// program's state of whatever it defines.
bool find_loaded(const elf::ModuleSet &set, const Namespace &ns,
                 const char *name, const elf::Module *requester,
                 const char *origin, const char *library_path,
                 elf::SystemPaths *system_paths, size_t &found) {
  for (size_t i = 0; i < set.count; ++i) {
    if (!namespace_sees_loaded(&ns, owner_of(set, i)))
      continue;
    if (same_name(set.modules[i].name(), name) ||
        same_file(set.modules[i].name(), name) ||
        same_name(set.modules[i].soname(), name)) {
      found = i;
      return true;
    }
  }

  dev_t device = 0;
  ino_t inode = 0;
  if (!visit_candidates_in(ns, name, requester, origin, library_path,
                           system_paths, [&](const char *path) {
                             struct stat file;
                             if (!linux_syscalls::newfstatat(AT_FDCWD, path,
                                                             &file, 0) ||
                                 (file.st_mode & S_IFMT) != S_IFREG)
                               return false;
                             device = file.st_dev;
                             inode = file.st_ino;
                             return true;
                           }))
    return false;
  for (size_t i = 0; i < set.count; ++i) {
    const char *known = set.modules[i].name();
    struct stat existing;
    if (!namespace_sees_loaded(&ns, owner_of(set, i)) || known == nullptr ||
        known[0] != '/' ||
        !linux_syscalls::newfstatat(AT_FDCWD, known, &existing, 0))
      continue;
    if (existing.st_dev == device && existing.st_ino == inode) {
      found = i;
      return true;
    }
  }
  return false;
}

// Makes room for one more module, moving the set onto the heap once the fixed
// block the startup linker filled in is full. The arrays are reached through
// pointers precisely so that they can be replaced here; nothing that holds
// the load lock may hold a pointer into them across this call.
//
// The arrays that were replaced are not freed. __tls_get_addr and the
// creation of a thread read them without a lock, and one of those may be
// part way through when this runs. They are left as they were, which is
// still right for what such a reader asks, and cost no more than the arrays
// that replaced them, since each is half the size of the next.
bool make_room(elf::ModuleSet &set) {
  if (set.count < set.capacity)
    return true;

  const size_t capacity = set.capacity * 2;
  auto *modules =
      static_cast<elf::Module *>(malloc(capacity * sizeof(elf::Module)));
  auto *mappings = static_cast<elf::MappedModule *>(
      malloc(capacity * sizeof(elf::MappedModule)));
  auto *tls_offsets =
      static_cast<intptr_t *>(malloc(capacity * sizeof(intptr_t)));
  auto *references = static_cast<size_t *>(malloc(capacity * sizeof(size_t)));
  if (modules == nullptr || mappings == nullptr || tls_offsets == nullptr ||
      references == nullptr) {
    free(modules);
    free(mappings);
    free(tls_offsets);
    free(references);
    return false;
  }

  for (size_t i = 0; i < set.count; ++i) {
    modules[i] = set.modules[i];
    mappings[i] = set.mappings[i];
    tls_offsets[i] = set.tls_offsets[i];
    references[i] = set.references[i];
  }

  __atomic_store_n(&set.modules, modules, __ATOMIC_RELEASE);
  __atomic_store_n(&set.mappings, mappings, __ATOMIC_RELEASE);
  __atomic_store_n(&set.tls_offsets, tls_offsets, __ATOMIC_RELEASE);
  set.references = references;
  set.capacity = capacity;
  return true;
}

// Holds the write lock for as long as the module set is being changed, and no
// longer: the initialisers of what was opened run after it is let go.
class WriteGuard {
public:
  WriteGuard() { dl::dl_write_lock.lock(); }
  ~WriteGuard() { release(); }
  void release() {
    if (held_)
      dl::dl_write_lock.unlock();
    held_ = false;
  }

private:
  bool held_ = true;
};

void run_init_array(const elf::Module &module) {
  elf::run_init_function(module);
  auto array = module.dynamic().address(DT_INIT_ARRAY);
  auto size = module.dynamic().value(DT_INIT_ARRAYSZ);
  if (!array || !size)
    return;
  auto **functions = reinterpret_cast<void (**)()>(*array);
  for (size_t i = 0; i < *size / sizeof(void *); ++i)
    if (functions[i] != nullptr)
      functions[i]();
}

enum class Resolved { Missing, Existing, Loaded, NoMemory };

// Puts a module that was just mapped at the end of the set, in `ns`.
void append_module(elf::ModuleSet &set, const elf::LoadedModule &loaded,
                   Namespace *ns) {
  const size_t at = set.count;
  set.modules[at] = loaded.module;
  set.mappings[at] = loaded.mapping;
  set.mappings[at].ns = ns;
  // Nothing loaded now has a place in the block each thread already has, so
  // there is no offset from a thread pointer to record. Each thread takes a
  // block of its own for it, the first time it asks.
  set.tls_offsets[at] = 0;
  set.references[at] = 1;
  elf::set_module_count(set, at + 1);
}

// Finds the library `name` for something in `ns`, loading it if it is not
// there. It is looked for in the namespace first, loaded or not, and only
// then in the namespaces that this one is linked to for that name.
Resolved resolve(elf::ModuleSet &set, Namespace *ns, const char *name,
                 const elf::Module *requester, const char *origin,
                 const char *library_path, elf::SystemPaths *system_paths,
                 size_t &index) {
  if (find_loaded(set, *ns, name, requester, origin, library_path, system_paths,
                  index))
    return Resolved::Existing;
  if (!make_room(set))
    return Resolved::NoMemory;
  auto loaded = find_and_load_in(*ns, name, requester, origin, library_path,
                                 system_paths, set.page_size);
  if (loaded.has_value()) {
    index = set.count;
    append_module(set, loaded.value(), ns);
    return Resolved::Loaded;
  }

  bool slash = false;
  for (const char *p = name; *p != '\0'; ++p)
    slash = slash || *p == '/';
  if (slash)
    return Resolved::Missing;
  for (const Namespace *from = ns; from != nullptr; from = from->shared_from) {
    for (const NamespaceLink *link = from->links; link != nullptr;
         link = link->next) {
      if (!link_allows(*link, name))
        continue;
      Namespace *target = link->target;
      if (find_loaded(set, *target, name, nullptr, nullptr, library_path,
                      system_paths, index))
        return Resolved::Existing;
      if (!make_room(set))
        return Resolved::NoMemory;
      auto other = find_and_load_in(*target, name, nullptr, nullptr,
                                    library_path, system_paths, set.page_size);
      if (other.has_value()) {
        index = set.count;
        append_module(set, other.value(), target);
        return Resolved::Loaded;
      }
    }
  }
  return Resolved::Missing;
}

} // anonymous namespace

namespace internal {

void *dlopen_in(const char *path, int mode, ElfW(Addr) caller, Namespace *ns) {
  // A statically linked program has no module set at all: the record is
  // weak and nothing defines it, so the pointer is null rather than a set
  // that says it is not linked.
  elf::ModuleSet *modules = elf::process_modules();
  cpp::lock_guard lock(dl::dl_mutex);

  // Without a loader there is no module set to add to, and nothing the caller
  // opened could be linked against the program.
  if (modules == nullptr || !modules->linked) {
    dl::set_error("dlopen is only available in a dynamically linked process");
    return nullptr;
  }
  elf::ModuleSet &set = *modules;
  constexpr int GLOBAL = 0x100;
  // A null path asks for a handle to the main program.
  if (path == nullptr)
    return dl::handle_for(0);

  const elf::Module *caller_module = nullptr;
  size_t caller_index = 0;
  for (size_t i = 0; i < set.count; ++i) {
    if (set.modules[i].contains(caller)) {
      caller_module = &set.modules[i];
      caller_index = i;
      break;
    }
  }

  // Unless it was asked for, the namespace is the caller's, or the anonymous
  // one where the caller is not a module this loaded.
  if (ns == nullptr)
    ns = namespace_of_address(caller, true);
  ns = canonical(ns);
  // The run path that applies is the one belonging to whatever called, which
  // only means something in the namespace that is its own.
  if (caller_module != nullptr && owner_of(set, caller_index) != ns)
    caller_module = nullptr;

  const char *library_path =
      elf::library_path_from(reinterpret_cast<char **>(environ));
  elf::SystemPaths system_paths;

  // The executable, which is always the first module, takes its run path
  // against the file it really is, as it did when the process started.
  char executable_path[elf::MAX_EXECUTABLE_PATH];
  const char *caller_origin = nullptr;
  if (caller_module != nullptr && caller_index == 0)
    caller_origin = elf::executable_origin(
        caller_module->name(), executable_path, sizeof(executable_path));

  size_t existing = 0;
  if (find_loaded(set, *ns, path, caller_module, caller_origin, library_path,
                  &system_paths, existing)) {
    if ((mode & GLOBAL) != 0 && set.mappings[existing].local)
      elf::make_global(set, existing);
    ++set.references[existing];
    return dl::handle_for(existing);
  }

  // RTLD_NOLOAD asks only for an object that is in the process already.
  constexpr int NOLOAD = 0x4;
  if ((mode & NOLOAD) != 0)
    return nullptr;

  WriteGuard write;
  if (!make_room(set)) {
    dl::set_error("cannot make room for another shared object");
    return nullptr;
  }

  link::announce_link_maps(r_debug::RT_ADD);

  const size_t first = set.count;
  size_t handle_index = 0;
  const Resolved top = resolve(set, ns, path, caller_module, caller_origin,
                               library_path, &system_paths, handle_index);
  if (top == Resolved::Existing) {
    // Found in a namespace this one is linked to, which is where it stays.
    if ((mode & GLOBAL) != 0 && set.mappings[handle_index].local)
      elf::make_global(set, handle_index);
    ++set.references[handle_index];
    link::announce_link_maps(r_debug::RT_CONSISTENT);
    return dl::handle_for(handle_index);
  }
  if (top != Resolved::Loaded) {
    link::announce_link_maps(r_debug::RT_CONSISTENT);
    dl::set_error(path, ns->isolated
                            ? "cannot open shared object file in this namespace"
                            : "cannot open shared object file");
    return nullptr;
  }

  // What the object was linked against has to be there too, and so does what
  // that was linked against, each found as the namespace of what needs it
  // finds it. Anything already loaded is left where it is and simply
  // referred to again.
  bool failed = false;
  const char *missing = nullptr;
  for (size_t i = first; i < set.count && !failed; ++i) {
    const elf::Module requester = set.modules[i];
    Namespace *home = owner_of(set, i);
    requester.for_each_needed([&](const char *name) {
      if (failed || elf::is_glibc_interpreter(name))
        return;
      size_t found = 0;
      const Resolved result = resolve(set, home, name, &requester, nullptr,
                                      library_path, &system_paths, found);
      if (result == Resolved::Missing || result == Resolved::NoMemory) {
        failed = true;
        missing = name;
      }
    });
  }

  // Without RTLD_GLOBAL what was opened, and what it brought in, stays out of
  // the scope that lookups through RTLD_DEFAULT and later objects use.
  for (size_t i = first; i < set.count; ++i)
    set.mappings[i].local = (mode & GLOBAL) == 0;

  // Bind against everything now loaded, the new objects included, so they can
  // refer to themselves and to each other as well as to what was there.
  // Under RTLD_LAZY a call nothing loaded defines yet is bound when first
  // made, by which time something opened since may define it.
  constexpr int BINDING_MASK = 3;
  constexpr int LAZY = 1;
  const bool lazy = (mode & BINDING_MASK) == LAZY;
  elf::BindResult result;
  if (!failed) {
    elf::SearchOrder order(set.modules, set.count);
    order.set_tls_offsets(set.tls_offsets);
    order.set_static_tls(set.static_count, dl::dynamic_tls_descriptor());
    bool *hidden = static_cast<bool *>(__builtin_alloca(set.count));
    order.set_hidden(hidden);
    for (size_t i = first; i < set.count; ++i) {
      elf::hide_from(set, i, hidden);
      const bool defer = lazy && dl::can_bind_lazily(set.modules[i]);
      const elf::BindResult one =
          elf::bind_module(set.modules[i], i, order, defer);
      if (one.deferred != 0)
        dl::enable_lazy_binding(set.modules[i]);
      result.bound += one.bound;
      result.unresolved += one.unresolved;
    }
  }

  if (failed || result.unresolved != 0) {
    for (size_t i = set.count; i > first; --i)
      elf::unmap_module(set.mappings[i - 1]);
    elf::set_module_count(set, first);
    ++set.generation;
    link::announce_link_maps(r_debug::RT_CONSISTENT);
    dl::set_error(failed ? "cannot open a shared object it was linked against"
                         : "shared object has undefined symbols");
    (void)missing;
    return nullptr;
  }

  // A thread may still hold a block for a module that used to be at one of
  // these indices. Stamping them with the generation this reaches is what
  // tells it that block is not for what is there now.
  ++set.generation;
  for (size_t i = first; i < set.count; ++i)
    set.mappings[i].generation = set.generation;

  // A debugger reads the new modules' symbols here, before their
  // constructors run.
  link::publish_link_maps();
  link::announce_link_maps(r_debug::RT_CONSISTENT);
  write.release();

  // Dependencies are initialised before what needs them. More new modules than
  // the order can hold at once, which nothing opens, are run backwards, which
  // puts each after those loaded on its behalf.
  constexpr size_t MAX_ORDERED = 256;
  const size_t added = set.count - first;
  if (added <= MAX_ORDERED) {
    size_t order[MAX_ORDERED];
    bool visited[MAX_ORDERED];
    elf::init_order(set.modules, first, set.count, order, visited);
    for (size_t i = 0; i < added; ++i)
      run_init_array(set.modules[order[i]]);
  } else {
    for (size_t i = set.count; i > first; --i)
      run_init_array(set.modules[i - 1]);
  }
  return dl::handle_for(handle_index);
}

void *dlopen_impl(const char *path, int mode) {
  return dlopen_in(path, mode,
                   reinterpret_cast<ElfW(Addr)>(__builtin_return_address(0)),
                   nullptr);
}

} // namespace internal

LLVM_LIBC_FUNCTION(void *, dlopen, (const char *path, int mode)) {
  return internal::dlopen_in(
      path, mode, reinterpret_cast<ElfW(Addr)>(__builtin_return_address(0)),
      nullptr);
}

} // namespace LIBC_NAMESPACE_DECL
