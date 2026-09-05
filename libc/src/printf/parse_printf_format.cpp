//===-- Implementation of parse_printf_format -----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/printf/parse_printf_format.h"

#include "hdr/printf_macros.h"
#include "hdr/types/size_t.h"
#include "src/__support/common.h"
#include "src/__support/ctype_utils.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

namespace {

// How wide the conversion said its argument is.
enum class Length { NONE, hh, h, l, ll, L, j, z, t };

Length read_length(const char *&p) {
  switch (*p) {
  case 'h':
    ++p;
    if (*p == 'h') {
      ++p;
      return Length::hh;
    }
    return Length::h;
  case 'l':
    ++p;
    if (*p == 'l') {
      ++p;
      return Length::ll;
    }
    return Length::l;
  case 'L':
    ++p;
    return Length::L;
  case 'q':
    ++p;
    return Length::ll;
  case 'j':
    ++p;
    return Length::j;
  case 'z':
  case 'Z':
    ++p;
    return Length::z;
  case 't':
    ++p;
    return Length::t;
  default:
    return Length::NONE;
  }
}

// The type an integer conversion of this length takes. The flag says how
// the length was spelled rather than how wide the type turns out to be, so
// "ll" is reported as long long on a machine where long is just as wide.
// glibc reports it the same way, and callers match on the flag.
int int_type_for(Length len) {
  switch (len) {
  case Length::hh:
    return PA_CHAR;
  case Length::h:
    return PA_INT | PA_FLAG_SHORT;
  case Length::l:
  case Length::z:
  case Length::t:
    return PA_INT | PA_FLAG_LONG;
  case Length::ll:
  case Length::j:
    return PA_INT | PA_FLAG_LONG_LONG;
  default:
    return PA_INT;
  }
}

// Somewhere to put a type as it is found, which keeps the count going past
// the end of what the caller gave room for.
class Types {
  int *out;
  size_t cap;
  size_t count = 0;

public:
  Types(int *o, size_t c) : out(o), cap(c) {}

  size_t size() const { return count; }

  void append(int type) { put(count, type); }

  // For an argument named by position, which may arrive out of order and
  // may leave gaps.
  void put(size_t index, int type) {
    if (index < cap)
      out[index] = type;
    if (index + 1 > count)
      count = index + 1;
  }
};

// Reads a run of digits followed by '$'. Returns 0 when what follows is not
// one, leaving the pointer where it was.
size_t read_position(const char *&p) {
  const char *start = p;
  size_t value = 0;
  while (internal::isdigit(*p)) {
    value = value * 10 + static_cast<size_t>(*p - '0');
    ++p;
  }
  if (value != 0 && *p == '$') {
    ++p;
    return value;
  }
  p = start;
  return 0;
}

} // namespace

LLVM_LIBC_FUNCTION(size_t, parse_printf_format,
                   (const char *__restrict format, size_t n,
                    int *__restrict argtypes)) {
  if (format == nullptr)
    return 0;

  Types types(argtypes, argtypes == nullptr ? 0 : n);

  for (const char *p = format; *p != '\0'; ++p) {
    if (*p != '%')
      continue;
    ++p;
    if (*p == '%')
      continue;

    // An argument may name which one it is, and so may a width or a
    // precision given as a star.
    size_t position = read_position(p);

    for (;; ++p) {
      if (*p != '-' && *p != '+' && *p != ' ' && *p != '#' && *p != '0' &&
          *p != '\'' && *p != 'I')
        break;
    }

    if (*p == '*') {
      ++p;
      size_t star_pos = read_position(p);
      if (star_pos != 0)
        types.put(star_pos - 1, PA_INT);
      else
        types.append(PA_INT);
    } else {
      while (internal::isdigit(*p))
        ++p;
    }

    if (*p == '.') {
      ++p;
      if (*p == '*') {
        ++p;
        size_t star_pos = read_position(p);
        if (star_pos != 0)
          types.put(star_pos - 1, PA_INT);
        else
          types.append(PA_INT);
      } else {
        while (internal::isdigit(*p))
          ++p;
      }
    }

    const Length length = read_length(p);
    if (*p == '\0')
      break;

    int type;
    switch (*p) {
    case 'd':
    case 'i':
    case 'o':
    case 'u':
    case 'x':
    case 'X':
    case 'b':
    case 'B':
      type = int_type_for(length);
      break;
    case 'c':
      // An l here makes it a wide character, but that still arrives in one
      // argument slot of the same size, and glibc does not report it
      // either.
      type = PA_CHAR;
      break;
    case 'C':
      type = PA_WCHAR;
      break;
    case 's':
      type = PA_STRING;
      break;
    case 'S':
      type = PA_WSTRING;
      break;
    case 'p':
      type = PA_POINTER;
      break;
    case 'n':
      // Whatever width was asked for, what arrives is a pointer, so the
      // width flags say nothing worth having.
      type = PA_INT | PA_FLAG_PTR;
      break;
    case 'a':
    case 'A':
    case 'e':
    case 'E':
    case 'f':
    case 'F':
    case 'g':
    case 'G':
      type = PA_DOUBLE | (length == Length::L ? PA_FLAG_LONG_DOUBLE : 0);
      break;
    case 'm':
      // The message for the current errno, which takes no argument.
      continue;
    default:
      // Not a conversion this knows. There is no way to say how many
      // arguments it would take, so it is passed over.
      continue;
    }

    if (position != 0)
      types.put(position - 1, type);
    else
      types.append(type);
  }

  return types.size();
}

} // namespace LIBC_NAMESPACE_DECL
