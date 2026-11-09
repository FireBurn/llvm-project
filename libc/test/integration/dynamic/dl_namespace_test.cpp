//===-- Tests for linker namespaces in a dynamically linked program -------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// libnstest.so is built in two directories, answering 1 and 2, and
/// libnsuser.so, which needs it by name, in a third.
///
//===----------------------------------------------------------------------===//

#include "check.h"

#include <android/dlext.h>
#include <dlfcn.h>

using Fn = int (*)();

namespace {

void *open_in(const char *name, struct android_namespace_t *ns,
              int flags = RTLD_NOW) {
  android_dlextinfo info = {};
  info.flags = ANDROID_DLEXT_USE_NAMESPACE;
  info.library_namespace = ns;
  return android_dlopen_ext(name, flags, &info);
}

int call(void *handle, const char *symbol) {
  Fn fn = reinterpret_cast<Fn>(dlsym(handle, symbol));
  CHECK(fn != nullptr);
  return fn ? fn() : -1;
}

} // namespace

extern "C" int main() {
  // Nothing in the default namespace looks where the test objects are.
  CHECK(dlopen("libnstest.so", RTLD_NOW) == nullptr);
  CHECK(dlerror() != nullptr);

  android_namespace_t *one = android_create_namespace(
      "one", LLVM_LIBC_TEST_NS_ONE, nullptr, ANDROID_NAMESPACE_TYPE_ISOLATED,
      nullptr, nullptr);
  android_namespace_t *two = android_create_namespace(
      "two", LLVM_LIBC_TEST_NS_TWO, nullptr, ANDROID_NAMESPACE_TYPE_ISOLATED,
      nullptr, nullptr);
  CHECK(one != nullptr);
  CHECK(two != nullptr);

  // The same soname loads a different file in each, and neither takes the
  // other's copy for it.
  void *in_one = open_in("libnstest.so", one);
  void *in_two = open_in("libnstest.so", two);
  CHECK(in_one != nullptr);
  CHECK(in_two != nullptr);
  CHECK(in_one != in_two);
  if (in_one && in_two) {
    CHECK(call(in_one, "llvm_libc_ns_which") == 1);
    CHECK(call(in_two, "llvm_libc_ns_which") == 2);
  }
  // Opening it again in the one it is in gives that copy again.
  CHECK(open_in("libnstest.so", one) == in_one);

  // What is loaded in a namespace is not in the scope of the default one.
  CHECK(dlsym(RTLD_DEFAULT, "llvm_libc_ns_which") == nullptr);
  CHECK(dlerror() != nullptr);

  // An isolated namespace takes a file by path only from where it may.
  CHECK(open_in(LLVM_LIBC_TEST_NS_TWO "/libnstest.so", one) == nullptr);
  CHECK(dlerror() != nullptr);
  android_namespace_t *wide = android_create_namespace(
      "wide", LLVM_LIBC_TEST_NS_ONE, nullptr, ANDROID_NAMESPACE_TYPE_ISOLATED,
      LLVM_LIBC_TEST_NS_TWO, nullptr);
  CHECK(wide != nullptr);
  void *by_path = open_in(LLVM_LIBC_TEST_NS_TWO "/libnstest.so", wide);
  CHECK(by_path != nullptr);
  if (by_path)
    CHECK(call(by_path, "llvm_libc_ns_which") == 2);
  // A namespace that is not isolated takes it from anywhere.
  android_namespace_t *loose = android_create_namespace(
      "open", nullptr, nullptr, ANDROID_NAMESPACE_TYPE_REGULAR, nullptr,
      nullptr);
  CHECK(loose != nullptr);
  CHECK(open_in(LLVM_LIBC_TEST_NS_TWO "/libnstest.so", loose) != nullptr);

  // A link lets a namespace with nowhere to look use what another has, for the
  // names listed and no others.
  android_namespace_t *linked = android_create_namespace(
      "linked", nullptr, nullptr, ANDROID_NAMESPACE_TYPE_ISOLATED, nullptr,
      nullptr);
  CHECK(linked != nullptr);
  CHECK(open_in("libnstest.so", linked) == nullptr);
  CHECK(android_link_namespaces(linked, one, "libother.so"));
  CHECK(open_in("libnstest.so", linked) == nullptr);
  CHECK(android_link_namespaces(linked, one, "libother.so:libnstest.so"));
  CHECK(open_in("libnstest.so", linked) == in_one);

  // What a library needs is found by the namespace the library is in. The
  // user is built against the copy answering 1.
  android_namespace_t *user_one = android_create_namespace(
      "user_one", LLVM_LIBC_TEST_NS_USER ":" LLVM_LIBC_TEST_NS_ONE, nullptr,
      ANDROID_NAMESPACE_TYPE_ISOLATED, nullptr, nullptr);
  android_namespace_t *user_two = android_create_namespace(
      "user_two", LLVM_LIBC_TEST_NS_USER ":" LLVM_LIBC_TEST_NS_TWO, nullptr,
      ANDROID_NAMESPACE_TYPE_ISOLATED, nullptr, nullptr);
  CHECK(user_one != nullptr);
  CHECK(user_two != nullptr);
  void *user_in_one = open_in("libnsuser.so", user_one);
  void *user_in_two = open_in("libnsuser.so", user_two);
  CHECK(user_in_one != nullptr);
  CHECK(user_in_two != nullptr);
  if (user_in_one && user_in_two) {
    CHECK(call(user_in_one, "llvm_libc_ns_user") == 101);
    CHECK(call(user_in_two, "llvm_libc_ns_user") == 102);
  }
  // A library whose need is nowhere in its namespace does not load.
  android_namespace_t *bare = android_create_namespace(
      "bare", LLVM_LIBC_TEST_NS_USER, nullptr, ANDROID_NAMESPACE_TYPE_ISOLATED,
      nullptr, nullptr);
  CHECK(bare != nullptr);
  CHECK(open_in("libnsuser.so", bare) == nullptr);
  CHECK(dlerror() != nullptr);

  // A namespace shared from the default one counts what is loaded there as
  // loaded in it.
  void *libc = dlopen("libc.so.1", RTLD_NOW);
  CHECK(libc != nullptr);
  android_namespace_t *shared = android_create_namespace(
      "shared", nullptr, nullptr,
      ANDROID_NAMESPACE_TYPE_ISOLATED | ANDROID_NAMESPACE_TYPE_SHARED, nullptr,
      android_get_exported_namespace("default"));
  CHECK(shared != nullptr);
  CHECK(open_in("libc.so.1", shared) == libc);
  CHECK(open_in("libc.so.1", one) != libc);

  // Extensions other than the namespace are still refused, and so is a
  // namespace that does not exist.
  android_dlextinfo info = {};
  info.flags = ANDROID_DLEXT_USE_NAMESPACE | ANDROID_DLEXT_FORCE_LOAD;
  info.library_namespace = one;
  CHECK(android_dlopen_ext("libnstest.so", RTLD_NOW, &info) == nullptr);
  CHECK(open_in("libnstest.so", nullptr) == nullptr);
  int not_a_namespace = 0;
  CHECK(open_in("libnstest.so", reinterpret_cast<android_namespace_t *>(
                                    &not_a_namespace)) == nullptr);

  return failures == 0 ? 0 : 1;
}
