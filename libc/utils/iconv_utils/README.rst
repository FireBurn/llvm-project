This folder holds the script which generates the character set tables used by
``iconv`` in ``libc/src/iconv``. It is meant to be run by hand by the
maintainers when a table is added or a vendor publishes a new mapping. The
mapping files it reads are not part of the repository; ``gen.py`` lists each
one and where it is published. The files it writes are checked in, and must
not be edited by hand.

Run it from anywhere, with the root of llvm-project and a folder holding the
mapping files::

  python3 libc/utils/iconv_utils/gen.py <llvm-project> <mapping folder>
