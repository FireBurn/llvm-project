//===-- Unittests for getopt_long -----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/unistd/getopt_long.h"
#include "src/unistd/getopt_long_only.h"
#include "test/UnitTest/Test.h"

#include "hdr/getopt_macros.h"
#include "src/__support/CPP/array.h"
#include "src/string/strcmp.h"
#include "src/unistd/getopt.h"

using LIBC_NAMESPACE::cpp::array;

namespace test_globals {
char *optarg;
int optind = 1;
int optopt;
int opterr = 0;
unsigned optpos;
} // namespace test_globals

void set_state() {
  LIBC_NAMESPACE::impl::set_getopt_state(
      &test_globals::optarg, &test_globals::optind, &test_globals::optopt,
      &test_globals::optpos, &test_globals::opterr, nullptr);
}

struct LlvmLibcGetoptLongTest : public LIBC_NAMESPACE::testing::Test {
  void SetUp() override {
    set_state();
    test_globals::optind = 1;
    test_globals::optarg = nullptr;
  }
};

char *operator""_c(const char *c, size_t) { return const_cast<char *>(c); }

namespace {
int verbose_flag = 0;
const struct option LONGOPTS[] = {
    {"verbose", no_argument, &verbose_flag, 1},
    {"file", required_argument, nullptr, 'f'},
    {"level", optional_argument, nullptr, 'l'},
    {"verify", no_argument, nullptr, 'V'},
    {nullptr, 0, nullptr, 0},
};
} // anonymous namespace

TEST_F(LlvmLibcGetoptLongTest, NoArgumentSetsFlagAndReturnsZero) {
  array<char *, 3> argv{"prog"_c, "--verbose"_c, nullptr};
  verbose_flag = 0;
  int index = -1;
  // A non-null flag means the value is stored there and 0 is returned.
  EXPECT_EQ(LIBC_NAMESPACE::getopt_long(2, argv.data(), "", LONGOPTS, &index),
            0);
  EXPECT_EQ(verbose_flag, 1);
  EXPECT_EQ(index, 0);
  EXPECT_EQ(test_globals::optind, 2);
}

TEST_F(LlvmLibcGetoptLongTest, ZeroMeansStartAgain) {
  array<char *, 4> argv{"prog"_c, "--verbose"_c, "operand"_c, nullptr};
  verbose_flag = 0;
  EXPECT_EQ(LIBC_NAMESPACE::getopt_long(3, argv.data(), "", LONGOPTS, nullptr),
            0);
  EXPECT_EQ(test_globals::optind, 2);

  // A program which parses its arguments twice puts optind back to zero
  // before the second pass, which starts at the argument after the program's
  // own name rather than at the name itself.
  verbose_flag = 0;
  test_globals::optind = 0;
  EXPECT_EQ(LIBC_NAMESPACE::getopt_long(3, argv.data(), "", LONGOPTS, nullptr),
            0);
  EXPECT_EQ(verbose_flag, 1);
  EXPECT_EQ(test_globals::optind, 2);
  EXPECT_STREQ(argv[test_globals::optind], "operand");
}

TEST_F(LlvmLibcGetoptLongTest, RequiredArgumentInline) {
  array<char *, 3> argv{"prog"_c, "--file=out.txt"_c, nullptr};
  EXPECT_EQ(LIBC_NAMESPACE::getopt_long(2, argv.data(), "", LONGOPTS, nullptr),
            int('f'));
  ASSERT_TRUE(test_globals::optarg != nullptr);
  EXPECT_EQ(LIBC_NAMESPACE::strcmp(test_globals::optarg, "out.txt"), 0);
}

TEST_F(LlvmLibcGetoptLongTest, RequiredArgumentSeparate) {
  array<char *, 4> argv{"prog"_c, "--file"_c, "out.txt"_c, nullptr};
  EXPECT_EQ(LIBC_NAMESPACE::getopt_long(3, argv.data(), "", LONGOPTS, nullptr),
            int('f'));
  ASSERT_TRUE(test_globals::optarg != nullptr);
  EXPECT_EQ(LIBC_NAMESPACE::strcmp(test_globals::optarg, "out.txt"), 0);
  EXPECT_EQ(test_globals::optind, 3);
}

TEST_F(LlvmLibcGetoptLongTest, MissingRequiredArgument) {
  array<char *, 3> argv{"prog"_c, "--file"_c, nullptr};
  EXPECT_EQ(LIBC_NAMESPACE::getopt_long(2, argv.data(), "", LONGOPTS, nullptr),
            int('?'));
  // A leading colon in optstring asks for ':' instead.
  test_globals::optind = 1;
  EXPECT_EQ(LIBC_NAMESPACE::getopt_long(2, argv.data(), ":", LONGOPTS, nullptr),
            int(':'));
}

TEST_F(LlvmLibcGetoptLongTest, OptionalArgumentOnlyTakenInline) {
  array<char *, 3> argv{"prog"_c, "--level=3"_c, nullptr};
  EXPECT_EQ(LIBC_NAMESPACE::getopt_long(2, argv.data(), "", LONGOPTS, nullptr),
            int('l'));
  ASSERT_TRUE(test_globals::optarg != nullptr);
  EXPECT_EQ(LIBC_NAMESPACE::strcmp(test_globals::optarg, "3"), 0);

  // Without "=", the next argv element must not be consumed.
  SetUp();
  array<char *, 4> argv2{"prog"_c, "--level"_c, "3"_c, nullptr};
  EXPECT_EQ(LIBC_NAMESPACE::getopt_long(3, argv2.data(), "", LONGOPTS, nullptr),
            int('l'));
  EXPECT_TRUE(test_globals::optarg == nullptr);
  EXPECT_EQ(test_globals::optind, 2);
}

TEST_F(LlvmLibcGetoptLongTest, UnambiguousAbbreviation) {
  array<char *, 3> argv{"prog"_c, "--fi=x"_c, nullptr};
  EXPECT_EQ(LIBC_NAMESPACE::getopt_long(2, argv.data(), "", LONGOPTS, nullptr),
            int('f'));
}

TEST_F(LlvmLibcGetoptLongTest, AmbiguousAbbreviationIsRejected) {
  // "--ver" matches both "verbose" and "verify".
  array<char *, 3> argv{"prog"_c, "--ver"_c, nullptr};
  EXPECT_EQ(LIBC_NAMESPACE::getopt_long(2, argv.data(), "", LONGOPTS, nullptr),
            int('?'));
}

TEST_F(LlvmLibcGetoptLongTest, ExactMatchBeatsLongerPrefix) {
  // "verify" is also a prefix of nothing else, but "verbose" starts with the
  // same letters; an exact match must win rather than being called ambiguous.
  const struct option opts[] = {
      {"ver", no_argument, nullptr, 'a'},
      {"verbose", no_argument, nullptr, 'b'},
      {nullptr, 0, nullptr, 0},
  };
  array<char *, 3> argv{"prog"_c, "--ver"_c, nullptr};
  EXPECT_EQ(LIBC_NAMESPACE::getopt_long(2, argv.data(), "", opts, nullptr),
            int('a'));
}

TEST_F(LlvmLibcGetoptLongTest, UnrecognizedOption) {
  array<char *, 3> argv{"prog"_c, "--nope"_c, nullptr};
  EXPECT_EQ(LIBC_NAMESPACE::getopt_long(2, argv.data(), "", LONGOPTS, nullptr),
            int('?'));
  EXPECT_EQ(test_globals::optind, 2);
}

TEST_F(LlvmLibcGetoptLongTest, ArgumentGivenToNoArgumentOption) {
  array<char *, 3> argv{"prog"_c, "--verify=x"_c, nullptr};
  EXPECT_EQ(LIBC_NAMESPACE::getopt_long(2, argv.data(), "", LONGOPTS, nullptr),
            int('?'));
}

// A program which hands the arguments it does not know to another one keeps
// scanning past them, so an unknown short option has to be eaten rather
// than offered again.
TEST_F(LlvmLibcGetoptLongTest, UnknownShortOptionIsEaten) {
  array<char *, 4> argv{"prog"_c, "-z"_c, "-a"_c, nullptr};

  EXPECT_EQ(LIBC_NAMESPACE::getopt_long(3, argv.data(), "a", LONGOPTS, nullptr),
            int('?'));
  EXPECT_EQ(test_globals::optopt, (int)'z');
  EXPECT_EQ(test_globals::optind, 2);
  EXPECT_EQ(LIBC_NAMESPACE::getopt_long(3, argv.data(), "a", LONGOPTS, nullptr),
            int('a'));
  EXPECT_EQ(LIBC_NAMESPACE::getopt_long(3, argv.data(), "a", LONGOPTS, nullptr),
            -1);
}

TEST_F(LlvmLibcGetoptLongTest, ShortOptionsStillWork) {
  array<char *, 3> argv{"prog"_c, "-ab"_c, nullptr};
  EXPECT_EQ(
      LIBC_NAMESPACE::getopt_long(2, argv.data(), "ab", LONGOPTS, nullptr),
      int('a'));
  EXPECT_EQ(
      LIBC_NAMESPACE::getopt_long(2, argv.data(), "ab", LONGOPTS, nullptr),
      int('b'));
}

TEST_F(LlvmLibcGetoptLongTest, DoubleDashEndsOptions) {
  array<char *, 4> argv{"prog"_c, "--"_c, "--file"_c, nullptr};
  EXPECT_EQ(LIBC_NAMESPACE::getopt_long(3, argv.data(), "", LONGOPTS, nullptr),
            -1);
  EXPECT_EQ(test_globals::optind, 2);
}

TEST_F(LlvmLibcGetoptLongTest, LongOnlyAcceptsSingleDash) {
  array<char *, 3> argv{"prog"_c, "-file=x"_c, nullptr};
  EXPECT_EQ(
      LIBC_NAMESPACE::getopt_long_only(2, argv.data(), "", LONGOPTS, nullptr),
      int('f'));
  ASSERT_TRUE(test_globals::optarg != nullptr);
  EXPECT_EQ(LIBC_NAMESPACE::strcmp(test_globals::optarg, "x"), 0);
}

TEST_F(LlvmLibcGetoptLongTest, LongOnlyFallsBackToShort) {
  // "-a" is not in the long table, so short parsing has to handle it.
  array<char *, 3> argv{"prog"_c, "-a"_c, nullptr};
  EXPECT_EQ(
      LIBC_NAMESPACE::getopt_long_only(2, argv.data(), "a", LONGOPTS, nullptr),
      int('a'));
}

TEST_F(LlvmLibcGetoptLongTest, PlainArgumentStops) {
  array<char *, 3> argv{"prog"_c, "file.txt"_c, nullptr};
  EXPECT_EQ(LIBC_NAMESPACE::getopt_long(2, argv.data(), "", LONGOPTS, nullptr),
            -1);
}
