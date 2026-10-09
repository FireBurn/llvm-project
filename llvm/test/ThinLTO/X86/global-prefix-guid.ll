; REQUIRES: x86-registered-target

; On a target with a global prefix the linker name of foo is _foo, which is also
; the IR name of another function. Make sure the GUID recorded for the linker
; name _foo does not come from the unrelated IR function _foo, which would make
; ThinLTO treat foo as dead and delete it.

; RUN: opt -module-summary %s -o %t.bc
; RUN: llvm-lto2 run %t.bc -o %t.out -r=%t.bc,_foo,px -r=%t.bc,__foo,px
; RUN: llvm-nm %t.out.1 | FileCheck %s

; CHECK-DAG: T __foo
; CHECK-DAG: T _foo

target datalayout = "e-m:x-p:32:32-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32-a:0:32-S32"
target triple = "i686-pc-windows-msvc"

define dso_local i32 @foo(i32 %x) {
  %r = or i32 %x, 32
  ret i32 %r
}

define dso_local i32 @_foo(i32 %x) {
  %r = or i32 %x, 33
  ret i32 %r
}
