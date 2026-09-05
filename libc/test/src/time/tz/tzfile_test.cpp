//===-- Unittests for reading a TZif file ---------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/time/tz/tzfile.h"
#include "test/UnitTest/Test.h"

using LIBC_NAMESPACE::tz::TZFile;

namespace {

// Builds a version 2 file the way RFC 8536 describes: a header and block with
// times four bytes wide, the same again with them eight, and a TZ string.
class Builder {
public:
  void reset() { size = 0; }

  const unsigned char *bytes() const { return buffer; }
  size_t length() const { return size; }

  void put(unsigned char byte) { buffer[size++] = byte; }

  void put32(uint32_t value) {
    for (int shift = 24; shift >= 0; shift -= 8)
      put(static_cast<unsigned char>(value >> shift));
  }

  void put64(uint64_t value) {
    for (int shift = 56; shift >= 0; shift -= 8)
      put(static_cast<unsigned char>(value >> shift));
  }

  void header(unsigned char version, uint32_t transitions, uint32_t types,
              uint32_t names) {
    put('T');
    put('Z');
    put('i');
    put('f');
    put(version);
    for (int i = 0; i < 15; ++i)
      put(0);
    put32(0); // no records saying which times are UT
    put32(0); // nor which are standard
    put32(0); // no leap seconds
    put32(transitions);
    put32(types);
    put32(names);
  }

  void type(int32_t offset, bool daylight, unsigned char name_index) {
    put32(static_cast<uint32_t>(offset));
    put(daylight ? 1 : 0);
    put(name_index);
  }

  void text(const char *from) {
    for (const char *at = from;; ++at) {
      put(static_cast<unsigned char>(*at));
      if (*at == '\0')
        break;
    }
  }

private:
  unsigned char buffer[1024] = {};
  size_t size = 0;
};

// One change, into daylight time and back, both copies of the table and a
// string covering the years past them.
void build_two_type_file(Builder &out, const char *footer) {
  out.reset();
  // The four byte wide copy, which nothing reads but which has to be there.
  out.header('2', 2, 2, 8);
  out.put32(static_cast<uint32_t>(-1000000000));
  out.put32(static_cast<uint32_t>(1000000000));
  out.put(0);
  out.put(1);
  out.type(-18000, false, 0);
  out.type(-14400, true, 4);
  out.text("EST");
  out.text("EDT");

  // The eight byte wide copy, which is the one that is read.
  out.header('2', 2, 2, 8);
  out.put64(static_cast<uint64_t>(static_cast<int64_t>(-1000000000)));
  out.put64(static_cast<uint64_t>(static_cast<int64_t>(1000000000)));
  out.put(0);
  out.put(1);
  out.type(-18000, false, 0);
  out.type(-14400, true, 4);
  out.text("EST");
  out.text("EDT");

  out.put('\n');
  for (const char *at = footer; *at != '\0'; ++at)
    out.put(static_cast<unsigned char>(*at));
  out.put('\n');
}

} // anonymous namespace

TEST(LlvmLibcTZFile, RejectsWhatIsNotOne) {
  TZFile file;
  const unsigned char nothing[4] = {'N', 'O', 'P', 'E'};
  ASSERT_FALSE(file.parse(nothing, sizeof(nothing)));
  ASSERT_FALSE(file.parse(nothing, 0));
}

TEST(LlvmLibcTZFile, RejectsOneCutShort) {
  Builder builder;
  build_two_type_file(builder, "EST5EDT,M3.2.0,M11.1.0");
  TZFile file;
  ASSERT_FALSE(file.parse(builder.bytes(), builder.length() / 2));
}

TEST(LlvmLibcTZFile, ReadsTheChangesItLists) {
  Builder builder;
  build_two_type_file(builder, "EST5EDT,M3.2.0,M11.1.0");
  TZFile file;
  ASSERT_TRUE(file.parse(builder.bytes(), builder.length()));

  // Before the first change, the first reading that is not daylight time.
  TZFile::Reading reading = file.read_at(-2000000000);
  ASSERT_EQ(reading.answer.offset, -18000);
  ASSERT_FALSE(reading.answer.daylight);
  ASSERT_STREQ(reading.answer.name, "EST");

  // Between the two changes.
  reading = file.read_at(0);
  ASSERT_EQ(reading.answer.offset, -18000);
  ASSERT_FALSE(reading.answer.daylight);

  reading = file.read_at(-1000000000);
  ASSERT_EQ(reading.answer.offset, -18000);
  reading = file.read_at(-1000000000 - 1);
  ASSERT_EQ(reading.answer.offset, -18000);
}

TEST(LlvmLibcTZFile, PastTheLastChangeTheStringTakesOver) {
  Builder builder;
  build_two_type_file(builder, "EST5EDT,M3.2.0,M11.1.0");
  TZFile file;
  ASSERT_TRUE(file.parse(builder.bytes(), builder.length()));

  // The first of July 2024, which the string puts on daylight time.
  TZFile::Reading reading = file.read_at(1719835200);
  ASSERT_TRUE(reading.from_footer);
  ASSERT_TRUE(reading.answer.daylight);
  ASSERT_EQ(reading.answer.offset, -14400);

  // The first of January 2024, which it does not.
  reading = file.read_at(1704110400);
  ASSERT_TRUE(reading.from_footer);
  ASSERT_FALSE(reading.answer.daylight);
  ASSERT_EQ(reading.answer.offset, -18000);
}

TEST(LlvmLibcTZFile, WithoutAStringTheLastChangeStands) {
  Builder builder;
  builder.reset();
  builder.header('\0', 2, 2, 8);
  builder.put32(static_cast<uint32_t>(-1000000000));
  builder.put32(static_cast<uint32_t>(1000000000));
  builder.put(0);
  builder.put(1);
  builder.type(-18000, false, 0);
  builder.type(-14400, true, 4);
  builder.text("EST");
  builder.text("EDT");

  TZFile file;
  ASSERT_TRUE(file.parse(builder.bytes(), builder.length()));
  TZFile::Reading reading = file.read_at(1719835200);
  ASSERT_FALSE(reading.from_footer);
  ASSERT_TRUE(reading.answer.daylight);
  ASSERT_EQ(reading.answer.offset, -14400);
}

TEST(LlvmLibcTZFile, TheNamesItGoesBy) {
  Builder builder;
  build_two_type_file(builder, "EST5EDT,M3.2.0,M11.1.0");
  TZFile file;
  ASSERT_TRUE(file.parse(builder.bytes(), builder.length()));

  // Taken by walking the changes back from the last, which leaves the pair
  // the zone was using most recently.
  TZFile::NamePair pair = file.name_pair();
  ASSERT_STREQ(pair.standard, "EST");
  ASSERT_STREQ(pair.daylight, "EDT");
  ASSERT_EQ(pair.standard_offset, -18000);
  ASSERT_TRUE(pair.has_daylight);
}

TEST(LlvmLibcTZFile, AZoneThatNeverChangesReportsOneNameTwice) {
  Builder builder;
  builder.reset();
  builder.header('\0', 0, 1, 4);
  builder.type(-18000, false, 0);
  builder.text("EST");

  TZFile file;
  ASSERT_TRUE(file.parse(builder.bytes(), builder.length()));
  TZFile::NamePair pair = file.name_pair();
  ASSERT_STREQ(pair.standard, "EST");
  ASSERT_STREQ(pair.daylight, "EST");
  ASSERT_FALSE(pair.has_daylight);
  ASSERT_EQ(file.read_at(0).answer.offset, -18000);
}

TEST(LlvmLibcTZFile, RejectsAChangePointingAtNoReading) {
  Builder builder;
  builder.reset();
  builder.header('\0', 1, 1, 4);
  builder.put32(0);
  builder.put(3); // there is no fourth reading
  builder.type(-18000, false, 0);
  builder.text("EST");

  TZFile file;
  ASSERT_FALSE(file.parse(builder.bytes(), builder.length()));
}

TEST(LlvmLibcTZFile, RejectsNamesThatDoNotEnd) {
  Builder builder;
  builder.reset();
  builder.header('\0', 0, 1, 3);
  builder.type(-18000, false, 0);
  builder.put('E');
  builder.put('S');
  builder.put('T');

  TZFile file;
  ASSERT_FALSE(file.parse(builder.bytes(), builder.length()));
}
