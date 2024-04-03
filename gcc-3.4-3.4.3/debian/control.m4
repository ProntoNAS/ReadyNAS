divert(-1)

define(`checkdef',`ifdef($1, , `errprint(`error: undefined macro $1
')m4exit(1)')')
define(`errexit',`errprint(`error: undefined macro `$1'
')m4exit(1)')

dnl The following macros must be defined, when called:
dnl ifdef(`SRCNAME',	, errexit(`SRCNAME'))
dnl ifdef(`CV',		, errexit(`CV'))
dnl ifdef(`NV',		, errexit(`NV'))
dnl ifdef(`PV',		, errexit(`PV'))
dnl ifdef(`ARCH',	, errexit(`ARCH'))

dnl The architecture will also be defined (-D__i386__, -D__powerpc__, etc.)

define(`PN', `$1')
ifdef(`PRI', `', `
    define(`PRI', `$1')
')
define(`MAINTAINER', `Debian GCC maintainers <debian-gcc@lists.debian.org>')

define(`ifenabled', `ifelse(index(enabled_languages, `$1'), -1, `dnl', `$2')')

divert`'dnl
dnl --------------------------------------------------------------------------
Source: SRCNAME
Section: devel
Priority: PRI(standard)
Maintainer: MAINTAINER
Uploaders: Matthias Klose <doko@debian.org>, Gerhard Tonn <gt@debian.org>
Standards-Version: 3.6.1
ifdef(`TARGET',`dnl cross
Build-Depends: LIBC_BUILD_DEP, m4, autoconf2.13, autoconf, automake1.4, automake1.7, libtool, autotools-dev, gawk, bzip2, dpkg-cross (>= 1.18.1), BINUTILS_BUILD_DEP, debhelper (>= 4.1), bison (>= 1:1.875a-1) | bison (<< 1:1.50), flex, realpath (>= 1.9.12)`'TARGETBD
',`dnl native
Build-Depends: LIBC_BUILD_DEP, libc6-dev-sparc64 [sparc], libc6-dev-s390x [s390], amd64-libs-dev [i386], ia32-libs-dev [amd64], libunwind7-dev (>= 0.98.5-1) [ia64], libatomic-ops-dev [ia64], m4, autoconf2.13, autoconf, automake1.4, automake1.7, libtool, autotools-dev, gawk, dejagnu (>= 1.4.3) [check_no_archs], expect (>= 5.38.0) [check_no_archs], bzip2, BINUTILS_BUILD_DEP, binutils-hppa64 [hppa], debhelper (>= 4.1), gperf (>= 2.7-3), bison (>= 1:1.875a-1) | bison (<< 1:1.50), flex, gettext, texinfo (>= 4.3), zlib1g-dev, libgc-dev [libgc_no_archs], xlibs-dev, gnat-3.3 [ada_no_archs] | gnat-3.4 [ada_no_archs] | gnat [i386 powerpc sparc], libncurses5-dev [pascal_no_archs], libgmp3-dev, tetex-bin [pascal_no_archs], locales [locale_no_archs !hurd-i386], procps [check_no_archs], help2man [pascal_no_archs], sharutils, libgtk2.0-dev (>= 2.4.4-2) [java_no_archs], libart-2.0-dev [java_no_archs], g++-3.3 [!amd64], g77-3.3 [!amd64], gobjc-3.3 [!amd64], realpath (>= 1.9.12)
Build-Depends-Indep: doxygen (>= 1.3.9.1), graphviz (>= 2.0)
')dnl

ifdef(`TARGET', `', `
Package: gcc`'PV-base
Architecture: any
Section: devel
Priority: PRI(required)
Description: The GNU Compiler Collection (base package)
 This package contains files common to all languages and libraries
 contained in the GNU Compiler Collection (GCC).
ifdef(`BASE_ONLY', `dnl
 .
 This version of GCC is not yet available for this architecture.
 Please use the compilers from the gcc-snapshot package for testing.
')`'dnl
')`'dnl native

ifenabled(`libgcc',`
Package: libgcc1`'LS
Architecture: ifdef(`TARGET',`all',`any')
Section: ifdef(`TARGET',`devel',`libs')
Priority: ifdef(`TARGET',`extra',required)
Depends: ${shlibs:Depends}
ifelse(ARCH,`ia64',`dnl
Conflicts: libunwind7`'LS (<< 0.98.3-4)
Replaces: libunwind7`'LS (<< 0.98.3-4)
')`'dnl
Description: GCC support library`'ifdef(`TARGET)',` (TARGET)', `')
 Shared version of the support library, a library of internal subroutines
 that GCC uses to overcome shortcomings of particular machines, or
 special needs for some languages.
ifdef(`TARGET', `dnl
 .
 This package contains files for TARGET architecture, for use in cross-compile
 environment.
')`'dnl

Package: libgcc2`'LS
Architecture: ifdef(`TARGET',`all',`any')
Section: ifdef(`TARGET',`devel',`libs')
Priority: ifdef(`TARGET',`extra',PRI(required))
Depends: ${shlibs:Depends}
Description: GCC support library`'ifdef(`TARGET)',` (TARGET)', `')
 Shared version of the support library, a library of internal subroutines
 that GCC uses to overcome shortcomings of particular machines, or
 special needs for some languages.
ifdef(`TARGET', `dnl
 .
 This package contains files for TARGET architecture, for use in cross-compile
 environment.
')`'dnl
')`'dnl libgcc

ifenabled(`lib64gcc',`
Package: lib64gcc`'GCC_SO`'LS
Architecture: ifdef(`TARGET',`all',`any')
Section: ifdef(`TARGET',`devel',`libs')
Priority: ifdef(`TARGET',`extra',PRI(optional))
Depends: ${shlibs:Depends}
Conflicts: libgcc`'GCC_SO`'LS (<= 1:3.3-0pre9)
Description: GCC support library`'ifdef(`TARGET)',` (TARGET)', `') (64bit)
 Shared version of the support library, a library of internal subroutines
 that GCC uses to overcome shortcomings of particular machines, or
 special needs for some languages.
ifdef(`TARGET', `dnl
 .
 This package contains files for TARGET architecture, for use in cross-compile
 environment.
')`'dnl
')`'dnl lib64gcc

ifdef(`TARGET', `', `dnl
Package: lib32gcc`'GCC_SO
Architecture: any
Section: libs
Priority: optional
Depends: ia32-libs (>= 0.7)
Description: GCC support library (ia32)
 Shared version of the support library, a library of internal subroutines
 that GCC uses to overcome shortcomings of particular machines, or
 special needs for some languages.
')dnl

ifenabled(`cdev',`
Package: gcc`'PV`'TS
Architecture: any
Section: devel
Priority: ifdef(`TARGET',`extra',`PRI(standard)')
Depends: gcc`'PV-base (>= CV), libgcc`'GCC_SO`'LS (>= ifelse(GCC_SO,1,CEV,CV)), ${shlibs:Depends}, cpp`'PV`'TS (>= CV), cpp`'PV`'TS (<< NV), binutils`'TS (>= BINUTILSV)ifelse(ARCH,`ia64',`, libunwind7-dev`'LS (>= 0.98.5-1)')
Recommends: LIBC_DEP
Conflicts: gcc-3.2`'TS (<= 1:3.2.3-0pre8)
Suggests: gcc`'PV-doc (>= CV)ifelse(ARCH,`i386',`, amd64-libs-dev',ARCH,`amd64',`, lib32gcc1')
Provides: c-compiler`'TS
Description: The GNU C compiler`'ifdef(`TARGET)',` (cross compiler for TARGET architecture)', `')
 This is the GNU C compiler, a fairly portable optimizing compiler for C.
 .
 ABIs changed between gcc-3.3 and gcc-3.4 on some architectures (hppa, m68k,
 mips, mipsel, sparc). Please read /usr/share/doc/gcc-3.4/README.Debian
 for more details.
ifdef(`TARGET', `dnl
 .
 This package contains C cross-compiler for TARGET architecture.
')`'dnl
')`'dnl cdev

ifenabled(`cdev',`
ifdef(`TARGET', `', `
Package: gcc`'PV-hppa64
Architecture: hppa
Section: devel
Priority: PRI(optional)
Depends: gcc`'PV-base (>= CV), gcc`'PV-base (<< NV), libc6-dev (>= 2.3.2.ds1-12), ${shlibs:Depends}
Conflicts: gcc-3.3-hppa64 (<= 1:3.3.4-5), gcc-3.5-hppa64 (<= 3.5-0pre1)
Description: The GNU C compiler (cross compiler for hppa64)
 This is the GNU C compiler, a fairly portable optimizing compiler for C.
')`'dnl native
')`'dnl cdev

ifenabled(`cdev',`
Package: cpp`'PV`'TS
Architecture: any
Section: ifdef(`TARGET',`devel',`interpreters')
Priority: ifdef(`TARGET',`extra',`PRI(standard)')
Depends: gcc`'PV-base (>= CV), gcc`'PV-base (<< NV), ${shlibs:Depends}
Description: The GNU C preprocessor
 A macro processor that is used automatically by the GNU C compiler
 to transform programs before actual compilation.
 .
 This package has been separated from gcc for the benefit of those who
 require the preprocessor but not the compiler.
ifdef(`TARGET', `dnl
 .
 This package contains preprocessor configured for TARGET architecture.
')`'dnl

ifdef(`TARGET', `', `
Package: cpp`'PV-doc
Architecture: all
Section: doc
Priority: PRI(optional)
Depends: gcc`'PV-base (>= CV)
Replaces: cpp (<= 1:2.93.12)
Description: Documentation for the GNU C preprocessor (cpp)
 Documentation for the GNU C preprocessor in info `format'.
')`'dnl native
')`'dnl cdev

ifenabled(`c++',`
ifenabled(`c++dev',`
Package: g++`'PV`'TS
Architecture: any
Section: devel
Priority: ifdef(`TARGET',`extra',`PRI(standard)')
ifdef(`OLDARMABI',`dnl
Depends: gcc`'PV-base (>= CV), ${shlibs:Depends}, gcc`'PV`'TS (>= CV), gcc`'PV`'TS (<< NV), libstdc++6-0-dev`'LS (>= CV)
',`dnl new arm abi
Depends: gcc`'PV-base (>= CV), ${shlibs:Depends}, gcc`'PV`'TS (>= CV), gcc`'PV`'TS (<< NV), libstdc++CXX_SO-dev`'LS (>= CV)
')`'dnl
Replaces: gcc`'TS (<= 2.7.2.3-3)
Provides: c++-compiler`'TS
Suggests: gcc`'PV-doc (>= CV)ifelse(ARCH,`amd64',`, lib32stdc++6')
Description: The GNU C++ compiler`'ifdef(`TARGET)',` (cross compiler for TARGET architecture)', `')
 This is the GNU C++ compiler, a fairly portable optimizing compiler for C++.
 .
 ABIs changed between gcc-3.3 and gcc-3.4 on some architectures (hppa, m68k,
 mips, mipsel, sparc). Please read /usr/share/doc/gcc-3.4/README.Debian
 for more details. Do not mix code compiled with g++-3.3 and g++-3.4.
ifdef(`TARGET', `dnl
 .
 This package contains C++ cross-compiler for TARGET architecture.
')`'dnl
')`'dnl c++dev
')`'dnl c++

ifenabled(`proto',`
Package: protoize
Architecture: any
Priority: PRI(optional)
Depends: gcc`'PV-base (>= CV), ${shlibs:Depends}, gcc`'PV (>= CV), gcc`'PV (<< NV)
Description: Create/remove ANSI prototypes from C code
 "protoize" can be used to add prototypes to a program, thus converting
 the program to ANSI C in one respect.  The companion program "unprotoize"
 does the reverse: it removes argument types from any prototypes
 that are found.
')`'dnl proto

ifenabled(`objc',`
ifenabled(`objcdev',`
Package: gobjc`'PV
Architecture: any
Priority: PRI(optional)
Depends: gcc`'PV-base (>= CV), ${shlibs:Depends}, libobjc`'OBJC_SO (>= 1:3.3.4-4), gcc`'PV (>= CV), gcc`'PV (<< NV)
Suggests: gcc`'PV-doc (>= CV)
Provides: objc-compiler
ifdef(`OBJC_GC',`Recommends: libgc-dev', `dnl')
ifdef(`__sparc__',`Conflicts: gcc`'PV-sparc64', `dnl')
Description: The GNU Objective-C compiler
 This is the GNU Objective-C compiler, which compiles
 Objective-C on platforms supported by the gcc compiler. It uses the
 gcc backend to generate optimized code.
')`'dnl objcdev

ifenabled(`libobjc',`
Package: libobjc`'OBJC_SO
Section: libs
Architecture: any
Priority: PRI(optional)
Depends: gcc`'PV-base (>= CV), ${shlibs:Depends}
Description: Runtime library for GNU Objective-C applications
 Library needed for GNU ObjC applications linked against the shared library.
')`'dnl libobjc

ifenabled(`lib64objc',`
Package: lib64objc`'OBJC_SO
Section: libs
Architecture: any
Priority: PRI(optional)
Depends: gcc`'PV-base (>= CV), ${shlibs:Depends}
Description: Runtime library for GNU Objective-C applications (64bit)
 Library needed for GNU ObjC applications linked against the shared library.
')`'dnl lib64objc
')`'dnl objc

ifenabled(`f77',`
ifenabled(`fdev',`
Package: g77`'PV
Architecture: any
Priority: PRI(optional)
Depends: gcc`'PV-base (>= CV), libg2c`'G2C_SO-dev (>= 1:3.3.4-4), ${shlibs:Depends}, gcc`'PV (>= CV), gcc`'PV (<< NV)
Provides: fortran77-compiler
Suggests: g77`'PV-doc
Conflicts: g77-2.95 (<= 1:2.95.4-19), g77-3.0 (<= 1:3.0.4-16), g77-3.2 (<= 1:3.2.3-9), g77-3.3 (<= 1:3.3.4-3)
Description: The GNU Fortran 77 compiler
 This is the GNU g77 Fortran compiler, which compiles
 Fortran 77 on platforms supported by the gcc compiler. It uses the
 gcc backend to generate optimized code.

Package: g77`'PV-doc
Architecture: all
Section: doc
Priority: PRI(optional)
Depends: gcc`'PV-base (>= CV)
Replaces: g77 (<= 1:2.91.58-3)
Description: Documentation for the GNU Fortran compiler (g77)
 Documentation for the GNU Fortran 77 compiler in info `format'.
')`'dnl fdev

ifenabled(`libg2c',`
Package: libg2c`'G2C_SO
Section: libs
Architecture: any
Priority: PRI(optional)
Depends: gcc`'PV-base (>= CV), ${shlibs:Depends}
Description: Runtime library for GNU Fortran 77 applications
 Library needed for GNU Fortran 77 applications linked against the
 shared library.

Package: libg2c`'G2C_SO-dev
Section: libdevel
Architecture: any
Priority: PRI(optional)
Depends: gcc`'PV-base (>= CV), libg2c`'G2C_SO (>= CEV), ${shlibs:Depends}
Conflicts: g77-2.95 (<= 1:2.95.4-19), g77-3.0 (<= 1:3.0.4-16), g77-3.2 (<= 1:3.2.3-9), g77-3.3 (<= 1:3.3.3-0pre3)
Description: GNU Fortran 77 library development
 Headers and static libraries for g2c.
')`'dnl libg2c

ifenabled(`lib64g2c',`
Package: lib64g2c`'G2C_SO
Section: libs
Architecture: any
Priority: PRI(optional)
Depends: gcc`'PV-base (>= CV), ${shlibs:Depends}
Description: Runtime library for GNU Fortran 77 applications (64bit)
 Library needed for GNU Fortran 77 applications linked against the
 shared library.
')`'dnl lib64g2c
')`'dnl f77

ifenabled(`java',`
ifenabled(`javadev',`
Package: gcj`'PV
Architecture: any
Priority: PRI(optional)
Depends: gcc`'PV-base (>= CV), ${shlibs:Depends}, libgcj`'GCJ_SO-common (>= CV), java-common, g++`'PV (>= CV), g++`'PV (<< NV)
Recommends: fastjar, gij`'PV (>= CV), libgcj`'GCJ_SO-dev (>= CV)
Provides: java-compiler
Suggests: libgcj`'GCJ_SO-awt (>= CV)
Description: The GNU compiler for Java(TM)
 GCJ is a front end to the GCC compiler which can natively compile both
 Java(tm) source and bytecode files. The compiler can also generate class
 files.
')`'dnl javadev

ifenabled(`libgcj',`
Package: gij`'PV
Priority: optional
Architecture: any
Depends: gcc`'PV-base (>= CV), libgcj`'GCJ_SO (>= CV), ${shlibs:Depends}
Suggests: fastjar, gcj`'PV (>= CV), libgcj`'GCJ_SO-awt (>= CV)
Provides: java-virtual-machine, java1-runtime
Description: The GNU Java bytecode interpreter
 GIJ is not limited to interpreting bytecode. It includes a class loader which
 can dynamically load shared objects, so it is possible to give it the name
 of a class which has been compiled and put into a shared library on the
 class path.

Package: libgcj-common
Section: libs
Architecture: all
Priority: PRI(optional)
Depends: gcc`'PV-base (>= SOFT_CV)
Conflicts: classpath (<= 0.04-4), libgcj3 (<< 1:3.2-0pre2)
Description: Java runtime library (common files)
 This package contains files shared by classpath and libgcj libraries.

Package: libgcj`'GCJ_SO
Section: libs
Architecture: any
Priority: PRI(optional)
Depends: libgcj-common, ${shlibs:Depends}
Recommends: libgcj`'GCJ_SO-common (>= CV)
Description: Java runtime library for use with gcj
 This is the runtime that goes along with the gcj front end to
 gcc. libgcj includes parts of the Java Class Libraries, plus glue to
 connect the libraries to the compiler and the underlying OS.

Package: libgcj`'GCJ_SO-common
Section: libs
Architecture: all
Priority: PRI(optional)
Depends: libgcj`'GCJ_SO (>= SOFT_CV)
Conflicts: libgcj5 (<< 3.4.0-1)
Replaces: libgcj5 (<< 3.4.0-1)
Description: Java runtime library for use with gcj (jar files)
 This is the jar file that goes along with the gcj front end to gcc.

Package: libgcj`'GCJ_SO-awt
Section: libs
Architecture: any
Priority: PRI(optional)
Depends: libgcj`'GCJ_SO (>= CV), ${shlibs:Depends}
Conflicts: libgcj5 (<< 3.4.0-2), libgcj-awt5
Replaces: libgcj5 (<< 3.4.0-2), libgcj-awt5
Description: AWT peer runtime libraries for use with gcj
 These are runtime libraries holding the AWT peer implementations
 for libgcj (currently only the GTK based peer library).
')`'dnl libgcj

ifenabled(`lib64gcj',`
Package: lib64gcj`'GCJ_SO
Section: libs
Architecture: any
Priority: PRI(optional)
Depends: libgcj-common, ${shlibs:Depends}
Description: Java runtime library for use with gcj (64bit)
 This is the runtime that goes along with the gcj front end to
 gcc. libgcj includes parts of the Java Class Libraries, plus glue to
 connect the libraries to the compiler and the underlying OS.

Package: lib64gcj`'GCJ_SO-awt
Section: libs
Architecture: any
Priority: PRI(optional)
Depends: libgcj`'GCJ_SO (>= CV), ${shlibs:Depends}
Conflicts: lib64gcj5 (<< 3.4.0-2), lib64gcj-awt5
Replaces: lib64gcj5 (<< 3.4.0-2), lib64gcj-awt5
Description: AWT peer runtime libraries for use with gcj (64bit)
 These are runtime libraries holding the AWT peer implementations
 for libgcj (currently only the GTK based peer library).
')`'dnl libgcj

ifenabled(`javadev',`
Package: libgcj`'GCJ_SO-dev
Section: libdevel
Architecture: any
Priority: PRI(optional)
Depends: gcj`'PV (>= CV), libgcj`'GCJ_SO (>= CV), libgcj`'GCJ_SO-common (>= CV), libgcj`'GCJ_SO-awt (>= CV), LIBC_DEP, zlib1g-dev, ${shlibs:Depends}
Conflicts: libgcj2-dev, libgcj3-dev, libgcj4-dev
Provides: libgcj-dev
Description: Java development headers and static library for use with gcj
 This is the development headers and static libraries that go along
 with the gcj front end to gcc. libgcj includes parts of the Java Class
 Libraries, plus glue to connect the libraries to the compiler and the
 underlying OS.
')`'dnl javadev
')`'dnl java

ifenabled(`fastjar',`
Package: fastjar
Section: devel
Architecture: any
Priority: PRI(optional)
Depends: ${shlibs:Depends}
Description: Jar creation utility
 Replacement for Suns .jar creation program.  It is written in C
 instead of java and is tons faster.  It is currently not complete.
')`'dnl fastjar

ifenabled(`libffi',`
Package: libffi`'FFI_SO
Section: libs
Architecture: any
Priority: PRI(optional)
Depends: gcc`'PV`'-base, ${shlibs:Depends}
Description: Foreign Function Interface library runtime
 A foreign function interface is the popular name for the interface that
 allows code written in one language to call code written in another
 language.

Package: lib64ffi`'FFI_SO
Section: libs
Architecture: any
Priority: PRI(optional)
Depends: gcc`'PV`'-base, ${shlibs:Depends}
Description: Foreign Function Interface library runtime (64bit)
 A foreign function interface is the popular name for the interface that
 allows code written in one language to call code written in another
 language.

Package: libffi`'FFI_SO-dev
Section: libdevel
Architecture: any
Priority: PRI(optional)
Depends: gcc`'PV`'-base, libffi`'FFI_SO (>= CEV), LIBC_DEP
Recommends: gcc-3.4
Conflicts: libffi1-dev, libffi2-dev
Description: Foreign Function Interface library (development files)
 This package contains the headers and static library files necessary for
 building building programs which use libffi.
 .
 Use this package together with gcc-3.4.
 .
 A foreign function interface is the popular name for the interface that
 allows code written in one language to call code written in another
 language.
')`'dnl libffi

ifenabled(`c++',`
ifenabled(`libcxx',`
Package: libstdc++CXX_SO`'LS
Architecture: ifdef(`TARGET',`all',`any')
Section: ifdef(`TARGET',`devel',`libs')
Priority: ifdef(`TARGET',`extra',PRI(required))
Depends: gcc`'PV-base (>= CV), ${shlibs:Depends}
Description: The GNU Standard C++ Library v3`'ifdef(`TARGET)',` (TARGET)', `')
 This package contains an additional runtime library for C++ programs
 built with the GNU compiler.
 .
 libstdc++-v3 is a complete rewrite from the previous libstdc++-v2, which
 was included up to g++-2.95. The first version of libstdc++-v3 appeared
 in g++-3.0.
ifdef(`TARGET', `dnl
 .
 This package contains files for TARGET architecture, for use in cross-compile
 environment.
')`'dnl

Package: libstdc++6-0`'LS
Architecture: ifdef(`TARGET', `all', `amd64 arm')
Section: ifdef(`TARGET',`devel',`base')
Priority: ifdef(`TARGET',`extra',PRI(required))
Depends: gcc`'PV-base (>= CV), ${shlibs:Depends}
Description: The GNU Standard C++ Library v3`'ifdef(`TARGET)',` (TARGET)', `')
 This package contains an additional runtime library for C++ programs
 built with the GNU compiler.
 .
 libstdc++-v3 is a complete rewrite from the previous libstdc++-v2, which
 was included up to g++-2.95. The first version of libstdc++-v3 appeared
 in g++-3.0.
ifdef(`TARGET', `dnl
 .
 This package contains files for TARGET architecture, for use in cross-compile
 environment.
')`'dnl

Package: lib32stdc++CXX_SO`'LS
Architecture: ifdef(`TARGET',`all',`any')
Section: ifdef(`TARGET',`devel',`libs')
Priority: ifdef(`TARGET',`extra',PRI(optional))
Depends: gcc`'PV-base (>= CV), lib32gcc`'GCC_SO
Description: The GNU Standard C++ Library v3 (ia32)
 This package contains an additional runtime library for C++ programs
 built with the GNU compiler.
ifdef(`TARGET', `dnl
 .
 This package contains files for TARGET architecture, for use in cross-compile
 environment.
')`'dnl

Package: lib32stdc++6-0`'LS
Architecture: ifdef(`TARGET',`all',`any')
Section: ifdef(`TARGET',`devel',`libs')
Priority: ifdef(`TARGET',`extra',PRI(optional))
Depends: gcc`'PV-base (>= CV), lib32gcc`'GCC_SO`'LS
Description: The GNU Standard C++ Library v3 (ia32)
 This package contains an additional runtime library for C++ programs
 built with the GNU compiler.
ifdef(`TARGET', `dnl
 .
 This package contains files for TARGET architecture, for use in cross-compile
 environment.
')`'dnl
')`'dnl libcxx

ifenabled(`lib64cxx',`
Package: lib64stdc++CXX_SO`'LS
Architecture: ifdef(`TARGET',`all',`any')
Section: ifdef(`TARGET',`devel',`libs')
Priority: ifdef(`TARGET',`extra',PRI(important))
Depends: gcc`'PV-base (>= CV), ${shlibs:Depends}
Description: The GNU Standard C++ Library v3`'ifdef(`TARGET)',` (TARGET)', `') (64bit)
 This package contains an additional runtime library for C++ programs
 built with the GNU compiler.
 .
 libstdc++-v3 is a complete rewrite from the previous libstdc++-v2, which
 was included up to g++-2.95. The first version of libstdc++-v3 appeared
 in g++-3.0.
ifdef(`TARGET', `dnl
 .
 This package contains files for TARGET architecture, for use in cross-compile
 environment.
')`'dnl
')`'dnl lib64cxx

ifenabled(`c++dev',`
Package: libstdc++CXX_SO-dev`'LS
Architecture: ifdef(`TARGET',`all',`any')
Section: ifdef(`TARGET',`devel',`libdevel')
Priority: ifdef(`TARGET',`extra',PRI(standard))
Depends: gcc`'PV-base (>= CV), libstdc++CXX_SO`'LS (>= CV), LIBC_DEP, g++`'PV`'TS (>= CV)
ifdef(`TARGET',`',`dnl
Conflicts: libg++27-dev, libg++272-dev (<< 2.7.2.8-1), libstdc++2.8-dev, libg++2.8-dev, libstdc++2.9-dev, libstdc++2.9-glibc2.1-dev, libstdc++2.10-dev (<< 1:2.95.3-2), libstdc++3.0-dev
Suggests: libstdc++CXX_SO-doc, stl-manual
')`'dnl native
Provides: libstdc++-dev`'LS
Description: The GNU Standard C++ Library v3 (development files)`'ifdef(`TARGET)',` (TARGET)', `')
 This package contains the headers and static library files necessary for
 building C++ programs which use libstdc++. Be advised that this only works
 with the GNU C++ compiler (version 3.0), and no earlier library will work it.
 .
 libstdc++-v3 is a complete rewrite from the previous libstdc++-v2, which
 was included up to g++-2.95. The first version of libstdc++-v3 appeared
 in g++-3.0.
ifdef(`TARGET', `dnl
 .
 This package contains files for TARGET architecture, for use in cross-compile
 environment.
')`'dnl

Package: libstdc++6-0-dev`'LS
Architecture: ifdef(`TARGET',`all',`amd64 arm')
Section: ifdef(`TARGET',`devel',`libdevel')
Priority: ifdef(`TARGET',`extra',PRI(standard))
Depends: gcc`'PV-base (>= CV), libstdc++6-0`'LS (>= CV), LIBC_DEP, g++`'PV`'TS (>= CV)
ifdef(`TARGET',`',`dnl native
Conflicts: libg++27-dev, libg++272-dev (<< 2.7.2.8-1), libstdc++2.8-dev, libg++2.8-dev, libstdc++2.9-dev, libstdc++2.9-glibc2.1-dev, libstdc++2.10-dev (<< 1:2.95.3-2), libstdc++3.0-dev, libstdc++6-dev (<< 3.4.1-2)
Suggests: libstdc++CXX_SO-doc, stl-manual
')`'dnl native
Provides: libstdc++-dev`'LS
Description: The GNU Standard C++ Library v3 (development files)`'ifdef(`TARGET)',` (TARGET)', `')
 This package contains the headers and static library files necessary for
 building C++ programs which use libstdc++. Be advised that this only works
 with the GNU C++ compiler (version 3.0), and no earlier library will work it.
 .
 libstdc++-v3 is a complete rewrite from the previous libstdc++-v2, which
 was included up to g++-2.95. The first version of libstdc++-v3 appeared
 in g++-3.0.
ifdef(`TARGET', `dnl
 .
 This package contains files for TARGET architecture, for use in cross-compile
 environment.
')`'dnl

Package: libstdc++CXX_SO-pic`'LS
Architecture: ifdef(`TARGET',`all',`any')
Section: ifdef(`TARGET',`devel',`libdevel')
Priority: extra
Depends: gcc`'PV-base (>= CV), libstdc++CXX_SO`'LS (>= CV), libstdc++CXX_SO-dev`'LS (>= CV)
Description: The GNU Standard C++ Library v3 (shared library subset kit)`'ifdef(`TARGET)',` (TARGET)', `')
 This is used to develop subsets of the libstdc++ shared libraries for
 use on custom installation floppies and in embedded systems.
 .
 Unless you are making one of those, you will not need this package.
ifdef(`TARGET', `dnl
 .
 This package contains files for TARGET architecture, for use in cross-compile
 environment.
')`'dnl

Package: libstdc++6-0-pic`'LS
Architecture: ifdef(`TARGET',`all',`amd64 arm')
Section: ifdef(`TARGET',`devel',`libdevel')
Priority: extra
Depends: gcc`'PV-base (>= CV), libstdc++6-0`'LS (>= CV), libstdc++6-0-dev`'LS (>= CV)
Description: The GNU Standard C++ Library v3 (shared library subset kit)`'ifdef(`TARGET)',` (TARGET)', `')
 This is used to develop subsets of the libstdc++ shared libraries for
 use on custom installation floppies and in embedded systems.
 .
 Unless you are making one of those, you will not need this package.
ifdef(`TARGET', `dnl
 .
 This package contains files for TARGET architecture, for use in cross-compile
 environment.
')`'dnl

Package: libstdc++CXX_SO-dbg`'LS
Architecture: ifdef(`TARGET',`all',`any')
Section: ifdef(`TARGET',`devel',`libdevel')
Priority: extra
Depends: gcc`'PV-base (>= CV), libstdc++CXX_SO`'LS (>= CV)
Recommends: libstdc++CXX_SO-dev`'LS (>= CV)
Conflicts: libstdc++5-dbg`'LS, libstdc++5-3.3-dbg`'LS, libstdc++6-4.0-dbg`'LS
Description: The GNU Standard C++ Library v3 (debugging files)`'ifdef(`TARGET)',` (TARGET)', `')
 This package contains the shared library of libstdc++ compiled with
 debugging symbols.
ifdef(`TARGET', `dnl
 .
 This package contains files for TARGET architecture, for use in cross-compile
 environment.
')`'dnl

Package: libstdc++6-0-dbg`'LS
Architecture: ifdef(`TARGET',`all',`amd64 arm')
Section: ifdef(`TARGET',`devel',`libdevel')
Priority: extra
Depends: gcc`'PV-base (>= CV), libstdc++6-0`'LS (>= CV)
Recommends: libstdc++6-0-dev`'LS (>= CV)
Conflicts: libstdc++5-dbg`'LS, libstdc++5-3.3-dbg`'LS, libstdc++6-dbg`'LS (<< 3.4.1-2), libstdc++6-4.0-dbg`'LS
Description: The GNU Standard C++ Library v3 (debugging files)`'ifdef(`TARGET)',` (TARGET)', `')
 This package contains the shared library of libstdc++ compiled with
 debugging symbols.
ifdef(`TARGET', `dnl
 .
 This package contains files for TARGET architecture, for use in cross-compile
 environment.
')`'dnl

ifdef(`TARGET', `', `
Package: libstdc++6-doc
Architecture: all
Section: doc
Priority: PRI(optional)
Depends: gcc`'PV-base (>= CV)
Conflicts: libstdc++5-doc, libstdc++5-3.3-doc
Description: The GNU Standard C++ Library v3 (documentation files)
 This package contains documentation files for the GNU stdc++ library.
 .
 One set is the distribution documentation, the other set is the
 source documentation including a namespace list, class hierarchy,
 alphabetical list, compound list, file list, namespace members,
 compound members and file members.
')`'dnl native
')`'dnl c++dev
')`'dnl c++

ifenabled(`ada',`
Package: gnat`'-GNAT_V
Architecture: any
Priority: PRI(optional)
Depends: gcc`'PV-base (>= CV), ${shlibs:Depends}, gcc`'PV (>= CV), gcc`'PV (<< NV)
Suggests: gnat`'PV-doc, ada-reference-manual
Provides: ada-compiler
Conflicts: gnat, gnat-3.1, gnat-3.2, gnat-3.3
Provides: gnat
Description: The GNU Ada compiler
 This is the GNU Ada compiler, which compiles Ada on platforms supported
 by the gcc compiler. It uses the gcc backend to generate optimized code.

ifenabled(`libgnat',`
Package: libgnat`'-GNAT_V
Section: libs
Architecture: any
Priority: PRI(optional)
Depends: gcc`'PV-base (>= CV), ${shlibs:Depends}
Description: Runtime library for GNU Ada applications
 Library needed for GNU Ada applications linked against the shared library.
')`'dnl libgnat

ifenabled(`lib64gnat',`
Package: lib64gnat`'-GNAT_V
Section: libs
Architecture: any
Priority: PRI(optional)
Depends: gcc`'PV-base (>= CV), ${shlibs:Depends}
Description: Runtime library for GNU Ada applications
 Library needed for GNU Ada applications linked against the shared library.
')`'dnl libgnat

Package: gnat`'PV-doc
Architecture: all
Section: doc
Priority: PRI(optional)
Depends: gcc`'PV-base (>= CV)
Suggests: gnat`'PV
Description: Documentation for the GNU Ada compiler (gnat)
 Documentation for the GNU Ada compiler in info `format'.
')`'dnl ada

ifenabled(`pascal',`
Package: gpc`'GPC_PV
Architecture: any
Priority: PRI(optional)
Depends: gcc`'PV-base (>= CV), ${shlibs:Depends}, gcc`'PV (>= CV), gcc`'PV (<< NV)
Recommends: libgmp3-dev, libncurses5-dev
Suggests: gpc`'GPC_PV-doc (>= GPC_CV)
Provides: pascal-compiler
Description: The GNU Pascal compiler
 This is the GNU Pascal compiler, which compiles Pascal on platforms supported
 by the gcc compiler. It uses the gcc backend to generate optimized code.
 .
 WARNING: the integration of gpc into gcc-3.x is still in an experimental
 stage. For production use, please use gpc or gpc-2.95.

Package: gpc`'GPC_PV-doc
Architecture: all
Section: doc
Priority: PRI(optional)
Depends: gcc`'PV-base (>= CV)
Replaces: gpc (<= 2.91.58-3)
Suggests: gpc`'GPC_PV
Description: Documentation for the GNU Pascal compiler (gpc)
 Documentation for the GNU Pascal compiler in info `format'.
 .
 WARNING: the integration of gpc into gcc-3.x is still in an experimental
 stage. For production use, please use gpc or gpc-2.95.
')`'dnl pascal

ifenabled(`treelang',`
Package: treelang`'PV
Architecture: any
Priority: PRI(optional)
Depends: gcc`'PV-base (>= CV), ${shlibs:Depends}, gcc`'PV (>= CV), gcc`'PV (<< NV)
Description: The GNU Treelang compiler
 Treelang is a sample language, useful only to help people understand how
 to implement a new language front end to GCC. It is not a useful
 language in itself other than as an example or basis for building a new
 language. Therefore only language developers are likely to have an
 interest in it.
')`'dnl treelang

ifdef(`TARGET',`',`dnl
ifenabled(`libs',`
Package: gcc`'PV-soft-float
Architecture: arm
Priority: PRI(optional)
Depends: gcc`'PV-base (>= CV), ${shlibs:Depends}ifenabled(`cdev',`, gcc`'PV (>= CV), gcc`'PV (<< NV)')
Replaces: gcc-soft-float-ss (<< NV)
Description: The soft-floating-point gcc libraries (arm)
 These are versions of basic static libraries such as libgcc.a compiled
 with the -msoft-float option, for CPUs without a floating-point unit.
')`'dnl commonlibs
')`'dnl

ifenabled(`fixincl',`
Package: fixincludes
Architecture: any
Priority: PRI(optional)
Depends: gcc`'PV-base (>= CV), ${shlibs:Depends}, gcc`'PV (>= CV), gcc`'PV (<< NV)
Description: Fix non-ANSI header files
 FixIncludes was created to fix non-ANSI system header files. Many
 system manufacturers supply proprietary headers that are not ANSI compliant.
 The GNU compilers cannot compile non-ANSI headers. Consequently, the
 FixIncludes shell script was written to fix the header files.
 .
 Not all packages with header files are installed at gcc's build time, so
 we make fixincludes available at build time of other packages, such that
 checking tools like lintian can make use of it.
')`'dnl proto

ifenabled(`cdev',`
ifdef(`TARGET', `', `
Package: gcc`'PV-doc
Architecture: all
Section: doc
Priority: PRI(optional)
Depends: gcc`'PV-base (>= CV)
Conflicts: gcc-docs (<< 2.95.2)
Replaces: gcc (<=2.7.2.3-4.3), gcc-docs (<< 2.95.2)
Description: Documentation for the GNU compilers (gcc, gobjc, g++)
 Documentation for the GNU compilers in info `format'.
')`'dnl native
')`'dnl cdev

ifdef(`TARGET',`',`dnl
ifenabled(`libnof',`
Package: gcc`'PV-nof
Architecture: powerpc
Priority: PRI(optional)
Depends: gcc`'PV-base (>= CV), ${shlibs:Depends}ifenabled(`cdev',`, gcc`'PV (>= CV), gcc`'PV (<< NV)')
Conflicts: gcc-3.2-nof
Description: The no-floating-point gcc libraries (powerpc)
 These are versions of basic static libraries such as libgcc.a compiled
 with the -msoft-float option, for CPUs without a floating-point unit.
')`'dnl libnof
')`'dnl
dnl
dnl last line in file
