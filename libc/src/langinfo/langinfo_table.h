//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// The langinfo items of the C locale, which is the only locale there is.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_LANGINFO_LANGINFO_TABLE_H
#define LLVM_LIBC_SRC_LANGINFO_LANGINFO_TABLE_H

#include "hdr/langinfo_macros.h"
#include "hdr/types/nl_item.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace langinfo {

// The item value is a category in the top half and an index in the bottom,
// so a lookup is two steps: pick the table, then index it.
LIBC_INLINE constexpr int category_of(nl_item item) {
  return static_cast<int>(item) >> 16;
}
LIBC_INLINE constexpr int index_of(nl_item item) {
  return static_cast<int>(item) & 0xFFFF;
}

// The C locale, which POSIX spells out in full.
LIBC_INLINE_VAR constexpr const char *CTYPE_ITEMS[] = {
    "ANSI_X3.4-1968", // CODESET, the name of the character set.
};

LIBC_INLINE_VAR constexpr const char *NUMERIC_ITEMS[] = {
    ".", // RADIXCHAR
    "",  // THOUSEP, which the C locale does not have.
};

LIBC_INLINE_VAR constexpr const char *TIME_ITEMS[] = {
    "%a %b %e %H:%M:%S %Y", // D_T_FMT
    "%m/%d/%y",             // D_FMT
    "%H:%M:%S",             // T_FMT
    "%I:%M:%S %p",          // T_FMT_AMPM
    "AM",                   // AM_STR
    "PM",                   // PM_STR
    "Sunday",               // DAY_1
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday",
    "Sun", // ABDAY_1
    "Mon",
    "Tue",
    "Wed",
    "Thu",
    "Fri",
    "Sat",
    "January", // MON_1
    "February",
    "March",
    "April",
    "May",
    "June",
    "July",
    "August",
    "September",
    "October",
    "November",
    "December",
    "Jan", // ABMON_1
    "Feb",
    "Mar",
    "Apr",
    "May",
    "Jun",
    "Jul",
    "Aug",
    "Sep",
    "Oct",
    "Nov",
    "Dec",
    "", // ERA, which the C locale does not have.
    "", // ERA_D_FMT
    "", // ERA_D_T_FMT
    "", // ERA_T_FMT
    "", // ALT_DIGITS
};

LIBC_INLINE_VAR constexpr const char *MONETARY_ITEMS[] = {
    "", // CRNCYSTR, which the C locale does not have.
};

LIBC_INLINE_VAR constexpr const char *MESSAGES_ITEMS[] = {
    "^[yY]", // YESEXPR
    "^[nN]", // NOEXPR
};

} // namespace langinfo
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_LANGINFO_LANGINFO_TABLE_H
