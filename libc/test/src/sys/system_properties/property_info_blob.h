//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// A property info area, laid out by hand, for the tests of the properties
/// that are kept in a directory of areas.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TEST_SRC_SYS_SYSTEM_PROPERTIES_PROPERTY_INFO_BLOB_H
#define LLVM_LIBC_TEST_SRC_SYS_SYSTEM_PROPERTIES_PROPERTY_INFO_BLOB_H

#include <stddef.h>
#include <stdint.h>

namespace property_info_blob {

constexpr const char DEFAULT_CONTEXT[] = "u:object_r:default_prop:s0";
constexpr const char BUILD_CONTEXT[] = "u:object_r:build_prop:s0";
constexpr size_t SIZE = 256;
constexpr uint32_t NONE = ~0u;

// Properties under "ro.build." are in the build context, as is "debug" itself.
// Everything else is in the default one.
//
//   root (default context)
//     exact match "debug" -> build
//     child "ro"
//       prefix "build." -> build
inline void fill(unsigned char *out) {
  uint32_t *w = reinterpret_cast<uint32_t *>(out);
  for (size_t i = 0; i < SIZE / 4; ++i)
    w[i] = 0;
  auto word = [&](size_t offset, uint32_t value) { w[offset / 4] = value; };
  auto text = [&](size_t offset, const char *s) {
    for (size_t i = 0; s[i] != '\0'; ++i)
      out[offset + i] = static_cast<unsigned char>(s[i]);
  };
  // header
  word(0, 1);
  word(4, 1);
  word(8, SIZE);
  word(12, 24);
  word(16, 36);
  word(20, 200);
  // contexts, then no types
  word(24, 2);
  word(28, 40);
  word(32, 68);
  word(36, 0);
  text(40, DEFAULT_CONTEXT);
  text(68, BUILD_CONTEXT);
  // names
  text(96, "root");
  text(104, "ro");
  text(108, "build.");
  text(116, "debug");
  // entries: name offset, length, context, type
  const uint32_t entries[4][4] = {{96, 4, 0, NONE},
                                  {104, 2, NONE, NONE},
                                  {108, 6, 1, NONE},
                                  {116, 5, 1, NONE}};
  for (size_t i = 0; i < 4; ++i)
    for (size_t k = 0; k < 4; ++k)
      word(124 + i * 16 + k * 4, entries[i][k]);
  // the children of the root, the prefixes of "ro", the exact matches of root
  word(188, 228);
  word(192, 156);
  word(196, 172);
  // nodes: entry, children (count, offset), prefixes (count, offset), exact
  // matches (count, offset)
  const uint32_t root[7] = {124, 1, 188, 0, 0, 1, 196};
  const uint32_t ro[7] = {140, 0, 0, 1, 192, 0, 0};
  for (size_t k = 0; k < 7; ++k) {
    word(200 + k * 4, root[k]);
    word(228 + k * 4, ro[k]);
  }
}

} // namespace property_info_blob

#endif // LLVM_LIBC_TEST_SRC_SYS_SYSTEM_PROPERTIES_PROPERTY_INFO_BLOB_H
