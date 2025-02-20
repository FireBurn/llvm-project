//=- AArch64SMEInstrInfo.td -  AArch64 SME Instructions -*- tablegen -*-----=//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// AArch64 Scalable Matrix Extension (SME) Instruction definitions.
//
//===----------------------------------------------------------------------===//

def AArch64_smstart : SDNode<"AArch64ISD::SMSTART", SDTypeProfile<0, 3,
                             [SDTCisInt<0>, SDTCisInt<0>, SDTCisInt<0>]>,
                             [SDNPHasChain, SDNPSideEffect, SDNPVariadic,
                              SDNPOptInGlue, SDNPOutGlue]>;
def AArch64_smstop  : SDNode<"AArch64ISD::SMSTOP", SDTypeProfile<0, 3,
                             [SDTCisInt<0>, SDTCisInt<0>, SDTCisInt<0>]>,
                             [SDNPHasChain, SDNPSideEffect, SDNPVariadic,
                              SDNPOptInGlue, SDNPOutGlue]>;
def AArch64_restore_za : SDNode<"AArch64ISD::RESTORE_ZA", SDTypeProfile<0, 3,
                             [SDTCisInt<0>, SDTCisPtrTy<1>]>,
                             [SDNPHasChain, SDNPSideEffect, SDNPVariadic,
                              SDNPOptInGlue]>;

def AArch64ObscureCopy : SDNode<"AArch64ISD::OBSCURE_COPY", SDTypeProfile<1, 1, []>, []>;

//===----------------------------------------------------------------------===//
// Add vector elements horizontally or vertically to ZA tile.
//===----------------------------------------------------------------------===//

def SDT_AArch64RDSVL  : SDTypeProfile<1, 1, [SDTCisInt<0>, SDTCisInt<1>]>;
def AArch64rdsvl : SDNode<"AArch64ISD::RDSVL", SDT_AArch64RDSVL>;

let Predicates = [HasSME] in {
def RDSVLI_XI  : sve_int_read_vl_a<0b0, 0b11111, "rdsvl", /*streaming_sve=*/0b1>;
def ADDSPL_XXI : sve_int_arith_vl<0b1, "addspl", /*streaming_sve=*/0b1>;
def ADDSVL_XXI : sve_int_arith_vl<0b0, "addsvl", /*streaming_sve=*/0b1>;

def ADDHA_MPPZ_S : sme_add_vector_to_tile_u32<0b0, "addha">;
def ADDVA_MPPZ_S : sme_add_vector_to_tile_u32<0b1, "addva">;

def : Pat<(AArch64rdsvl (i32 simm6_32b:$imm)), (RDSVLI_XI simm6_32b:$imm)>;
}

let Predicates = [HasSMEI16I64] in {
def ADDHA_MPPZ_D : sme_add_vector_to_tile_u64<0b0, "addha">;
def ADDVA_MPPZ_D : sme_add_vector_to_tile_u64<0b1, "addva">;
}

let Predicates = [HasSME] in {
//===----------------------------------------------------------------------===//
// Outer products
//===----------------------------------------------------------------------===//

defm BFMOPA_MPPZZ  : sme_bf16_outer_product<0b0, "bfmopa", int_aarch64_sme_mopa_wide>;
defm BFMOPS_MPPZZ  : sme_bf16_outer_product<0b1, "bfmops", int_aarch64_sme_mops_wide>;

defm FMOPA_MPPZZ_S : sme_outer_product_fp32<0b0, "fmopa", int_aarch64_sme_mopa>;
defm FMOPS_MPPZZ_S : sme_outer_product_fp32<0b1, "fmops", int_aarch64_sme_mops>;
}

let Predicates = [HasSMEF64F64] in {
defm FMOPA_MPPZZ_D : sme_outer_product_fp64<0b0, "fmopa", int_aarch64_sme_mopa>;
defm FMOPS_MPPZZ_D : sme_outer_product_fp64<0b1, "fmops", int_aarch64_sme_mops>;
}

let Predicates = [HasSME] in {
defm FMOPAL_MPPZZ  : sme_f16_outer_product<0b0, "fmopa", int_aarch64_sme_mopa_wide>;
defm FMOPSL_MPPZZ  : sme_f16_outer_product<0b1, "fmops", int_aarch64_sme_mops_wide>;

defm SMOPA_MPPZZ_S  : sme_int_outer_product_i32<0b000, "smopa",  int_aarch64_sme_smopa_wide>;
defm SMOPS_MPPZZ_S  : sme_int_outer_product_i32<0b001, "smops",  int_aarch64_sme_smops_wide>;
defm UMOPA_MPPZZ_S  : sme_int_outer_product_i32<0b110, "umopa",  int_aarch64_sme_umopa_wide>;
defm UMOPS_MPPZZ_S  : sme_int_outer_product_i32<0b111, "umops",  int_aarch64_sme_umops_wide>;
defm SUMOPA_MPPZZ_S : sme_int_outer_product_i32<0b010, "sumopa", int_aarch64_sme_sumopa_wide>;
defm SUMOPS_MPPZZ_S : sme_int_outer_product_i32<0b011, "sumops", int_aarch64_sme_sumops_wide>;
defm USMOPA_MPPZZ_S : sme_int_outer_product_i32<0b100, "usmopa", int_aarch64_sme_usmopa_wide>;
defm USMOPS_MPPZZ_S : sme_int_outer_product_i32<0b101, "usmops", int_aarch64_sme_usmops_wide>;
}

let Predicates = [HasSMEI16I64] in {
defm SMOPA_MPPZZ_D  : sme_int_outer_product_i64<0b000, "smopa",  int_aarch64_sme_smopa_wide>;
defm SMOPS_MPPZZ_D  : sme_int_outer_product_i64<0b001, "smops",  int_aarch64_sme_smops_wide>;
defm UMOPA_MPPZZ_D  : sme_int_outer_product_i64<0b110, "umopa",  int_aarch64_sme_umopa_wide>;
defm UMOPS_MPPZZ_D  : sme_int_outer_product_i64<0b111, "umops",  int_aarch64_sme_umops_wide>;
defm SUMOPA_MPPZZ_D : sme_int_outer_product_i64<0b010, "sumopa", int_aarch64_sme_sumopa_wide>;
defm SUMOPS_MPPZZ_D : sme_int_outer_product_i64<0b011, "sumops", int_aarch64_sme_sumops_wide>;
defm USMOPA_MPPZZ_D : sme_int_outer_product_i64<0b100, "usmopa", int_aarch64_sme_usmopa_wide>;
defm USMOPS_MPPZZ_D : sme_int_outer_product_i64<0b101, "usmops", int_aarch64_sme_usmops_wide>;
}

let Predicates = [HasSME] in {
//===----------------------------------------------------------------------===//
// Loads and stores
//===----------------------------------------------------------------------===//

defm LD1_MXIPXX : sme_mem_ld_ss<"ld1">;
defm ST1_MXIPXX : sme_mem_st_ss<"st1">;

//===----------------------------------------------------------------------===//
// Spill + fill
//===----------------------------------------------------------------------===//

defm LDR_ZA : sme_fill<"ldr">;
defm STR_ZA : sme_spill<"str">;

//===----------------------------------------------------------------------===//
// Move instructions
//===----------------------------------------------------------------------===//

defm INSERT_MXIPZ  : sme_vector_to_tile<"mova">;
defm EXTRACT_ZPMXI : sme_tile_to_vector<"mova">;

//===----------------------------------------------------------------------===//
// Zero instruction
//===----------------------------------------------------------------------===//

defm ZERO_M : sme_zero<"zero">;

//===----------------------------------------------------------------------===//
// Mode selection and state access instructions
//===----------------------------------------------------------------------===//

// SME defines three pstate fields to set or clear PSTATE.SM, PSTATE.ZA, or
// both fields:
//
//   MSR SVCRSM, #<imm1>
//   MSR SVCRZA, #<imm1>
//   MSR SVCRSMZA, #<imm1>
//
// It's tricky to using the existing pstate operand defined in
// AArch64SystemOperands.td since it only encodes 5 bits including op1;op2,
// when these fields are also encoded in CRm[3:1].
def MSRpstatesvcrImm1
  : PstateWriteSimple<(ins svcr_op:$pstatefield, timm0_1:$imm), "msr",
                      "\t$pstatefield, $imm">,
    Sched<[WriteSys]> {
  bits<3> pstatefield;
  bit imm;
  let Inst{18-16} = 0b011; // op1
  let Inst{11-9} = pstatefield;
  let Inst{8} = imm;
  let Inst{7-5} = 0b011; // op2
}

def : InstAlias<"smstart",    (MSRpstatesvcrImm1 0b011, 0b1)>;
def : InstAlias<"smstart sm", (MSRpstatesvcrImm1 0b001, 0b1)>;
def : InstAlias<"smstart za", (MSRpstatesvcrImm1 0b010, 0b1)>;

def : InstAlias<"smstop",     (MSRpstatesvcrImm1 0b011, 0b0)>;
def : InstAlias<"smstop sm",  (MSRpstatesvcrImm1 0b001, 0b0)>;
def : InstAlias<"smstop za",  (MSRpstatesvcrImm1 0b010, 0b0)>;


// Pseudo to match to smstart/smstop. This expands:
//
//  pseudonode (pstate_za|pstate_sm), before_call, expected_value
//
// Into:
//
//   if (before_call != expected_value)
//     node (pstate_za|pstate_sm)
//
// where node can be either 'smstart' or 'smstop'.
def MSRpstatePseudo :
  Pseudo<(outs),
           (ins svcr_op:$pstatefield, timm0_1:$imm, GPR64:$rtpstate, timm0_1:$expected_pstate, variable_ops), []>,
    Sched<[WriteSys]>;

// Pseudo to conditionally restore ZA state. This expands:
//
//   pseudonode tpidr2_el0, tpidr2obj, restore_routine
//
// Into:
//
//   if (tpidr2_el0 == 0)
//     BL restore_routine, implicit-use tpidr2obj
//
def RestoreZAPseudo :
  Pseudo<(outs),
         (ins GPR64:$tpidr2_el0, GPR64sp:$tpidr2obj, i64imm:$restore_routine, variable_ops), []>,
         Sched<[]>;

def : Pat<(AArch64_restore_za
            (i64 GPR64:$tpidr2_el0), (i64 GPR64sp:$tpidr2obj), (i64 texternalsym:$restore_routine)),
          (RestoreZAPseudo GPR64:$tpidr2_el0, GPR64sp:$tpidr2obj, texternalsym:$restore_routine)>;

// Scenario A:
//
//   %pstate.before.call = 1
//   if (%pstate.before.call != 0)
//     smstop (pstate_za|pstate_sm)
//   call fn()
//   if (%pstate.before.call != 0)
//     smstart (pstate_za|pstate_sm)
//
def : Pat<(AArch64_smstop (i32 svcr_op:$pstate), (i64 1), (i64 0)),   // before call
          (MSRpstatesvcrImm1 svcr_op:$pstate, 0b0)>;
def : Pat<(AArch64_smstart (i32 svcr_op:$pstate), (i64 1), (i64 0)),  // after call
          (MSRpstatesvcrImm1 svcr_op:$pstate, 0b1)>;

// Scenario B:
//
//   %pstate.before.call = 0
//   if (%pstate.before.call != 1)
//     smstart (pstate_za|pstate_sm)
//   call fn()
//   if (%pstate.before.call != 1)
//     smstop (pstate_za|pstate_sm)
//
def : Pat<(AArch64_smstart (i32 svcr_op:$pstate), (i64 0), (i64 1)),  // before call
          (MSRpstatesvcrImm1 svcr_op:$pstate, 0b1)>;
def : Pat<(AArch64_smstop (i32 svcr_op:$pstate), (i64 0), (i64 1)),   // after call
          (MSRpstatesvcrImm1 svcr_op:$pstate, 0b0)>;

// The generic case which gets expanded to a pseudo node.
def : Pat<(AArch64_smstart (i32 svcr_op:$pstate), (i64 GPR64:$rtpstate), (i64 timm0_1:$expected_pstate)),
          (MSRpstatePseudo svcr_op:$pstate, 0b1, GPR64:$rtpstate, timm0_1:$expected_pstate)>;
def : Pat<(AArch64_smstop (i32 svcr_op:$pstate), (i64 GPR64:$rtpstate), (i64 timm0_1:$expected_pstate)),
          (MSRpstatePseudo svcr_op:$pstate, 0b0, GPR64:$rtpstate, timm0_1:$expected_pstate)>;

// Read and write TPIDR2_EL0
def : Pat<(int_aarch64_sme_set_tpidr2 i64:$val),
          (MSR 0xde85, GPR64:$val)>;
def : Pat<(i64 (int_aarch64_sme_get_tpidr2)),
          (MRS 0xde85)>;

def OBSCURE_COPY : Pseudo<(outs GPR64:$dst), (ins GPR64:$idx), []>, Sched<[]> { }
def : Pat<(i64 (AArch64ObscureCopy (i64 GPR64:$idx))),
          (OBSCURE_COPY GPR64:$idx)>;
} // End let Predicates = [HasSME]

//===----------------------------------------------------------------------===//
// SME2 Instructions
//===----------------------------------------------------------------------===//
let Predicates = [HasSME2] in {
defm ADD_VG2_M2ZZ_S  : sme2_mla_add_sub_array_vg2_single_S<"add", 0b10>;
defm ADD_VG4_M4ZZ_S  : sme2_mla_add_sub_array_vg4_single_S<"add", 0b10>;
defm ADD_VG2_M2Z2Z_S : sme2_mla_add_sub_array_vg2_multi_S<"add", 0b10>;
defm ADD_VG4_M4Z4Z_S : sme2_mla_add_sub_array_vg4_multi_S<"add", 0b10>;

defm SUB_VG2_M2ZZ_S  : sme2_mla_add_sub_array_vg2_single_S<"sub", 0b11>;
defm SUB_VG4_M4ZZ_S  : sme2_mla_add_sub_array_vg4_single_S<"sub", 0b11>;
defm SUB_VG2_M2Z2Z_S : sme2_mla_add_sub_array_vg2_multi_S<"sub", 0b11>;
defm SUB_VG4_M4Z4Z_S : sme2_mla_add_sub_array_vg4_multi_S<"sub", 0b11>;

defm FMLA_VG2_M2ZZ_S  : sme2_mla_add_sub_array_vg2_single_S<"fmla", 0b00>;
defm FMLA_VG4_M4ZZ_S  : sme2_mla_add_sub_array_vg4_single_S<"fmla", 0b00>;
defm FMLA_VG2_M2Z2Z_S : sme2_mla_add_sub_array_vg2_multi_S<"fmla", 0b00>;
defm FMLA_VG4_M4Z4Z_S : sme2_mla_add_sub_array_vg4_multi_S<"fmla", 0b00>;

defm FMLS_VG2_M2ZZ_S  : sme2_mla_add_sub_array_vg2_single_S<"fmls", 0b01>;
defm FMLS_VG4_M4ZZ_S  : sme2_mla_add_sub_array_vg4_single_S<"fmls", 0b01>;
defm FMLS_VG2_M2Z2Z_S : sme2_mla_add_sub_array_vg2_multi_S<"fmls", 0b01>;
defm FMLS_VG4_M4Z4Z_S : sme2_mla_add_sub_array_vg4_multi_S<"fmls", 0b01>;

defm ADD_VG2_2ZZ  : sme2_sqdmulh_add_vector_vg2_single<"add", 0b011000>;
defm ADD_VG4_4ZZ  : sme2_sqdmulh_add_vector_vg4_single<"add", 0b011000>;

defm ADDA_VG2_M2Z2Z_S : sme2_multivec_accum_add_sub_vg2_S<"add", 0b10>;
defm ADDA_VG4_M4Z4Z_S : sme2_multivec_accum_add_sub_vg4_S<"add", 0b10>;

defm SUBA_VG2_M2Z2Z_S : sme2_multivec_accum_add_sub_vg2_S<"sub", 0b11>;
defm SUBA_VG4_M4Z4Z_S : sme2_multivec_accum_add_sub_vg4_S<"sub", 0b11>;

defm FADD_VG2_M2Z2Z_S : sme2_multivec_accum_add_sub_vg2_S<"fadd", 0b00>;
defm FADD_VG4_M4Z4Z_S : sme2_multivec_accum_add_sub_vg4_S<"fadd", 0b00>;

defm FSUB_VG2_M2Z2Z_S : sme2_multivec_accum_add_sub_vg2_S<"fsub", 0b01>;
defm FSUB_VG4_M4Z4Z_S : sme2_multivec_accum_add_sub_vg4_S<"fsub", 0b01>;

defm SQDMULH_2ZZ : sme2_sqdmulh_add_vector_vg2_single<"sqdmulh", 0b100000>;
defm SQDMULH_4ZZ : sme2_sqdmulh_add_vector_vg4_single<"sqdmulh", 0b100000>;
defm SQDMULH_2Z2Z2Z : sme2_sqdmulh_vector_vg2_multi<"sqdmulh">;
defm SQDMULH_4Z4Z4Z : sme2_sqdmulh_vector_vg4_multi<"sqdmulh">;

defm FMLAL_MZZI      : sme2_mla_long_array_index<"fmlal",  0b10,   0b00>;
defm FMLAL_VG2_M2ZZI : sme2_fp_mla_long_array_vg2_index<"fmlal",   0b00>;
defm FMLAL_VG4_M4ZZI : sme2_fp_mla_long_array_vg4_index<"fmlal",   0b00>;
defm FMLAL_MZZ       : sme2_mla_long_array_single<"fmlal", 0b00,   0b00>;
defm FMLAL_VG2_M2ZZ  : sme2_fp_mla_long_array_vg2_single<"fmlal",  0b00>;
defm FMLAL_VG4_M4ZZ  : sme2_fp_mla_long_array_vg4_single<"fmlal",  0b00>;
defm FMLAL_VG2_M2Z2Z : sme2_fp_mla_long_array_vg2_multi<"fmlal",   0b00>;
defm FMLAL_VG4_M4Z4Z : sme2_fp_mla_long_array_vg4_multi<"fmlal",   0b00>;

defm FMLSL_MZZI      : sme2_mla_long_array_index<"fmlsl",  0b10,   0b01>;
defm FMLSL_VG2_M2ZZI : sme2_fp_mla_long_array_vg2_index<"fmlsl",   0b01>;
defm FMLSL_VG4_M4ZZI : sme2_fp_mla_long_array_vg4_index<"fmlsl",   0b01>;
defm FMLSL_MZZ       : sme2_mla_long_array_single<"fmlsl", 0b00,   0b01>;
defm FMLSL_VG2_M2ZZ  : sme2_fp_mla_long_array_vg2_single<"fmlsl",  0b01>;
defm FMLSL_VG4_M4ZZ  : sme2_fp_mla_long_array_vg4_single<"fmlsl",  0b01>;
defm FMLSL_VG2_M2Z2Z : sme2_fp_mla_long_array_vg2_multi<"fmlsl",   0b01>;
defm FMLSL_VG4_M4Z4Z : sme2_fp_mla_long_array_vg4_multi<"fmlsl",   0b01>;

defm BFMLAL_MZZI      : sme2_mla_long_array_index<"bfmlal",  0b10,   0b10>;
defm BFMLAL_VG2_M2ZZI : sme2_fp_mla_long_array_vg2_index<"bfmlal",   0b10>;
defm BFMLAL_VG4_M4ZZI : sme2_fp_mla_long_array_vg4_index<"bfmlal",   0b10>;
defm BFMLAL_MZZ       : sme2_mla_long_array_single<"bfmlal", 0b00,   0b10>;
defm BFMLAL_VG2_M2ZZ  : sme2_fp_mla_long_array_vg2_single<"bfmlal",  0b10>;
defm BFMLAL_VG4_M4ZZ  : sme2_fp_mla_long_array_vg4_single<"bfmlal",  0b10>;
defm BFMLAL_VG2_M2Z2Z : sme2_fp_mla_long_array_vg2_multi<"bfmlal",   0b10>;
defm BFMLAL_VG4_M4Z4Z : sme2_fp_mla_long_array_vg4_multi<"bfmlal",   0b10>;

defm BFMLSL_MZZI      : sme2_mla_long_array_index<"bfmlsl",  0b10,   0b11>;
defm BFMLSL_VG2_M2ZZI : sme2_fp_mla_long_array_vg2_index<"bfmlsl",   0b11>;
defm BFMLSL_VG4_M4ZZI : sme2_fp_mla_long_array_vg4_index<"bfmlsl",   0b11>;
defm BFMLSL_MZZ       : sme2_mla_long_array_single<"bfmlsl", 0b00,   0b11>;
defm BFMLSL_VG2_M2ZZ  : sme2_fp_mla_long_array_vg2_single<"bfmlsl",  0b11>;
defm BFMLSL_VG4_M4ZZ  : sme2_fp_mla_long_array_vg4_single<"bfmlsl",  0b11>;
defm BFMLSL_VG2_M2Z2Z : sme2_fp_mla_long_array_vg2_multi<"bfmlsl",   0b11>;
defm BFMLSL_VG4_M4Z4Z : sme2_fp_mla_long_array_vg4_multi<"bfmlsl",   0b11>;

defm SMLAL_MZZI      : sme2_mla_long_array_index<"smlal", 0b11,    0b00>;
defm SMLAL_VG2_M2ZZI : sme2_int_mla_long_array_vg2_index<"smlal",  0b00>;
defm SMLAL_VG4_M4ZZI : sme2_int_mla_long_array_vg4_index<"smlal",  0b00>;
defm SMLAL_MZZ       : sme2_mla_long_array_single<"smlal",0b01,    0b00>;
defm SMLAL_VG2_M2ZZ  : sme2_int_mla_long_array_vg2_single<"smlal", 0b00>;
defm SMLAL_VG4_M4ZZ  : sme2_int_mla_long_array_vg4_single<"smlal", 0b00>;
defm SMLAL_VG2_M2Z2Z : sme2_int_mla_long_array_vg2_multi<"smlal",  0b00>;
defm SMLAL_VG4_M4Z4Z : sme2_int_mla_long_array_vg4_multi<"smlal",  0b00>;

defm SMLSL_MZZI      : sme2_mla_long_array_index<"smlsl", 0b11,    0b01>;
defm SMLSL_VG2_M2ZZI : sme2_int_mla_long_array_vg2_index<"smlsl",  0b01>;
defm SMLSL_VG4_M4ZZI : sme2_int_mla_long_array_vg4_index<"smlsl",  0b01>;
defm SMLSL_MZZ       : sme2_mla_long_array_single<"smlsl",0b01,    0b01>;
defm SMLSL_VG2_M2ZZ  : sme2_int_mla_long_array_vg2_single<"smlsl", 0b01>;
defm SMLSL_VG4_M4ZZ  : sme2_int_mla_long_array_vg4_single<"smlsl", 0b01>;
defm SMLSL_VG2_M2Z2Z : sme2_int_mla_long_array_vg2_multi<"smlsl",  0b01>;
defm SMLSL_VG4_M4Z4Z : sme2_int_mla_long_array_vg4_multi<"smlsl",  0b01>;

defm UMLAL_MZZI      : sme2_mla_long_array_index<"umlal", 0b11,    0b10>;
defm UMLAL_VG2_M2ZZI : sme2_int_mla_long_array_vg2_index<"umlal",  0b10>;
defm UMLAL_VG4_M4ZZI : sme2_int_mla_long_array_vg4_index<"umlal",  0b10>;
defm UMLAL_MZZ       : sme2_mla_long_array_single<"umlal",0b01,    0b10>;
defm UMLAL_VG2_M2ZZ  : sme2_int_mla_long_array_vg2_single<"umlal", 0b10>;
defm UMLAL_VG4_M4ZZ  : sme2_int_mla_long_array_vg4_single<"umlal", 0b10>;
defm UMLAL_VG2_M2Z2Z : sme2_int_mla_long_array_vg2_multi<"umlal",  0b10>;
defm UMLAL_VG4_M4Z4Z : sme2_int_mla_long_array_vg4_multi<"umlal",  0b10>;

defm UMLSL_MZZI      : sme2_mla_long_array_index<"umlsl", 0b11,    0b11>;
defm UMLSL_VG2_M2ZZI : sme2_int_mla_long_array_vg2_index<"umlsl",  0b11>;
defm UMLSL_VG4_M4ZZI : sme2_int_mla_long_array_vg4_index<"umlsl",  0b11>;
defm UMLSL_MZZ       : sme2_mla_long_array_single<"umlsl",0b01,    0b11>;
defm UMLSL_VG2_M2ZZ  : sme2_int_mla_long_array_vg2_single<"umlsl", 0b11>;
defm UMLSL_VG4_M4ZZ  : sme2_int_mla_long_array_vg4_single<"umlsl", 0b11>;
defm UMLSL_VG2_M2Z2Z : sme2_int_mla_long_array_vg2_multi<"umlsl",  0b11>;
defm UMLSL_VG4_M4Z4Z : sme2_int_mla_long_array_vg4_multi<"umlsl",  0b11>;

def FCVT_Z2Z_StoH   : sme2_cvt_vg2_single<"fcvt",   0b00, 0b00>;
def FCVTN_Z2Z_StoH  : sme2_cvt_vg2_single<"fcvtn",  0b01, 0b00>;
def BFCVT_Z2Z_StoH  : sme2_cvt_vg2_single<"bfcvt",  0b10, 0b00>;
def BFCVTN_Z2Z_StoH : sme2_cvt_vg2_single<"bfcvtn", 0b11, 0b00>;

def SQCVT_Z2Z_StoH  : sme2_cvt_vg2_single<"sqcvt", 0b00, 0b11>;
def UQCVT_Z2Z_StoH  : sme2_cvt_vg2_single<"uqcvt", 0b01, 0b11>;
def SQCVTU_Z2Z_StoH : sme2_cvt_vg2_single<"sqcvtu", 0b10, 0b11>;
defm SQCVT_Z4Z      : sme2_int_cvt_vg4_single<"sqcvt", 0b000>;
defm UQCVT_Z4Z      : sme2_int_cvt_vg4_single<"uqcvt", 0b001>;
defm SQCVTU_Z4Z     : sme2_int_cvt_vg4_single<"sqcvtu", 0b100>;
defm SQCVTN_Z4Z     : sme2_int_cvt_vg4_single<"sqcvtn", 0b010>;
defm SQCVTUN_Z4Z    : sme2_int_cvt_vg4_single<"sqcvtun", 0b110>;
defm UQCVTN_Z4Z     : sme2_int_cvt_vg4_single<"uqcvtn", 0b011>;

def FCVTZS_2Z2Z_StoS : sme2_fp_cvt_vg2_multi<"fcvtzs", 0b010>;
def FCVTZS_4Z4Z_StoS : sme2_fp_cvt_vg4_multi<"fcvtzs", 0b010>;
def FCVTZU_2Z2Z_StoS : sme2_fp_cvt_vg2_multi<"fcvtzu", 0b011>;
def FCVTZU_4Z4Z_StoS : sme2_fp_cvt_vg4_multi<"fcvtzu", 0b011>;
def SCVTF_2Z2Z_StoS  : sme2_fp_cvt_vg2_multi<"scvtf", 0b100>;
def SCVTF_4Z4Z_StoS  : sme2_fp_cvt_vg4_multi<"scvtf", 0b100>;
def UCVTF_2Z2Z_StoS  : sme2_fp_cvt_vg2_multi<"ucvtf", 0b101>;
def UCVTF_4Z4Z_StoS  : sme2_fp_cvt_vg4_multi<"ucvtf", 0b101>;

}


let Predicates = [HasSME2, HasSMEI16I64] in {
defm ADD_VG2_M2ZZ_D  : sme2_mla_add_sub_array_vg2_single_D<"add", 0b10>;
defm ADD_VG4_M4ZZ_D  : sme2_mla_add_sub_array_vg4_single_D<"add", 0b10>;
defm ADD_VG2_M2Z2Z_D : sme2_mla_add_sub_array_vg2_multi_D<"add", 0b10>;
defm ADD_VG4_M4Z4Z_D : sme2_mla_add_sub_array_vg4_multi_D<"add", 0b10>;

defm SUB_VG2_M2ZZ_D  : sme2_mla_add_sub_array_vg2_single_D<"sub", 0b11>;
defm SUB_VG4_M4ZZ_D  : sme2_mla_add_sub_array_vg4_single_D<"sub", 0b11>;
defm SUB_VG2_M2Z2Z_D : sme2_mla_add_sub_array_vg2_multi_D<"sub", 0b11>;
defm SUB_VG4_M4Z4Z_D : sme2_mla_add_sub_array_vg4_multi_D<"sub", 0b11>;

defm ADDA_VG2_M2Z2Z_D : sme2_multivec_accum_add_sub_vg2_D<"add", 0b10>;
defm ADDA_VG4_M4Z4Z_D : sme2_multivec_accum_add_sub_vg4_D<"add", 0b10>;

defm SUBA_VG2_M2Z2Z_D : sme2_multivec_accum_add_sub_vg2_D<"sub", 0b11>;
defm SUBA_VG4_M4Z4Z_D : sme2_multivec_accum_add_sub_vg4_D<"sub", 0b11>;
}

let Predicates = [HasSME2, HasSMEF64F64] in {
defm FMLA_VG2_M2ZZ_D  : sme2_mla_add_sub_array_vg2_single_D<"fmla", 0b00>;
defm FMLA_VG4_M4ZZ_D  : sme2_mla_add_sub_array_vg4_single_D<"fmla", 0b00>;
defm FMLA_VG2_M2Z2Z_D : sme2_mla_add_sub_array_vg2_multi_D<"fmla", 0b00>;
defm FMLA_VG4_M4Z4Z_D : sme2_mla_add_sub_array_vg4_multi_D<"fmla", 0b00>;

defm FMLS_VG2_M2ZZ_D  : sme2_mla_add_sub_array_vg2_single_D<"fmls", 0b01>;
defm FMLS_VG4_M4ZZ_D  : sme2_mla_add_sub_array_vg4_single_D<"fmls", 0b01>;
defm FMLS_VG2_M2Z2Z_D : sme2_mla_add_sub_array_vg2_multi_D<"fmls", 0b01>;
defm FMLS_VG4_M4Z4Z_D : sme2_mla_add_sub_array_vg4_multi_D<"fmls", 0b01>;

defm FADD_VG2_M2Z2Z_D : sme2_multivec_accum_add_sub_vg2_D<"fadd", 0b00>;
defm FADD_VG4_M4Z4Z_D : sme2_multivec_accum_add_sub_vg4_D<"fadd", 0b00>;

defm FSUB_VG2_M2Z2Z_D : sme2_multivec_accum_add_sub_vg2_D<"fsub", 0b01>;
defm FSUB_VG4_M4Z4Z_D : sme2_multivec_accum_add_sub_vg4_D<"fsub", 0b01>;
}
