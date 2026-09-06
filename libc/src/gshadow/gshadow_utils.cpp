//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Helper functions for gshadow.
///
//===----------------------------------------------------------------------===//

#include "src/gshadow/gshadow_utils.h"
#include "hdr/types/struct_sgrp.h"
#include "src/__support/CPP/span.h"
#include "src/__support/field_tokenizer.h"
#include "src/__support/flat_file_db.h"
#include "src/__support/macros/attributes.h"

#ifndef LIBC_COPT_GSHADOW_FILE_PATH
#define LIBC_COPT_GSHADOW_FILE_PATH "/etc/gshadow"
#endif

namespace LIBC_NAMESPACE_DECL {
namespace gshadow {

char *admin_list[GSHADOW_NAME_MAX];
char *member_list[GSHADOW_NAME_MAX];

bool split_names(cpp::span<char> field, char **out, size_t max) {
  if (!out || max < 1)
    return false;
  size_t count = 0;
  if (!field.empty() && field.front() != '\0') {
    internal::FieldTokenizer tokenizer(field, ',');
    while (auto name = tokenizer.next_field()) {
      if (count + 1 >= max)
        return false;
      out[count++] = name->data();
    }
  }
  out[count] = nullptr;
  return true;
}

bool parse_line_into(cpp::span<char> line, struct sgrp *entry, char **admins,
                     size_t max_admins, char **members, size_t max_members) {
  if (line.empty() || !entry)
    return false;

  internal::FieldTokenizer tokenizer(line);

  auto name = tokenizer.next_field();
  if (!name || name->empty())
    return false;
  entry->sg_namp = name->data();

  // A line may stop after the name, and the fields it leaves out are empty
  // ones rather than a reason to reject it. The name's own terminator serves
  // as the empty string, so nothing has to be written into the line.
  auto passwd = tokenizer.next_field();
  entry->sg_passwd = passwd ? passwd->data() : name->data() + name->size() - 1;

  // Either list may be missing from the line, and an absent one is an
  // empty one rather than a reason to reject the entry.
  auto admin_field = tokenizer.next_field();
  if (admin_field) {
    if (!split_names(*admin_field, admins, max_admins))
      return false;
  } else {
    admins[0] = nullptr;
  }
  entry->sg_adm = admins;

  // The member list is the rest of the line, separator and all, since
  // nothing follows it to be told apart from.
  auto member_field = tokenizer.remaining();
  if (member_field) {
    if (!split_names(*member_field, members, max_members))
      return false;
  } else {
    members[0] = nullptr;
  }
  entry->sg_mem = members;

  return true;
}

} // namespace gshadow

namespace gshadow_db {

static LIBC_CONSTINIT internal::FlatFileDatabase<struct sgrp>
    db(LIBC_COPT_GSHADOW_FILE_PATH);
// Process wide and not under a lock, the same as the group and shadow
// databases: getsgent is not reentrant, and the _r calls open their own.
static char line_buffer[gshadow::GSHADOW_LINE_SIZE];
static struct sgrp gshadow_entry;

void TESTONLY_set_gshadow_path(const char *path) { db.set_path(path); }

const char *path() { return db.path(); }

ErrorOr<void> open() { return db.setdb(); }

ErrorOr<void> close() { return db.enddb(); }

ErrorOr<struct sgrp *> read_next() {
  auto res = db.getnext(&gshadow_entry, line_buffer);
  if (!res.has_value())
    return Error(res.error());
  if (!res.value())
    return nullptr;
  return &gshadow_entry;
}

} // namespace gshadow_db
} // namespace LIBC_NAMESPACE_DECL
