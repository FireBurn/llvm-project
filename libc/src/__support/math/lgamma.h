//===-- Natural logarithm of the gamma function -----------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC___SUPPORT_MATH_LGAMMA_H
#define LLVM_LIBC_SRC___SUPPORT_MATH_LGAMMA_H

#include "src/__support/FPUtil/FEnvImpl.h"
#include "src/__support/FPUtil/FPBits.h"
#include "src/__support/FPUtil/dyadic_float.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/optimization.h"
#include "src/__support/math/log2l_impl.h"

namespace LIBC_NAMESPACE_DECL {
namespace math {

// lgamma is computed throughout in 128 bit dyadic floats and rounded once at
// the end, which leaves some sixty bits beyond what a double shows even where
// the terms that make it up cancel. The constants were generated with MPFR
// at 700 bits and rounded to 128.
//
// For x of 16 and over, Stirling's series. Below that the recurrence
// lgamma(x) = lgamma(x + n) - log(x (x + 1) ... (x + n - 1)) moves x to 16,
// the product being exact to 128 bits, except near 1 and 2 where lgamma
// vanishes and a Taylor series about the zero is used so that the answer
// keeps its relative accuracy. Negative x goes through the reflection
// formula, save near the zeros of lgamma between -20 and -2, where it is a
// Taylor expansion about each zero.
namespace lgamma_internal {

using DFloat128 = fputil::DyadicFloat<128>;

LIBC_INLINE_VAR constexpr DFloat128 LN2 = {
    Sign::POS, -128, 0xb17217f7'd1cf79ab'c9e3b398'03f2f6af_u128};
LIBC_INLINE_VAR constexpr DFloat128 LN_PI = {
    Sign::POS, -127, 0x92868247'3d0de85e'afcab635'421fa4cc_u128};
LIBC_INLINE_VAR constexpr DFloat128 LN_SQRT_2PI = {
    Sign::POS, -128, 0xeb3f8e43'25f5a534'94bc9001'44192024_u128};

// B_2k / (2k (2k - 1)) for k from 1, the coefficients of 1/x^(2k-1) in
// Stirling's series. At x = 16 the thirtieth term is below 2^-130.
LIBC_INLINE_VAR constexpr DFloat128 STIRLING[30] = {
    {Sign::POS, -131, 0xaaaaaaaa'aaaaaaaa'aaaaaaaa'aaaaaaab_u128},
    {Sign::NEG, -136, 0xb60b60b6'0b60b60b'60b60b60'b60b60b6_u128},
    {Sign::POS, -138, 0xd00d00d0'0d00d00d'00d00d00'd00d00d0_u128},
    {Sign::NEG, -138, 0x9c09c09c'09c09c09'c09c09c0'9c09c09c_u128},
    {Sign::POS, -138, 0xdca8f158'c7f91ab8'7539c037'2a3c5632_u128},
    {Sign::NEG, -137, 0xfb5586cc'c9e3e40f'b5586ccc'9e3e40fb_u128},
    {Sign::POS, -135, 0xd20d20d2'0d20d20d'20d20d20'd20d20d2_u128},
    {Sign::NEG, -133, 0xf2143658'7a9cbee1'03254769'8badcff2_u128},
    {Sign::POS, -130, 0xb7f4b1c0'f033ffd0'c3b7f4b1'c0f03400_u128},
    {Sign::NEG, -127, 0xb23b3808'c0f9cf6d'edce7312'cc3ea607_u128},
    {Sign::POS, -124, 0xd6722191'67002d3a'7a9c8864'59c00b4f_u128},
    {Sign::NEG, -120, 0x9cd9292e'6660d55b'3f712eb9'e07ca39e_u128},
    {Sign::POS, -116, 0x8911a740'da740da7'40da740d'a740da74_u128},
    {Sign::NEG, -112, 0x8d0cc570'e255bf59'ff6eec24'b48ff1b4_u128},
    {Sign::POS, -108, 0xa8d1044d'3708d1c2'19ee4fdc'4469ccaf_u128},
    {Sign::NEG, -104, 0xe8844d8a'169abbc4'06169abb'c406169b_u128},
    {Sign::POS, -99, 0xb694d07b'219dbcc4'8676f312'19dbcc48_u128},
    {Sign::NEG, -94, 0xa2288cec'f23376ae'a6024d5c'49761635_u128},
    {Sign::POS, -89, 0xa1bbcde4'ea012735'0b881273'50b88127_u128},
    {Sign::NEG, -84, 0xb4005bde'03d4642a'24358171'4af642a2_u128},
    {Sign::POS, -79, 0xde466b7c'78fbaae3'c3a9e6da'eae46d99_u128},
    {Sign::NEG, -73, 0x977d7628'77729bcb'40509f4f'd884644b_u128},
    {Sign::POS, -68, 0xe2e1337f'5af0bed9'0b6b0a35'2d4f335d_u128},
    {Sign::NEG, -62, 0xb9e09405'8ad89016'b4f92ff9'86cdeea2_u128},
    {Sign::POS, -56, 0xa5f7eef9'e71ac7c8'0326ab4c'c8bf3f7c_u128},
    {Sign::NEG, -50, 0xa0ef80e5'7954084c'da64925c'6c86491a_u128},
    {Sign::POS, -44, 0xa8ebfe48'da17dd99'9790760b'0ce0256f_u128},
    {Sign::NEG, -38, 0xbf582a43'3556fb17'24c95ab5'6cbec2ef_u128},
    {Sign::POS, -32, 0xe940b372'3e6c7d0e'7770e671'04316dcb_u128},
    {Sign::NEG, -25, 0x989a1506'89672663'f8cc3b4f'451835e1_u128},
};

// lgamma(1 + t) = sum of c_k t^(k+1), with c_0 = -euler_gamma and
// c_k = (-1)^(k+1) zeta(k+1) / (k+1) after it.
LIBC_INLINE_VAR constexpr DFloat128 SERIES_AT_1[30] = {
    {Sign::NEG, -128, 0x93c467e3'7db0c7a4'd1be3f81'0152cb57_u128},
    {Sign::POS, -128, 0xd28d3312'983e9918'73d89122'00bace5f_u128},
    {Sign::NEG, -129, 0xcd26aadf'5596b7ac'f64b92e6'99b5aa12_u128},
    {Sign::POS, -129, 0x8a899156'3ec241b5'f9121119'6e5235fc_u128},
    {Sign::NEG, -130, 0xd45ce0bd'530a492e'826a4fda'e19904dc_u128},
    {Sign::POS, -130, 0xada06588'061830a5'3cca078a'b4ad8efb_u128},
    {Sign::NEG, -130, 0x9381d0ee'751d72c6'0ffa0f29'58701a44_u128},
    {Sign::POS, -130, 0x80859b57'c31cb745'f2ce526e'dad3266e_u128},
    {Sign::NEG, -131, 0xe4033836'abeda2ad'5500dfba'7db22232_u128},
    {Sign::POS, -131, 0xcd00f1c2'eafc7981'3418f376'8cd66b0f_u128},
    {Sign::NEG, -131, 0xba461988'a636b07a'95d89020'1c7f1588_u128},
    {Sign::POS, -131, 0xaab56b19'21de202b'bd0b01f5'd03a1ccd_u128},
    {Sign::NEG, -131, 0x9d8ecb8f'e2cc261a'79f81b6c'2036aff2_u128},
    {Sign::POS, -131, 0x924b6fc1'9062fb9e'9695c081'7d0a6441_u128},
    {Sign::NEG, -131, 0x88899a3b'73ff01dc'9a4ff396'ee34ec23_u128},
    {Sign::POS, -131, 0x80008032'66f59178'79d0156a'ffdbc10b_u128},
    {Sign::NEG, -132, 0xf0f16988'f455e4e5'abdde62d'5294e08c_u128},
    {Sign::POS, -132, 0xe38e71d1'05a0c482'd3d05f94'9f431375_u128},
    {Sign::NEG, -132, 0xd79450da'b44502e8'e83d10bf'df68da3c_u128},
    {Sign::POS, -132, 0xccccd99a'96add052'546d31af'95b3951c_u128},
    {Sign::NEG, -132, 0xc30c36db'bdfdfadb'dbde3eb0'eef83e63_u128},
    {Sign::POS, -132, 0xba2e8e8b'bc6fc881'5c7f0544'c73b8b66_u128},
    {Sign::NEG, -132, 0xb216442c'8db365df'1755c6a0'7401d739_u128},
    {Sign::POS, -132, 0xaaaaab55'57ee6aa9'40bf9c09'dec5d623_u128},
    {Sign::NEG, -132, 0xa3d70a8f'5cfdbba0'71a9a8ca'784bf5f2_u128},
    {Sign::POS, -132, 0x9d89d8c4'ec92f3e9'5ab513f0'c18e1dd3_u128},
    {Sign::NEG, -132, 0x97b42600'0015e20a'b4774bcd'423ba824_u128},
    {Sign::POS, -132, 0x9249249b'6dbde3e4'c63869e5'97816d33_u128},
    {Sign::NEG, -132, 0x8d3dcb0d'3dcd4c3d'd25a1dc8'c88bd742_u128},
    {Sign::POS, -132, 0x8888888a'aaab655b'063b7a50'6e1e3949_u128},
};

// lgamma(2 + t) = sum of c_k t^(k+1), with c_0 = 1 - euler_gamma and
// c_k = (-1)^(k+1) (zeta(k+1) - 1) / (k+1) after it.
LIBC_INLINE_VAR constexpr DFloat128 SERIES_AT_2[30] = {
    {Sign::POS, -129, 0xd8773039'049e70b6'5c8380fd'fd5a6953_u128},
    {Sign::POS, -129, 0xa51a6625'307d3230'e7b12244'01759cbd_u128},
    {Sign::NEG, -131, 0x89f000d2'abb03409'2e83a0ef'bc2bfd9e_u128},
    {Sign::POS, -133, 0xa8991563'ec241b5f'91211196'e5235fbc_u128},
    {Sign::NEG, -135, 0xf2027e10'c7af8c36'b3b061c2'998701e8_u128},
    {Sign::POS, -136, 0xbd6eb756'db617ea4'87d73802'80b91426_u128},
    {Sign::NEG, -137, 0x9c562e15'fc703e75'b3e30263'137ad8da_u128},
    {Sign::POS, -138, 0x859b57c3'1cb745f2'ce526eda'd3266dc3_u128},
    {Sign::NEG, -140, 0xe9fea63b'697e3e38'3aa30334'47d29cb6_u128},
    {Sign::POS, -141, 0xd093d878'beb2d19d'309aa700'2679085c_u128},
    {Sign::NEG, -142, 0xbc6f2deb'e40f7797'7eaf8c86'e1666c26_u128},
    {Sign::POS, -143, 0xac06e773'37581126'0574b258'f72222c0_u128},
    {Sign::NEG, -144, 0x9e5e4b1e'7112142b'52327055'9aea87d8_u128},
    {Sign::POS, -145, 0x92cbd1cf'9a555c80'ddd73ab0'4febc740_u128},
    {Sign::NEG, -146, 0x88d975bb'3caa08e3'b58732d6'31cd43f1_u128},
    {Sign::POS, -147, 0x803266f5'917879d0'156affdb'c10b5834_u128},
    {Sign::NEG, -149, 0xf13006c9'e7e975d9'ea78c347'df3575c5_u128},
    {Sign::POS, -150, 0xe3b5dd9f'83d26bb3'456eeed3'6a479b3b_u128},
    {Sign::NEG, -151, 0xd7ad365d'fc54bb2b'e44fd2db'07c41af8_u128},
    {Sign::POS, -152, 0xccdc9e10'38587a06'4e2c8e6c'84f4f594_u128},
    {Sign::NEG, -153, 0xc31639a6'f9f56366'50057d81'b40bd395_u128},
    {Sign::POS, -154, 0xba34ed66'7d6e6592'c58ee628'aaefa88c_u128},
    {Sign::NEG, -155, 0xb21a5422'3d75681a'f72edf87'6fccc234_u128},
    {Sign::POS, -156, 0xaaad43bf'fe9614f1'5f341b2b'789db0f1_u128},
    {Sign::NEG, -157, 0xa3d8b3c9'2c687209'e6dc1d0a'9c7a1e78_u128},
    {Sign::POS, -158, 0x9d8ae959'7e085e28'60901114'd74d2998_u128},
    {Sign::NEG, -159, 0x97b4d4fd'5f1efcbd'3aa99167'0edadcee_u128},
    {Sign::POS, -160, 0x92499519'ba1a620c'1053848a'11d1b0ac_u128},
    {Sign::NEG, -161, 0x8d3e1376'1291e29e'9d7b6940'969463c3_u128},
    {Sign::POS, -162, 0x8888b734'9f6cbc71'f9656c30'072b8e20_u128},
};

// (-1)^j pi^(2j+1) / (2j+1)!, so that sin(pi r) is r times a polynomial in
// r^2. For |r| <= 1/2 the last term is below 2^-170.
LIBC_INLINE_VAR constexpr DFloat128 SINPI[24] = {
    {Sign::POS, -126, 0xc90fdaa2'2168c234'c4c6628b'80dc1cd1_u128},
    {Sign::NEG, -125, 0xa55de731'2df295f5'5dc72f71'2af24826_u128},
    {Sign::POS, -126, 0xa335e33b'ad570e92'3f34224f'03d18616_u128},
    {Sign::NEG, -128, 0x99696673'15ec2df3'2f70bfb2'32e0b12b_u128},
    {Sign::POS, -131, 0xa83c1a43'f73c0dc8'3d6322ef'56c7a534_u128},
    {Sign::NEG, -135, 0xf183a7ef'4438fb11'de407614'66b06704_u128},
    {Sign::POS, -139, 0xf47a1a68'0c6b1994'682b2571'2632ab96_u128},
    {Sign::NEG, -143, 0xb7d6dcf8'aaba1c8a'8d075e96'65f35590_u128},
    {Sign::POS, -148, 0xd5761957'c99ac94f'e55050e5'769db93d_u128},
    {Sign::NEG, -153, 0xc5202108'fcaa382d'a84980c4'04903ad0_u128},
    {Sign::POS, -158, 0x943b8106'a9677c6c'499c4cc8'cb93fc42_u128},
    {Sign::NEG, -164, 0xb90afc3c'f0d644ba'674c07a3'8309c485_u128},
    {Sign::POS, -170, 0xc2ce2ca5'd22b9946'44658887'4ebfc223_u128},
    {Sign::NEG, -176, 0xaf48d562'4946d592'2f9dca06'2d383208_u128},
    {Sign::POS, -182, 0x885a9217'12b65fba'70a446fb'80537f4b_u128},
    {Sign::NEG, -189, 0xb938fc93'8d698855'22fe5e13'5dead130_u128},
    {Sign::POS, -196, 0xdd95af51'6573fbb3'87423f78'bd8172ff_u128},
    {Sign::NEG, -203, 0xeb3c3e20'96862e3b'3b29f907'ea2a6714_u128},
    {Sign::POS, -210, 0xdf1ab648'b182739e'40eabb4a'1d99ea1f_u128},
    {Sign::NEG, -217, 0xbe2e9b35'6f717ba3'4aa79335'f85bcc44_u128},
    {Sign::POS, -224, 0x927fceeb'c9817162'7e36775e'62beb652_u128},
    {Sign::NEG, -232, 0xccf4575d'89c6d8cb'9884dee6'66a79a48_u128},
    {Sign::POS, -239, 0x82c4a3ca'04a9dea5'41be6e69'16b4de62_u128},
    {Sign::NEG, -247, 0x98d26f6d'150e1400'759ef70b'85ebb403_u128},
};

// A zero of lgamma, x_hi + x_lo, and the Taylor coefficients of lgamma
// about it, c_k = psi^(k-1)(x0) / k! for k from 1 to 8. The expansion is
// used within radius of the zero, where it is good to far more than 2^-128
// relative; beyond that the reflection formula loses at most 34 bits to
// cancellation.
struct Zero {
  double x_hi;
  DFloat128 x_lo;
  double radius;
  DFloat128 coeffs[8];
};

// Two zeros in each interval (-n-1, -n) for n from 2 to 19, the more
// negative first. From -20 down lgamma stays below zero.
LIBC_INLINE_VAR constexpr Zero ZEROS[36] = {
    // n=2 left
    {-0x1.5fb410a1bd901p+1,
     {Sign::POS, -181, 0xd0cd4b69'737c2a28'169fe96f'ac0d3c38_u128},
     0x1.0b74244e42c42p-29,
     {{Sign::NEG, -127, 0xf5096d48'258c6244'0261f336'59d5fc91_u128},
      {Sign::POS, -124, 0x9933f9e1'32d28dc7'39e01cc4'7959d678_u128},
      {Sign::NEG, -123, 0xa0c2d618'645f8e0e'9ed9c94a'09dc36fa_u128},
      {Sign::POS, -122, 0xfa825666'4f8cd615'33fde035'9848ad7d_u128},
      {Sign::NEG, -120, 0xc2c422c1'03f55d69'5d35d170'29ab273c_u128},
      {Sign::POS, -118, 0xa1b9fbe6'384d96db'8af7be4a'b7ffb846_u128},
      {Sign::NEG, -116, 0x8911cdee'b600975c'8374ed2f'2e6e5560_u128},
      {Sign::POS, -115, 0xedd32f13'a10e2137'6791ab2d'9d525a2e_u128}}},
    // n=2 right
    {-0x1.3a7fc9600f86cp+1,
     {Sign::NEG, -182, 0xaafb27cc'57c681c4'b0cd2013'669e72d7_u128},
     0x1.51d1b97f9e467p-29,
     {{Sign::POS, -127, 0xc1ff4b35'7a9af688'a6f65c95'95c7c5ca_u128},
      {Sign::POS, -125, 0x9b775d80'17aae4e5'69bdb6b8'7a963d57_u128},
      {Sign::POS, -127, 0xb4a5302c'53c2bee2'7374668a'98256b57_u128},
      {Sign::POS, -124, 0x8b8c6be5'04f2db06'32b61fe2'80112a94_u128},
      {Sign::POS, -125, 0xb99cff02'593b1d6f'36c9c530'b4baad05_u128},
      {Sign::POS, -123, 0xc6997b41'5505e66f'69a3064c'435f8fc9_u128},
      {Sign::POS, -123, 0xc04f8277'3707cd3e'72958f6c'5abfb144_u128},
      {Sign::POS, -121, 0xa475540b'2b9b0947'8fc81181'4230be15_u128}}},
    // n=3 left
    {-0x1.fa471547c2fe5p+1,
     {Sign::NEG, -183, 0xb86a2b09'4891b661'9052443e'8e5a6389_u128},
     0x1.8b452f295e525p-33,
     {{Sign::NEG, -123, 0xa5ccecb3'62b234c6'8bb75ea0'0192137d_u128},
      {Sign::POS, -120, 0xfbb6f570'21b5ed4a'0ccbca15'1d144db6_u128},
      {Sign::NEG, -116, 0xe929acea'5979beef'3fd44817'63121bd1_u128},
      {Sign::POS, -112, 0xf47c14f8'a0d528a5'9ffb8860'05c42ca3_u128},
      {Sign::NEG, -107, 0x88b7bc03'693698d1'7881a70b'070f4fe4_u128},
      {Sign::POS, -103, 0x9f479d5c'fe0fa3c7'1fd872d7'92d45ffc_u128},
      {Sign::NEG, -99, 0xbeddf031'7fecf244'42494358'185a1d64_u128},
      {Sign::POS, -95, 0xe97bb6f3'de8137a7'd10c8267'a591f8a2_u128}}},
    // n=3 right
    {-0x1.9260dbc9e59afp+1,
     {Sign::NEG, -180, 0xfb8be699'ad3d9ba6'5454cb7f'ac60e3f1_u128},
     0x1.072ce1e59e2a3p-31,
     {{Sign::POS, -125, 0xf90532f9'7d62a6e2'da71b4f4'17bf2007_u128},
      {Sign::POS, -123, 0xcea694bb'8a877b40'8112af11'8316176e_u128},
      {Sign::POS, -121, 0xe089b892'6ae2d9d6'c922cbb9'e531a784_u128},
      {Sign::POS, -118, 0x933901eb'bb586cab'3efdd330'a460cc8a_u128},
      {Sign::POS, -116, 0xccd319be'd1cee949'b005fbb0'2a928f0a_u128},
      {Sign::POS, -113, 0x949e1fbc'69ded87b'6f150814'87c83a53_u128},
      {Sign::POS, -111, 0xddcbd505'b8f227c0'28f24647'a26f5bfc_u128},
      {Sign::POS, -108, 0xa8f519a2'fa9a4ac7'73ea9cfb'04ceeff6_u128}}},
    // n=4 left
    {-0x1.3f7577a6eeafdp+2,
     {Sign::POS, -180, 0xaef2f55b'f89677af'e28287ed'ad790873_u128},
     0x1.192f1c3042b93p-35,
     {{Sign::NEG, -121, 0xe91251f7'cf20fb37'c4e54277'5785b9e6_u128},
      {Sign::POS, -115, 0xda99e33c'51caaec8'2595d3db'da407605_u128},
      {Sign::NEG, -108, 0x869fbff7'32e99c32'e8120331'00662280_u128},
      {Sign::POS, -102, 0xba9537ad'61392fba'4387f0c1'daf253bb_u128},
      {Sign::NEG, -95, 0x89eae8b1'de9fbb02'6fb076ce'291d2f13_u128},
      {Sign::POS, -89, 0xd462e29a'2c6529fd'9924a26f'e809b549_u128},
      {Sign::NEG, -82, 0xa83459f6'b4a04938'6641cd50'438ff359_u128},
      {Sign::POS, -75, 0x87fd2baf'53ff491d'569be93e'54ffe25f_u128}}},
    // n=4 right
    {-0x1.0284e78599581p+2,
     {Sign::POS, -180, 0xf3c60f4f'21e7eed5'3e840294'16e12420_u128},
     0x1.31c7be9a7d496p-33,
     {{Sign::POS, -123, 0xd652e7a4'90b211a4'6a2e0d8f'e1032500_u128},
      {Sign::POS, -119, 0xa220ae6c'09fc706b'f7099c9c'84c5f311_u128},
      {Sign::POS, -115, 0xaacd88d9'54e3e117'b8ada88b'73490609_u128},
      {Sign::POS, -111, 0xcb68c710'd75ed78f'434aca3b'6a084d69_u128},
      {Sign::POS, -106, 0x8130f5ab'99720272'21ebdfa4'eea8b4c5_u128},
      {Sign::POS, -102, 0xaaf1edfc'cf59e43a'51a58a0a'2135d000_u128},
      {Sign::POS, -98, 0xe8a7f24e'2721b459'0ba1a86f'1d569c84_u128},
      {Sign::POS, -93, 0xa19ee714'16d36ed4'5162ff8e'7f864434_u128}}},
    // n=5 left
    {-0x1.7fe92f591f40dp+2,
     {Sign::NEG, -179, 0xbeea76b1'65e98f6f'c71fc72e'f5d16b3f_u128},
     0x1.6dff58c81578cp-38,
     {{Sign::NEG, -118, 0xb30fb521'd2f09218'86f70778'1ccd754d_u128},
      {Sign::POS, -110, 0xfbcee5bc'a7937794'95ff9ca7'811e1164_u128},
      {Sign::NEG, -101, 0xeb740445'0cfff12c'0870846a'4e531e50_u128},
      {Sign::POS, -92, 0xf7ae9846'dfe4b861'f370802b'a5cf4d5e_u128},
      {Sign::NEG, -82, 0x8af53585'5a94effe'b55b0fab'77fd981a_u128},
      {Sign::POS, -73, 0xa26aa74f'f11cb8f8'4c7edad3'3dcc10ec_u128},
      {Sign::NEG, -64, 0xc3427206'75d9e917'a6f1638d'e6f42671_u128},
      {Sign::POS, -55, 0xefa260ec'0e391a3f'0b62d380'ce91222c_u128}}},
    // n=5 right
    {-0x1.4086a57f0b6d9p+2,
     {Sign::NEG, -182, 0xca9315b9'654e537b'31abc10c'353f95d2_u128},
     0x1.099fd55aacf46p-35,
     {{Sign::POS, -121, 0xf6b97041'4d700f01'1f3d328a'dc6afd1b_u128},
      {Sign::POS, -115, 0xe7661976'117cd9b6'ecc778e4'470ea633_u128},
      {Sign::POS, -108, 0x929ec2b1'fb931d46'5b185315'75b1e9d7_u128},
      {Sign::POS, -102, 0xd112ef96'd3731402'c63c0e4a'5be5f8f3_u128},
      {Sign::POS, -95, 0x9f00bb9b'b133841e'2fc95633'b2920f82_u128},
      {Sign::POS, -89, 0xfbec6ade'e58c2c4e'c2813d22'9d264911_u128},
      {Sign::POS, -82, 0xcd468063'bd495ff0'5638323a'b3ae8295_u128},
      {Sign::POS, -75, 0xaabfec62'485a5a7a'2b6c05e1'a661474c_u128}}},
    // n=6 left
    {-0x1.bffcbf76b86fp+2,
     {Sign::POS, -184, 0xc29d949a'3dc02de0'bfcff5c4'57ebcf4d_u128},
     0x1.a06f4e314cf26p-41,
     {{Sign::NEG, -115, 0x9d5fbd2e'7548db04'af7157a0'a227379f_u128},
      {Sign::POS, -104, 0xc1a4d12a'82116472'1a217dd8'c246d46c_u128},
      {Sign::NEG, -92, 0x9ec8ed6e'4c213eb9'9f9fdfdf'78704617_u128},
      {Sign::POS, -80, 0x9279eb1b'7999cab2'cd5349df'33949581_u128},
      {Sign::NEG, -68, 0x90213ef8'd9a491e9'828b8f4f'359330c0_u128},
      {Sign::POS, -56, 0x93baf42b'fdb4e311'6742f057'796a971a_u128},
      {Sign::NEG, -44, 0x9bbf385a'31e09a3d'47ce764a'efa9dd2a_u128},
      {Sign::POS, -32, 0xa79e9476'dd2e689d'2dd54adc'9e6acb62_u128}}},
    // n=6 right
    {-0x1.8016b25897c8dp+2,
     {Sign::POS, -181, 0x93f07a4d'25d38f46'8f2a752d'97e61c34_u128},
     0x1.6a35882dbcc59p-38,
     {{Sign::POS, -118, 0xb4ef24f1'd79550ca'a5b484a1'd999e47b_u128},
      {Sign::POS, -110, 0xfe711a42'67e88304'cdceaf73'c6cb6473_u128},
      {Sign::POS, -101, 0xef281d1e'1be2019f'a7459b07'bb8b3ae2_u128},
      {Sign::POS, -92, 0xfce3da92'ac55db6d'12e9b885'94ad4dd3_u128},
      {Sign::POS, -82, 0x8e9ea838'a20b5156'02358d88'edd093fd_u128},
      {Sign::POS, -73, 0xa790f17d'cf03033b'963c75bf'cd4e8453_u128},
      {Sign::POS, -64, 0xca804ca6'6c54ec61'af76b0f8'1d3e792d_u128},
      {Sign::POS, -55, 0xf9d1611e'0cebcb5d'597aaf3c'fac387c3_u128}}},
    // n=7 left
    {-0x1.ffff97f8159cfp+2,
     {Sign::NEG, -182, 0xf2a7a0ad'48ac32a7'4ba20df3'47c52f4f_u128},
     0x1.a025519cb45d6p-44,
     {{Sign::NEG, -112, 0x9d7bb7f2'617d597c'c92f0b99'6a4f648e_u128},
      {Sign::POS, -98, 0xc1c73b65'57891a52'5c2bea52'6ea31f2a_u128},
      {Sign::NEG, -83, 0x9ef34599'2b292d57'52b6f860'2e1b3cbb_u128},
      {Sign::POS, -68, 0x92ae0292'9863874c'7fd88548'8bd54493_u128},
      {Sign::NEG, -53, 0x90615420'c0934f71'4fc35b59'd1de0d0a_u128},
      {Sign::POS, -38, 0x9409c9a1'67780102'5f854d38'65b96dfd_u128},
      {Sign::NEG, -23, 0x9c203361'91122e36'5718083b'74d1472c_u128},
      {Sign::POS, -8, 0xa815e26d'6a3e97d1'f971299d'ffab1463_u128}}},
    // n=7 right
    {-0x1.c0033fdedfe1fp+2,
     {Sign::POS, -179, 0x905dbe91'9233c3ea'a6ccf1ad'3a6b203e_u128},
     0x1.9fc4e67aae761p-41,
     {{Sign::POS, -115, 0x9da03d51'c3de89da'1e57343b'1eed774f_u128},
      {Sign::POS, -104, 0xc1f42ed5'7dd6ac19'158e4c47'681f4252_u128},
      {Sign::POS, -92, 0x9f2a95af'1e112df0'9d55e47b'255d08c9_u128},
      {Sign::POS, -80, 0x92f21522'f482d8b0'd327d870'999de949_u128},
      {Sign::POS, -68, 0x90b51ab0'3a1f70fa'814b02fd'd8098115_u128},
      {Sign::POS, -56, 0x9470e387'7a989931'a46ac8e1'a104c99b_u128},
      {Sign::POS, -44, 0x9c9f15e1'980406b8'7cb188dc'1dcb7a26_u128},
      {Sign::POS, -32, 0xa8b20a0f'ad7350e4'faaedcd3'889b0610_u128}}},
    // n=8 left
    {-0x1.1ffffa3884bdp+3,
     {Sign::NEG, -180, 0xffc864e9'5749259e'7e20210e'7f81cf78_u128},
     0x1.71df672a02bb3p-47,
     {{Sign::NEG, -109, 0xb12f6fe3'1ed97625'803c1296'b989ad57_u128},
      {Sign::POS, -92, 0xf5460a82'4053d268'9c997ea2'99fbebfc_u128},
      {Sign::NEG, -74, 0xe2598725'e2ae0b3d'881fe6d8'67f9acc8_u128},
      {Sign::POS, -56, 0xeaff2346'ddf814ff'1f4752c6'3175532f_u128},
      {Sign::NEG, -37, 0x821e90de'126f61b0'da8eb7a5'bd8bb855_u128},
      {Sign::POS, -19, 0x9619a572'9af0eaa6'00cb226e'c816b406_u128},
      {Sign::NEG, -1, 0xb218a5a1'8e6b2162'4e12a5e2'43dd6095_u128},
      {Sign::POS, 17, 0xd7b76ac4'd9d42eec'324fea24'6b3f8e35_u128}}},
    // n=8 right
    {-0x1.000034028b3f9p+3,
     {Sign::NEG, -179, 0xfb0659e7'60e7642b'b33be529'c6c40422_u128},
     0x1.a00eb27d765ecp-44,
     {{Sign::POS, -112, 0x9d8447f6'b3b8bd5f'53d88bac'b9da843e_u128},
      {Sign::POS, -98, 0xc1d1c49a'a876e3ea'5c764868'5102bb76_u128},
      {Sign::POS, -83, 0x9f003c6d'c56d22a0'cae7f7da'f5ddd8f9_u128},
      {Sign::POS, -68, 0x92bdf64a'32128863'13a3bd9a'fcf42a54_u128},
      {Sign::POS, -53, 0x9074f503'aad234d0'e22dc449'62576771_u128},
      {Sign::POS, -38, 0x9421f098'9e41d7eb'20b69f1f'3d0693d9_u128},
      {Sign::POS, -23, 0x9c3deb53'c2a3bdc6'9cac76c6'ee9e0942_u128},
      {Sign::POS, -8, 0xa83a73c4'6f3b8142'25981bf1'ca49a688_u128}}},
    // n=9 left
    {-0x1.3fffff6c0d7cp+3,
     {Sign::POS, -178, 0x8cbe7546'216beae0'e58b4524'330ba3c1_u128},
     0x1.27e5149a0ecd5p-50,
     {{Sign::NEG, -106, 0xdd7bed2f'9bcaea2d'4f2a3190'244daef9_u128},
      {Sign::POS, -85, 0xbf9f43c8'fd068daa'26fdc71f'bce9a846_u128},
      {Sign::NEG, -64, 0xdd0c5f7e'55531ba3'189c7071'2873f6e2_u128},
      {Sign::POS, -42, 0x8f6f0a3b'2ee06227'79241f37'7b7c9aab_u128},
      {Sign::NEG, -21, 0xc68d4d5a'd28280c8'25e9b398'09e5106e_u128},
      {Sign::POS, 1, 0x8f26c61a'e9165f62'79e83d49'355b7dcb_u128},
      {Sign::NEG, 22, 0xd450c8ed'8848008f'8ef7f292'3ef1add9_u128},
      {Sign::POS, 44, 0xa0ba7b2f'fc34032a'c67e42c1'ddf4b37b_u128}}},
    // n=9 right
    {-0x1.200005c7768fbp+3,
     {Sign::NEG, -181, 0xdadb087f'db86a3bd'6f5a13d8'bd3add16_u128},
     0x1.71dd0d836fefap-47,
     {{Sign::POS, -109, 0xb130901c'8ca200be'af413966'd408a540_u128},
      {Sign::POS, -92, 0xf547997d'bfac2d97'49c67bad'b38edd17_u128},
      {Sign::POS, -74, 0xe25baf73'4715ce7e'd2845cf5'ed653e2b_u128},
      {Sign::POS, -56, 0xeb021fd0'ffd5254b'62a59766'ac46c7f8_u128},
      {Sign::POS, -37, 0x8220a208'edbfa3ae'848381a5'3a8c7d5d_u128},
      {Sign::POS, -19, 0x961c81f6'4e4862e7'9f03d9f5'd8c88515_u128},
      {Sign::POS, -1, 0xb21c9ba2'5dcde574'77f2a00b'1dc9c56c_u128},
      {Sign::POS, 17, 0xd7bce66e'38e99a63'545a6d78'9d48a341_u128}}},
    // n=10 left
    {-0x1.5ffffff28cdd4p+3,
     {Sign::POS, -180, 0xe4c92532'd5242e72'fa5b1ba7'f9d0c06b_u128},
     0x1.ae6459f310478p-54,
     {{Sign::NEG, -102, 0x98453ec7'56dbdf2b'52cfb3c4'3446ed06_u128},
      {Sign::POS, -78, 0xb5249c03'2dfe902c'ea0bf913'c8882e8f_u128},
      {Sign::NEG, -53, 0x8fa8fb23'4c0626a2'c6b3f8b3'2884b1f8_u128},
      {Sign::POS, -28, 0x802cc9d8'bf023683'f1b70898'417732fe_u128},
      {Sign::NEG, -4, 0xf3f73ee6'78806115'af18f815'203ae3ec_u128},
      {Sign::POS, 21, 0xf1daa854'0ae2a98b'7ef5ad33'40a84154_u128},
      {Sign::NEG, 46, 0xf69c6d50'6a6162bd'ac5cfd1d'a119e4c0_u128},
      {Sign::POS, 72, 0x8059a35d'8153e576'15db3a34'8bb56ffa_u128}}},
    // n=10 right
    {-0x1.40000093f2777p+3,
     {Sign::NEG, -179, 0xc93da2ec'af0aa20f'018436dc'8a354944_u128},
     0x1.27e4e2550ad8dp-50,
     {{Sign::POS, -106, 0xdd7c12d0'631d8f6f'e6e077f4'd06cb322_u128},
      {Sign::POS, -85, 0xbf9f6457'02f97255'd901a4c5'66fa593b_u128},
      {Sign::POS, -64, 0xdd0c97d3'152e3db5'33ea5133'26ae5fe9_u128},
      {Sign::POS, -42, 0x8f6f3af7'a18d7cb2'dc6224a7'187586f5_u128},
      {Sign::POS, -21, 0xc68da1af'67107856'bfc1ba12'070943aa_u128},
      {Sign::POS, 1, 0x8f270f10'c64ce2ee'd68c3aea'49eff495_u128},
      {Sign::POS, 22, 0xd451472c'b6666cb2'84f29714'b9b4fba2_u128},
      {Sign::POS, 44, 0xa0bae869'ad9e9d7e'6d97635d'c7e02412_u128}}},
    // n=11 left
    {-0x1.7ffffffee1127p+3,
     {Sign::NEG, -181, 0xe70fbc83'5987a792'f17b0739'188516be_u128},
     0x1.1eed8f3252fd8p-57,
     {{Sign::NEG, -99, 0xe467dfd7'95863e81'6bfe7393'872bdd14_u128},
      {Sign::POS, -71, 0xcbc93101'f4c55d84'36010062'5df4a656_u128},
      {Sign::NEG, -43, 0xf26d2a75'e36d5e28'3fcc0a1c'431866a8_u128},
      {Sign::POS, -14, 0xa238b1d7'18a51eac'd7697551'eb118993_u128},
      {Sign::NEG, 14, 0xe793b4f3'14b3295c'6cbc65fa'ca058c6f_u128},
      {Sign::POS, 43, 0xac2dee1f'0d0f008b'e0596035'e77a2a45_u128},
      {Sign::NEG, 72, 0x83aca8fe'9f3731d7'ec5a528e'934ee18b_u128},
      {Sign::POS, 100, 0xcd979b45'09adcfbd'd0c73e23'3f2b3f59_u128}}},
    // n=11 right
    {-0x1.6000000d7322ap+3,
     {Sign::NEG, -178, 0xc5765969'bffa9039'2fa8e400'37306120_u128},
     0x1.ae64530b9867bp-54,
     {{Sign::POS, -102, 0x98454138'a9227ddb'98991a80'4c32ad81_u128},
      {Sign::POS, -78, 0xb5249eeb'12016fd3'15f406e9'7e1e527f_u128},
      {Sign::POS, -53, 0x8fa8fe98'3d3e6bc6'18a825b5'ce85f5cd_u128},
      {Sign::POS, -28, 0x802ccdf5'7c39898f'4a340818'f7034248_u128},
      {Sign::POS, -4, 0xf3f748af'2cfc832d'7eebffef'4d99f4c9_u128},
      {Sign::POS, 21, 0xf1dab3f7'ac4bb730'3e906f69'6f3e24c5_u128},
      {Sign::POS, 46, 0xf69c7b29'0614f647'3ea27224'a3297536_u128},
      {Sign::POS, 72, 0x8059ab99'dc9ea363'785c3a90'de30bf8d_u128}}},
    // n=12 left
    {-0x1.9fffffffe9edcp+3,
     {Sign::POS, -178, 0xc27a01a1'6800e2a0'aadcb5db'ccbabb8b_u128},
     0x1.6124613f7ad0dp-61,
     {{Sign::NEG, -95, 0xb99465fd'65a728f5'743aec89'bba4ceae_u128},
      {Sign::POS, -63, 0x8687d170'359787bf'b6be0e8b'81412d41_u128},
      {Sign::NEG, -31, 0x82082df6'229d93b2'863c6169'9266bd53_u128},
      {Sign::POS, 1, 0x8d64eea0'0f97213a'c54eb092'4f4ee373_u128},
      {Sign::NEG, 33, 0xa3ffd834'fe2d158a'be27f2e3'a2d49afd_u128},
      {Sign::POS, 65, 0xc624ecd2'a85ffa05'76d1b3af'4de5dd9d_u128},
      {Sign::NEG, 97, 0xf63cef07'2c7aaa38'e375d23f'f1d10945_u128},
      {Sign::POS, 130, 0x9c30ad4f'394d537a'65d166da'56379cd3_u128}}},
    // n=12 right
    {-0x1.800000011eed9p+3,
     {Sign::POS, -180, 0x8cea983f'0fdaf0c7'86df20a9'8a7cc7a7_u128},
     0x1.1eed8eccc8159p-57,
     {{Sign::POS, -99, 0xe467e028'6a79bd02'1f1b2563'0a7ae2ef_u128},
      {Sign::POS, -71, 0xcbc9314a'133aa27b'c9feff9d'a200b224_u128},
      {Sign::POS, -43, 0xf26d2af6'9434f8ab'f7526633'c053d7a1_u128},
      {Sign::POS, -14, 0xa238b249'ea4e9f47'f90f2378'830b5a20_u128},
      {Sign::POS, 14, 0xe793b5bf'f756bbbe'de907866'71ef16b4_u128},
      {Sign::POS, 43, 0xac2deed5'da0b250a'936d05a9'1fd80822_u128},
      {Sign::POS, 72, 0x83aca9a1'b7f42f28'12c1e698'925b4a0e_u128},
      {Sign::POS, 100, 0xcd979c68'1219c653'26fdc07c'519e8b93_u128}}},
    // n=13 left
    {-0x1.bffffffffe6c7p+3,
     {Sign::POS, -178, 0xe951879e'd707d8bb'a248edc6'e02749ba_u128},
     0x1.93974a8c7223ap-65,
     {{Sign::NEG, -91, 0xa261d93f'd535e046'5cb02c58'2cac30c9_u128},
      {Sign::POS, -56, 0xcdfff8b6'7f729a0c'f7fcc997'8694c3af_u128},
      {Sign::NEG, -20, 0xae38f598'2dddd1e1'0eb53b00'60029643_u128},
      {Sign::POS, 16, 0xa5c3f445'b5518a0f'294874a8'ef9d02c9_u128},
      {Sign::NEG, 52, 0xa83bc733'73c182d2'deb2bde7'44aa12d3_u128},
      {Sign::POS, 88, 0xb1da37ca'dacf5ae8'40b5cd55'c1f3f712_u128},
      {Sign::NEG, 124, 0xc164b102'17f6b504'a1f5125f'd4edca2c_u128},
      {Sign::POS, 160, 0xd6ac5dbf'ce04959d'17bc09a7'ec3691ad_u128}}},
    // n=13 right
    {-0x1.a000000016124p+3,
     {Sign::NEG, -178, 0xc27019a0'f746e40f'8dc0215e'857066b1_u128},
     0x1.6124613592d06p-61,
     {{Sign::POS, -95, 0xb9946602'9a58d704'e1e2d69f'6a07069a_u128},
      {Sign::POS, -63, 0x8687d173'fbb07840'4941f174'7ebed2ad_u128},
      {Sign::POS, -31, 0x82082dfb'9b4dadfe'4e4edc96'a6300d46_u128},
      {Sign::POS, 1, 0x8d64eea7'fe5d8f07'090d1f64'ce08b908_u128},
      {Sign::POS, 33, 0xa3ffd840'7e7a47f1'0bbe4945'7e753b1a_u128},
      {Sign::POS, 65, 0xc624ece3'5525105d'1d683850'73ba3d6d_u128},
      {Sign::POS, 97, 0xf63cef1f'59849212'7561d502'9133c4bb_u128},
      {Sign::POS, 130, 0x9c30ad60'bfdd27b2'9c6e01b5'f060e2c5_u128}}},
    // n=14 left
    {-0x1.dfffffffffe52p+3,
     {Sign::POS, -178, 0xfe7ce67e'c433f238'1d8b232a'0d8708fa_u128},
     0x1.ae7f3e7343424p-69,
     {{Sign::NEG, -87, 0x983bbbab'fd424cf3'54eb5969'c651f3e7_u128},
      {Sign::POS, -48, 0xb50df998'9276eb0b'9a87cfcf'e0e83b0e_u128},
      {Sign::NEG, -8, 0x8f8e0edd'49287f59'a560e878'e9e529f5_u128},
      {Sign::POS, 32, 0x800cc3b5'0bf5304b'ddef0169'8035c8ab_u128},
      {Sign::NEG, 71, 0xf3ab100c'a88eaa92'57a3421b'0d614e3e_u128},
      {Sign::POS, 111, 0xf1800a4f'22400f71'c2e69b60'dd9d5963_u128},
      {Sign::NEG, 151, 0xf630a402'884b74e7'cfe993f8'8086c8b8_u128},
      {Sign::POS, 192, 0x801988af'f5173f13'abc9585f'233ccb09_u128}}},
    // n=14 right
    {-0x1.c000000001939p+3,
     {Sign::NEG, -178, 0xe9517a53'9d7b15c6'3dd4f894'079d5e86_u128},
     0x1.93974a8b9d7p-65,
     {{Sign::POS, -91, 0xa261d940'2aca1fb9'9caf3d41'd46b33b4_u128},
      {Sign::POS, -56, 0xcdfff8b6'ec03a5f3'08033668'796b3c51_u128},
      {Sign::POS, -20, 0xae38f598'b798631f'6e077bca'3072c21c_u128},
      {Sign::POS, 16, 0xa5c3f446'640af307'52b527c6'8f2411fa_u128},
      {Sign::POS, 52, 0xa83bc734'5169bcbc'2fd8e747'7e1465e7_u128},
      {Sign::POS, 88, 0xb1da37cb'f401b5e5'748418da'2dbbd939_u128},
      {Sign::POS, 124, 0xc164b103'7cb1659a'02d62cb6'50b144a7_u128},
      {Sign::POS, 160, 0xd6ac5dc1'9291900a'd441e281'6f9b6c0c_u128}}},
    // n=15 left
    {-0x1.fffffffffffe5p+3,
     {Sign::NEG, -180, 0xc060c662'1f512e72'e4d11362'69f42652_u128},
     0x1.ae7f3e733c00cp-73,
     {{Sign::NEG, -83, 0x983bbbab'ffd324cf'354eebb6'350283ce_u128},
      {Sign::POS, -40, 0xb50df998'95841de7'42507cfc'fe0e841a_u128},
      {Sign::NEG, 4, 0x8f8e0edd'4cc9984d'4fe540e9'a40613c6_u128},
      {Sign::POS, 48, 0x800cc3b5'10462f80'f3a4fb3f'23c3fb6f_u128},
      {Sign::NEG, 91, 0xf3ab100c'b2d31003'f373dae5'2a479de1_u128},
      {Sign::POS, 135, 0xf1800a4f'2e76127d'265974c1'6a6f96cc_u128},
      {Sign::NEG, 179, 0xf630a402'96d14b52'0e36b98b'e47a1df2_u128},
      {Sign::POS, 224, 0x801988af'fdba19e0'b0c496d5'630e8b40_u128}}},
    // n=15 right
    {-0x1.e0000000001aep+3,
     {Sign::NEG, -178, 0xfe7ce66f'439083e1'7a7b8c6c'2e241d14_u128},
     0x1.ae7f3e7333c1bp-69,
     {{Sign::POS, -87, 0x983bbbac'02bdb30c'ab0d6c9a'82dd7c2f_u128},
      {Sign::POS, -48, 0xb50df998'98fc0334'65783030'1f17c4f2_u128},
      {Sign::POS, -8, 0x8f8e0edd'50e996f2'6d4ff503'7fe85414_u128},
      {Sign::POS, 32, 0x800cc3b5'152e1ab0'afb7f1e1'bfc5ff2e_u128},
      {Sign::POS, 71, 0xf3ab100c'be7e72c4'f4a66661'3dd826ca_u128},
      {Sign::POS, 111, 0xf1800a4f'3c5709dd'ed5dab4c'82bc702c_u128},
      {Sign::POS, 151, 0xf630a402'a752eb20'50ed1124'ac7a14e4_u128},
      {Sign::POS, 192, 0x801988b0'078aeabc'c3c7dab3'714b3d18_u128}}},
    // n=16 left
    {-0x1.0ffffffffffffp+4,
     {Sign::NEG, -178, 0xd5a711f9'ea4f8711'a8228b80'22a663af_u128},
     0x1.952c77030adbep-77,
     {{Sign::NEG, -79, 0xa1bf7766'bffd233d'e445dfe3'ce715818_u128},
      {Sign::POS, -32, 0xcc64c6c5'4102c186'e3f1d84c'ccdef6c6_u128},
      {Sign::NEG, 16, 0xac3054b4'513216d9'09d6ccf5'40534d84_u128},
      {Sign::POS, 64, 0xa330c476'440ce723'8ec86e2b'd2b9b453_u128},
      {Sign::NEG, 112, 0xa4f92816'99e2e276'7467f07d'47791f2a_u128},
      {Sign::POS, 160, 0xadb9786b'4320e0ab'0cb16fd5'3d0788b5_u128},
      {Sign::NEG, 208, 0xbc2ab317'd8b38a7d'1bf36e0d'c6c6d098_u128},
      {Sign::POS, 256, 0xd00e46f1'bb8e8409'0a2654a2'10427490_u128}}},
    // n=16 right
    {-0x1.000000000000dp+4,
     {Sign::NEG, -177, 0xe7f3e733'b428497f'27723e2e'f9f45b00_u128},
     0x1.ae7f3e733b032p-73,
     {{Sign::POS, -83, 0x983bbbac'002cdb30'cab10ce9'40cb3af0_u128},
      {Sign::POS, -40, 0xb50df998'95eed058'bdaf8303'01f17be6_u128},
      {Sign::POS, 4, 0x8f8e0edd'4d487dfe'c2c68898'29e64667_u128},
      {Sign::POS, 48, 0x800cc3b5'10dd1b7b'99f1587c'eaa13700_u128},
      {Sign::POS, 91, 0xf3ab100c'b43a0d53'58951643'e5337664_u128},
      {Sign::POS, 135, 0xf1800a4f'302106d2'897fe901'b775fd02_u128},
      {Sign::POS, 179, 0xf630a402'98cd14b6'11fd28e9'3f175f3f_u128},
      {Sign::POS, 224, 0x801988af'fee80fef'be56a5a4'f80bd036_u128}}},
    // n=17 left
    {-0x1.2p+4,
     {Sign::POS, -180, 0xb413c31d'cbecd2f7'4d48d9e7'837be347_u128},
     0x1.6827863b97d9dp-81,
     {{Sign::NEG, -75, 0xb5f76653'97ffd150'500b7a70'397149e1_u128},
      {Sign::POS, -23, 0x8157c5c8'd325e73c'94ac19da'b333d6ad_u128},
      {Sign::NEG, 28, 0xf52ad09a'bda1f52e'9708dc97'4051818a_u128},
      {Sign::POS, 81, 0x82b326d8'd834d2a2'508d1697'592da844_u128},
      {Sign::NEG, 133, 0x94a4c27f'c533a261'82263826'649ccf3c_u128},
      {Sign::POS, 185, 0xb0183bd4'f8cb8797'a197d0d4'53b8abb6_u128},
      {Sign::NEG, 237, 0xd69364b1'eff57b3d'19557075'1b3fac45_u128},
      {Sign::POS, 290, 0x8574e1a7'29722f51'4e2180d2'8c634a40_u128}}},
    // n=17 right
    {-0x1.1000000000001p+4,
     {Sign::POS, -178, 0xd5a711f9'ea5dde24'd697166c'4fa893d6_u128},
     0x1.952c77030acd7p-77,
     {{Sign::POS, -79, 0xa1bf7766'c002dcc2'1bba2015'1d4c01a8_u128},
      {Sign::POS, -32, 0xcc64c6c5'4109fd6f'5c0e27b3'3321093a_u128},
      {Sign::POS, 16, 0xac3054b4'513b3b05'954e59d9'4449ef17_u128},
      {Sign::POS, 64, 0xa330c476'44187450'17a4dd31'f5676a4b_u128},
      {Sign::POS, 112, 0xa4f92816'99f17b4f'e786a78d'766a30ec_u128},
      {Sign::POS, 160, 0xadb9786b'433352c2'41c9b581'3991cbe5_u128},
      {Sign::POS, 208, 0xbc2ab317'd8cad995'c03d58e5'c0d161c3_u128},
      {Sign::POS, 256, 0xd00e46f1'bbabf863'cdcf5b56'c0b5a9c1_u128}}},
    // n=18 left
    {-0x1.3p+4,
     {Sign::POS, -184, 0x97a4da34'0a0aba31'26cd1c7b'7a0822d7_u128},
     0x1.2f49b46814157p-85,
     {{Sign::NEG, -71, 0xd815c983'447ffd07'8bbd5956'2c336506_u128},
      {Sign::POS, -15, 0xb664c5e8'31c09f5e'4a83e0b1'bb4cd8ef_u128},
      {Sign::NEG, 41, 0xcd461119'0fb6dd65'68d23940'e09888a9_u128},
      {Sign::POS, 98, 0x81f37111'5d23f043'3fb1020b'd12fc2ab_u128},
      {Sign::NEG, 154, 0xaf80bddc'51d025e5'8af40d69'739581a1_u128},
      {Sign::POS, 210, 0xf6e5efd7'3d05b3af'c6217d6f'8eb04356_u128},
      {Sign::NEG, 267, 0xb2a1727e'5bdf329b'8bc07eff'a67294d4_u128},
      {Sign::POS, 324, 0x83ee7ee6'b18abf8a'413f6dbd'e9744c27_u128}}},
    // n=18 right
    {-0x1.2p+4,
     {Sign::NEG, -180, 0xb413c31d'cbeca4c3'b2ffacbb'4932f18e_u128},
     0x1.6827863b97d92p-81,
     {{Sign::POS, -75, 0xb5f76653'98002eaf'aff4858f'c02610ec_u128},
      {Sign::POS, -23, 0x8157c5c8'd326299b'3fd3e625'4ccc2953_u128},
      {Sign::POS, 28, 0xf52ad09a'bda2b1e2'e28f8920'e64f6de2_u128},
      {Sign::POS, 81, 0x82b326d8'd83558c4'2755a21c'569a1792_u128},
      {Sign::POS, 133, 0x94a4c27f'c5346110'9c64d4ce'f1dfc3bd_u128},
      {Sign::POS, 185, 0xb0183bd4'f8cc96ab'cb091011'd154308d_u128},
      {Sign::POS, 237, 0xd69364b1'eff6fc9b'8d8ca66b'26907548_u128},
      {Sign::POS, 290, 0x8574e1a7'2973413d'827823fc'552fa2a3_u128}}},
    // n=19 left
    {-0x1.4p+4,
     {Sign::POS, -189, 0xf2a15d20'10112853'031be550'f575fa90_u128},
     0x1.e542ba4020225p-90,
     {{Sign::NEG, -66, 0x870d9df2'0acfffe7'd5f78464'4afb499b_u128},
      {Sign::POS, -6, 0x8e7eba9d'66de7e5d'a482c985'4f110079_u128},
      {Sign::NEG, 54, 0xc8766cb2'79589821'719a9a95'c0ca2702_u128},
      {Sign::POS, 115, 0x9ea1ab85'b23262ed'd9185094'4026e1a9_u128},
      {Sign::NEG, 176, 0x85e5e8da'272b2247'd7c34a29'9781919f_u128},
      {Sign::POS, 236, 0xeb75e0ee'e06e0d48'56206958'394158e2_u128},
      {Sign::NEG, 297, 0xd4f1bfc9'abab1b40'c3e3dbff'895844b3_u128},
      {Sign::POS, 358, 0xc497eba4'72186ebf'f7677567'eb3817e7_u128}}},
    // n=19 right
    {-0x1.3p+4,
     {Sign::NEG, -184, 0x97a4da34'0a0ab81b'7b1f1f00'1c72bf91_u128},
     0x1.2f49b46814157p-85,
     {{Sign::POS, -71, 0xd815c983'448002f8'7442a6a9'd3c71d41_u128},
      {Sign::POS, -15, 0xb664c5e8'31c0a462'10249f4e'44b32711_u128},
      {Sign::POS, 41, 0xcd461119'0fb6e5dc'a0d4f82a'00bba144_u128},
      {Sign::POS, 98, 0x81f37111'5d23f768'8f64dd99'e53679ec_u128},
      {Sign::POS, 154, 0xaf80bddc'51d031f5'b8d28e96'332f2f52_u128},
      {Sign::POS, 210, 0xf6e5efd7'3d05c80d'212e44f7'44779374_u128},
      {Sign::POS, 267, 0xb2a1727e'5bdf43cc'0c437d1c'21053d57_u128},
      {Sign::POS, 324, 0x83ee7ee6'b18ace0c'a4306436'e0258a2b_u128}}},
};

LIBC_INLINE DFloat128 horner(const DFloat128 *c, int n, const DFloat128 &t) {
  DFloat128 p = c[n - 1];
  for (int k = n - 2; k >= 0; --k)
    p = fputil::quick_add(fputil::quick_mul(p, t), c[k]);
  return p;
}

// log(v) for v > 0. v is split into a long double hi, whose logarithm
// log2_dyadic_long gives to about 2^-120, and what is left, lo, which is
// under 2^-64 of hi, so that log(1 + lo/hi) is lo/hi to within 2^-128.
LIBC_INLINE DFloat128 log_dyadic(const DFloat128 &v) {
  long double hi = static_cast<long double>(v);
  DFloat128 r = fputil::quick_mul(log2_dyadic_long(hi), LN2);
  DFloat128 lo = fputil::quick_sub(v, DFloat128(hi));
  if (!lo.mantissa.is_zero())
    r = fputil::quick_add(r, fputil::quick_mul(lo, DFloat128(1.0L / hi)));
  return r;
}

// Stirling's series for z >= 16:
//
//   lgamma(z) = (z - 1/2) log(z) - z + log(sqrt(2 pi))
//               + sum of STIRLING[k] / z^(2k+1)
LIBC_INLINE DFloat128 stirling(const DFloat128 &z) {
  constexpr DFloat128 HALF(0.5);
  DFloat128 r = fputil::quick_mul(fputil::quick_sub(z, HALF), log_dyadic(z));
  r = fputil::quick_add(fputil::quick_sub(r, z), LN_SQRT_2PI);
  // Past 2^60 the series is below 2^-60 while the rest is above 2^65.
  if (z.exponent + 128 <= 61) {
    constexpr DFloat128 ONE(1.0);
    DFloat128 w = fputil::rounded_div(ONE, z);
    DFloat128 s = horner(STIRLING, 30, fputil::quick_mul(w, w));
    r = fputil::quick_add(r, fputil::quick_mul(s, w));
  }
  return r;
}

// lgamma(x) for x > 0, x held exactly and xd a double near it.
LIBC_INLINE DFloat128 lgamma_positive(const DFloat128 &x, double xd) {
  if (xd >= 16.0)
    return stirling(x);
  if (__builtin_fabs(xd - 1.0) <= 0x1.0p-4) {
    DFloat128 t = fputil::quick_sub(x, DFloat128(1.0));
    return fputil::quick_mul(horner(SERIES_AT_1, 30, t), t);
  }
  if (__builtin_fabs(xd - 2.0) <= 0x1.0p-3) {
    DFloat128 t = fputil::quick_sub(x, DFloat128(2.0));
    return fputil::quick_mul(horner(SERIES_AT_2, 30, t), t);
  }
  // Away from 1 and 2, |lgamma(x)| is above 1/32, so taking one sum of
  // about 30 from another costs at most ten bits.
  int n = 16 - static_cast<int>(xd);
  DFloat128 z = x;
  DFloat128 product = x;
  for (int k = 1; k < n; ++k) {
    z = fputil::quick_add(z, DFloat128(1.0));
    product = fputil::quick_mul(product, z);
  }
  z = fputil::quick_add(z, DFloat128(1.0));
  return fputil::quick_sub(stirling(z), log_dyadic(product));
}

// sin(pi r) for |r| <= 1/2.
LIBC_INLINE DFloat128 sinpi(double r) {
  DFloat128 rd(r);
  return fputil::quick_mul(horner(SINPI, 24, fputil::quick_mul(rd, rd)), rd);
}

} // namespace lgamma_internal

// The sign of gamma at x, which lgamma reports separately since it returns
// the logarithm of the magnitude, goes in *signp.
LIBC_INLINE double lgamma_r(double x, int *signp) {
  using namespace lgamma_internal;
  using FPBits = fputil::FPBits<double>;
  FPBits xbits(x);

  *signp = 1;

  if (LIBC_UNLIKELY(xbits.is_nan())) {
    if (xbits.is_signaling_nan()) {
      fputil::raise_except_if_required(FE_INVALID);
      return FPBits::quiet_nan().get_val();
    }
    return x;
  }
  if (LIBC_UNLIKELY(xbits.is_inf()))
    return FPBits::inf().get_val();

  if (LIBC_UNLIKELY(xbits.is_zero())) {
    *signp = xbits.is_neg() ? -1 : 1;
    fputil::set_errno_if_required(ERANGE);
    fputil::raise_except_if_required(FE_DIVBYZERO);
    return FPBits::inf().get_val();
  }

  // The two places lgamma is exactly zero.
  if (x == 1.0 || x == 2.0)
    return 0.0;

  DFloat128 result;
  if (!xbits.is_neg()) {
    result = lgamma_positive(DFloat128(x), x);
  } else {
    double fl = __builtin_floor(x);
    if (LIBC_UNLIKELY(fl == x)) {
      fputil::set_errno_if_required(ERANGE);
      fputil::raise_except_if_required(FE_DIVBYZERO);
      return FPBits::inf().get_val();
    }
    // gamma changes sign at each negative integer, and is negative on
    // (-1, 0), where the floor is -1.
    bool floor_is_even = __builtin_fmod(fl, 2.0) == 0.0;
    *signp = floor_is_even ? 1 : -1;

    const Zero *zero = nullptr;
    if (x < -2.0 && x > -20.0) {
      int n = static_cast<int>(-fl) - 1;
      for (const Zero *z = &ZEROS[2 * (n - 2)]; z < &ZEROS[2 * (n - 1)]; ++z)
        if (__builtin_fabs((x - z->x_hi) - static_cast<double>(z->x_lo)) <=
            z->radius)
          zero = z;
    }
    if (zero != nullptr) {
      // x - x_hi is exact, the two being within a factor of two.
      DFloat128 d = fputil::quick_sub(DFloat128(x - zero->x_hi), zero->x_lo);
      result = fputil::quick_mul(horner(zero->coeffs, 8, d), d);
    } else {
      // lgamma(x) = log(pi) - log|sin(pi x)| - lgamma(1 - x).
      double r = x - __builtin_round(x);
      DFloat128 s = sinpi(r);
      s.sign = Sign::POS;
      DFloat128 one_minus_x = fputil::quick_sub(DFloat128(1.0), DFloat128(x));
      result = fputil::quick_sub(fputil::quick_sub(LN_PI, log_dyadic(s)),
                                 lgamma_positive(one_minus_x, 1.0 - x));
    }
  }

  // The largest finite result needs an exponent of 1023.
  if (LIBC_UNLIKELY(result.exponent + 127 >= 1024)) {
    fputil::set_errno_if_required(ERANGE);
    fputil::raise_except_if_required(FE_OVERFLOW | FE_INEXACT);
  }
  return static_cast<double>(result);
}

LIBC_INLINE double lgamma(double x) {
  int sign;
  return lgamma_r(x, &sign);
}

} // namespace math
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC___SUPPORT_MATH_LGAMMA_H
