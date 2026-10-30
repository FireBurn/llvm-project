// RUN: %clang_cc1 -triple x86_64-w64-mingw32 -fsyntax-only -verify=coff %s
// RUN: %clang_cc1 -triple x86_64-pc-windows-msvc -fsyntax-only -verify=coff %s
// RUN: %clang_cc1 -triple x86_64-linux-gnu -fsyntax-only -verify=elf %s

int data;

void test(int *p) {
  unsigned a = __builtin_imgrel32(&data);
  // elf-error@-1 {{'__builtin_imgrel32' is only supported on COFF targets}}
  unsigned b = __builtin_imgrel32(p);
  // coff-error@-1 {{argument to '__builtin_imgrel32' must be the address of a global variable or function}}
  // elf-error@-2 {{'__builtin_imgrel32' is only supported on COFF targets}}
  unsigned c = __builtin_imgrel32();
  // coff-error@-1 {{too few arguments to function call, expected 1, have 0}}
  // elf-error@-2 {{too few arguments to function call, expected 1, have 0}}
}
