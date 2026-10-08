; RUN: opt -thinlto-bc %s -o %t1.bc
; RUN: opt -thinlto-bc %p/Inputs/import-symver-default-foo.ll -o %t2.bc
; RUN: llvm-lto -thinlto-action=thinlink %t1.bc %t2.bc -o %t3.index.bc

; RUN: llvm-lto -thinlto-action=import -exported-symbol=main %t1.bc -thinlto-index=%t3.index.bc
; RUN: llvm-dis %t1.bc.thinlto.imported.bc -o - | FileCheck %s

; @bar is imported as available_externally, so the importing module does not
; define it. A default version names a definition, which the assembler rejects
; for a symbol the module does not define, so the directive stays behind in the
; module that does define @bar. A non-default version, which is a reference,
; is imported: see import-symver.ll.
; CHECK: define available_externally {{.*}}@bar()
; CHECK-NOT: module asm
; CHECK-NOT: .symver

target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

declare dso_local i32 @foo()

define dso_local i32 @main() {
entry:
  %call = tail call i32 @foo()
  ret i32 %call
}
