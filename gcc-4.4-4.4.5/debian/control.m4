divert(-1)

define(`checkdef',`ifdef($1, , `errprint(`error: undefined macro $1
')m4exit(1)')')
define(`errexit',`errprint(`error: undefined macro `$1'
')m4exit(1)')

dnl The following macros must be defined, when called:
dnl ifdef(`SRCNAME',	, errexit(`SRCNAME'))
dnl ifdef(`PV',		, errexit(`PV'))
dnl ifdef(`ARCH',		, errexit(`ARCH'))

dnl The architecture will also be defined (-D__i386__, -D__powerpc__, etc.)

define(`PN', `$1')
ifdef(`PRI', `', `
    define(`PRI', `$1')
')
define(`MAINTAINER', `Debian GCC Maintainers <debian-gcc@lists.debian.org>')

define(`ifenabled', `ifelse(index(enabled_languages, `$1'), -1, `dnl', `$2')')

divert`'dnl
dnl --------------------------------------------------------------------------
Source: SRCNAME
Section: devel
Priority: PRI(optional)
ifelse(DIST,`Ubuntu',`dnl
ifelse(regexp(SRCNAME, `gnat\|gpc-|gdc-'),0,`dnl
Maintainer: Ubuntu MOTU Developers <ubuntu-motu@lists.ubuntu.com>
', `dnl
Maintainer: Ubuntu Core developers <ubuntu-devel-discuss@lists.ubuntu.com>
')dnl SRCNAME
XSBC-Original-Maintainer: MAINTAINER
', `dnl
Maintainer: MAINTAINER
')dnl DIST
ifelse(regexp(SRCNAME, `gnat'),0,`dnl
Uploaders: Ludovic Brenta <lbrenta@debian.org>
', regexp(SRCNAME, `gdc'),0,`dnl
Uploaders: Iain Buclaw <ibuclaw@ubuntu.com>, Arthur Loiret <aloiret@debian.org>
', regexp(SRCNAME, `gpc'),0,`dnl
Uploaders: Matthias Klose <doko@debian.org>
', `dnl
Uploaders: Matthias Klose <doko@debian.org>, Arthur Loiret <aloiret@debian.org>
')dnl SRCNAME
Standards-Version: 3.9.1
ifdef(`TARGET',`dnl cross
Build-Depends: dpkg-dev (>= 1.14.15), debhelper (>= 5.0.62), dpkg-cross (>= 1.25.99), LIBC_BUILD_DEP, LIBC_BIARCH_BUILD_DEP LIBUNWIND_BUILD_DEP LIBATOMIC_OPS_BUILD_DEP AUTOGEN_BUILD_DEP CLOOG_BUILD_DEP AUTO_BUILD_DEP SOURCE_BUILD_DEP CROSS_BUILD_DEP libmpfr-dev (>= 2.3.0), zlib1g-dev, gawk, lzma, xz-utils, patchutils, BINUTILS_BUILD_DEP, bison (>= 1:2.3), flex, realpath (>= 1.9.12), lsb-release, make (>= 3.81), quilt
',`dnl native
Build-Depends: dpkg-dev (>= 1.14.15), debhelper (>= 5.0.62), g++-multilib [amd64 i386 mips mipsel powerpc ppc64 s390 sparc kfreebsd-amd64], LIBC_BUILD_DEP, LIBC_BIARCH_BUILD_DEP AUTO_BUILD_DEP AUTOGEN_BUILD_DEP libunwind7-dev (>= 0.98.5-6) [ia64], libatomic-ops-dev [ia64], zlib1g-dev, gawk, lzma, xz-utils, patchutils, BINUTILS_BUILD_DEP, binutils-hppa64 (>= BINUTILSV) [hppa], gperf (>= 3.0.1), bison (>= 1:2.3), flex, gettext, texinfo (>= 4.3), libmpfr-dev (>= 2.3.0), FORTRAN_BUILD_DEP locales [locale_no_archs], procps, sharutils, PASCAL_BUILD_DEP JAVA_BUILD_DEP GNAT_BUILD_DEP SPU_BUILD_DEP CLOOG_BUILD_DEP MPC_BUILD_DEP ELF_BUILD_DEP CHECK_BUILD_DEP GDC_BUILD_DEP realpath (>= 1.9.12), chrpath, lsb-release, make (>= 3.81), quilt
Build-Depends-Indep: LIBSTDCXX_BUILD_INDEP JAVA_BUILD_INDEP
')dnl
dnl Build-Conflicts: qt3-dev-tools
ifelse(regexp(SRCNAME, `gnat'),0,`dnl
Homepage: http://gcc.gnu.org/
', regexp(SRCNAME, `gdc'),0,`dnl
Homepage: http://bitbucket.org/goshawk/gdc
', regexp(SRCNAME, `gpc'),0,`dnl
Homepage: http://www.gnu-pascal.de/gpc/h-index.html
', `dnl
Homepage: http://gcc.gnu.org/
')dnl SRCNAME
Vcs-Browser: http://svn.debian.org/viewsvn/gcccvs/branches/sid/gcc`'PV/
Vcs-Svn: svn://svn.debian.org/svn/gcccvs/branches/sid/gcc`'PV

ifelse(SRCNAME,gcc-snapshot,`dnl
Package: gcc-snapshot
Architecture: any
Section: devel
Priority: extra
Depends: binutils`'TS (>= ${binutils:Version}), ${dep:libcbiarchdev}, ${dep:libcdev}, ${dep:libunwinddev}, ${snap:depends}, ${shlibs:Depends}, ${dep:ecj}, python, ${misc:Depends}
Recommends: ${snap:recommends}
Suggests: ${dep:gold}
Provides: c++-compiler`'TS`'ifdef(`TARGET)',`',`, c++abi2-dev')
Description: A SNAPSHOT of the GNU Compiler Collection
 This package contains a recent development SNAPSHOT of all files
 contained in the GNU Compiler Collection (GCC).
 .
 The source code for this package has been exported from SVN trunk.
 .
 DO NOT USE THIS SNAPSHOT FOR BUILDING DEBIAN PACKAGES!
 .
 This package will NEVER hit the testing distribution. It is used for
 tracking gcc bugs submitted to the Debian BTS in recent development
 versions of gcc.
',`dnl gcc-X.Y

dnl default base package dependencies
define(`BASETARGET', `')
define(`BASEDEP', `gcc`'PV-base (= ${gcc:Version})')
define(`SOFTBASEDEP', `gcc`'PV-base (>= ${gcc:SoftVersion})')

dnl base, when building libgcc out of the gcj source; needed if new symbols
dnl in libgcc are used in libgcj.
ifelse(index(SRCNAME, `gcj'), 0, `
define(`BASEDEP', `gcj`'PV-base (= ${gcj:Version})')
define(`SOFTBASEDEP', `gcj`'PV-base (>= ${gcj:SoftVersion})')
')

ifdef(`TARGET', `', `
ifenabled(`gccbase',`

Package: gcc`'PV-base
Architecture: any
ifdef(`MULTIARCH', `Multi-Arch: same
')`'dnl
Section: libs
Priority: PRI(required)
Depends: ${misc:Depends}
Replaces: ${base:Replaces}
Description: The GNU Compiler Collection (base package)
 This package contains files common to all languages and libraries
 contained in the GNU Compiler Collection (GCC).
ifdef(`BASE_ONLY', `dnl
 .
 This version of GCC is not yet available for this architecture.
 Please use the compilers from the gcc-snapshot package for testing.
')`'dnl
')`'dnl
')`'dnl native

ifenabled(`gccxbase',`
dnl override default base package dependencies to cross version
dnl This creates a toolchain that doesnt depend on the system -base packages
define(`BASETARGET', `PV`'TS')
define(`BASEDEP', `gcc`'BASETARGET-base (= ${gcc:Version})')
define(`SOFTBASEDEP', `gcc`'BASETARGET-base (>= ${gcc:SoftVersion})')

Package: gcc`'BASETARGET-base
Architecture: any
Section: devel
Priority: PRI(extra)
Depends: ${misc:Depends}
Description: The GNU Compiler Collection (base package)
 This package contains files common to all languages and libraries
 contained in the GNU Compiler Collection (GCC).
')`'dnl

ifenabled(`java',`
Package: gcj`'PV-base
Architecture: any
Section: libs
Priority: PRI(optional)
Depends: ${misc:Depends}
Description: The GNU Compiler Collection (gcj base package)
 This package contains files common to all java related packages
 built from the GNU Compiler Collection (GCC).
')`'dnl java

ifenabled(`ada',`
Package: gnat`'PV-base
Architecture: any
Section: libs
Priority: PRI(optional)
Depends: ${misc:Depends}
Description: The GNU Compiler Collection (gnat base package)
 This package contains files common to all Ada related packages
 built from the GNU Compiler Collection (GCC).
')`'dnl ada

ifenabled(`libgcc',`
Package: libgcc1`'LS
Architecture: ifdef(`TARGET',`all',`any')
Section: ifdef(`TARGET',`devel',`libs')
Priority: ifdef(`TARGET',`extra',required)
Depends: BASEDEP, ${shlibs:Depends}, ${misc:Depends}
ifdef(`TARGET',`Provides: libgcc1-TARGET-dcv1
',ifdef(`MULTIARCH', `Multi-Arch: same
'))`'dnl
Description: GCC support library`'ifdef(`TARGET)',` (TARGET)', `')
 Shared version of the support library, a library of internal subroutines
 that GCC uses to overcome shortcomings of particular machines, or
 special needs for some languages.
ifdef(`TARGET', `dnl
 .
 This package contains files for TARGET architecture, for use in cross-compile
 environment.
')`'dnl

Package: libgcc1-dbg`'LS
Architecture: ifdef(`TARGET',`all',`any')
Section: debug
Priority: extra
Depends: BASEDEP, libgcc1`'LS (= ${gcc:EpochVersion}), ${misc:Depends}
ifdef(`TARGET',`',ifdef(`MULTIARCH', `Multi-Arch: same
'))`'dnl
Description: GCC support library (debug symbols)`'ifdef(`TARGET)',` (TARGET)', `')
 Debug symbols for the GCC support library.
ifdef(`TARGET', `dnl
 .
 This package contains files for TARGET architecture, for use in cross-compile
 environment.
')`'dnl

Package: libgcc2`'LS
Architecture: ifdef(`TARGET',`all',`m68k')
Section: ifdef(`TARGET',`devel',`libs')
Priority: ifdef(`TARGET',`extra',required)
Depends: BASEDEP, ${shlibs:Depends}, ${misc:Depends}
ifdef(`TARGET',`Provides: libgcc2-TARGET-dcv1
',ifdef(`MULTIARCH', `Multi-Arch: same
'))`'dnl
Description: GCC support library`'ifdef(`TARGET)',` (TARGET)', `')
 Shared version of the support library, a library of internal subroutines
 that GCC uses to overcome shortcomings of particular machines, or
 special needs for some languages.
ifdef(`TARGET', `dnl
 .
 This package contains files for TARGET architecture, for use in cross-compile
 environment.
')`'dnl

Package: libgcc2-dbg`'LS
Architecture: ifdef(`TARGET',`all',`m68k')
Section: debug
Priority: extra
Depends: BASEDEP, libgcc2`'LS (= ${gcc:Version}), ${misc:Depends}
ifdef(`TARGET',`',ifdef(`MULTIARCH', `Multi-Arch: same
'))`'dnl
Description: GCC support library (debug symbols)`'ifdef(`TARGET)',` (TARGET)', `')
 Debug symbols for the GCC support library.
ifdef(`TARGET', `dnl
 .
 This package contains files for TARGET architecture, for use in cross-compile
 environment.
')`'dnl
')`'dnl libgcc

ifenabled(`lib4gcc',`
Package: libgcc4`'LS
Architecture: ifdef(`TARGET',`all',`hppa')
Section: ifdef(`TARGET',`devel',`libs')
Priority: ifdef(`TARGET',`extra',required)
Depends: ifdef(`STANDALONEJAVA',`gcj`'PV-base (>= ${gcj:Version})',`BASEDEP'), ${shlibs:Depends}, ${misc:Depends}
Description: GCC support library`'ifdef(`TARGET)',` (TARGET)', `')
 Shared version of the support library, a library of internal subroutines
 that GCC uses to overcome shortcomings of particular machines, or
 special needs for some languages.
ifdef(`TARGET', `dnl
 .
 This package contains files for TARGET architecture, for use in cross-compile
 environment.
')`'dnl

Package: libgcc4-dbg`'LS
Architecture: ifdef(`TARGET',`all',`hppa')
Section: debug
Priority: extra
Depends: BASEDEP, libgcc4`'LS (= ${gcc:Version}), ${misc:Depends}
Description: GCC support library (debug symbols)`'ifdef(`TARGET)',` (TARGET)', `')
 Debug symbols for the GCC support library.
ifdef(`TARGET', `dnl
 .
 This package contains files for TARGET architecture, for use in cross-compile
 environment.
')`'dnl
')`'dnl lib4gcc

ifenabled(`lib64gcc',`
Package: lib64gcc1`'LS
Architecture: ifdef(`TARGET',`all',`biarch64_archs')
Section: ifdef(`TARGET',`devel',`libs')
Priority: ifdef(`TARGET',`extra',PRI(optional))
Depends: BASEDEP, ${dep:libcbiarch}, ${misc:Depends}
ifdef(`TARGET',`Provides: lib64gcc1-TARGET-dcv1
',`')`'dnl
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

Package: lib64gcc1-dbg`'LS
Architecture: ifdef(`TARGET',`all',`biarch64_archs')
Section: debug
Priority: extra
Depends: BASEDEP, lib64gcc1`'LS (= ${gcc:EpochVersion}), ${misc:Depends}
Description: GCC support library (debug symbols)`'ifdef(`TARGET)',` (TARGET)', `')
 Debug symbols for the GCC support library.
ifdef(`TARGET', `dnl
 .
 This package contains files for TARGET architecture, for use in cross-compile
 environment.
')`'dnl
')`'dnl lib64gcc

ifenabled(`lib32gcc',`
Package: lib32gcc1`'LS
Architecture: ifdef(`TARGET',`all',`biarch32_archs')
Section: ifdef(`TARGET',`devel',`libs')
Priority: ifdef(`TARGET',`extra',PRI(optional))
Depends: BASEDEP, ${dep:libcbiarch}, ${misc:Depends}
Conflicts: ${confl:lib32}
ifdef(`TARGET',`Provides: lib32gcc1-TARGET-dcv1
',`')`'dnl
Description: GCC support library (32 bit Version)
 Shared version of the support library, a library of internal subroutines
 that GCC uses to overcome shortcomings of particular machines, or
 special needs for some languages.
ifdef(`TARGET', `dnl
 .
 This package contains files for TARGET architecture, for use in cross-compile
 environment.
')`'dnl

Package: lib32gcc1-dbg`'LS
Architecture: ifdef(`TARGET',`all',`biarch32_archs')
Section: debug
Priority: extra
Depends: BASEDEP, lib32gcc1`'LS (= ${gcc:EpochVersion}), ${misc:Depends}
Description: GCC support library (debug symbols)`'ifdef(`TARGET)',` (TARGET)', `')
 Debug symbols for the GCC support library.
ifdef(`TARGET', `dnl
 .
 This package contains files for TARGET architecture, for use in cross-compile
 environment.
')`'dnl
')`'dnl lib32gcc1

ifenabled(`libneongcc',`
Package: libgcc1-neon`'LS
Architecture: NEON_ARCHS
Section: libs
Priority: extra
Depends: BASEDEP, libc6-neon`'LS, ${shlibs:Depends}, ${misc:Depends}
Description: GCC support library [neon optimized]
 Shared version of the support library, a library of internal subroutines
 that GCC uses to overcome shortcomings of particular machines, or
 special needs for some languages.
 .
 This set of libraries is optimized to use a NEON coprocessor, and will
 be selected instead when running under systems which have one.
')`'dnl libneongcc1

ifenabled(`libn32gcc',`
Package: libn32gcc1`'LS
Architecture: ifdef(`TARGET',`all',`biarchn32_archs')
Section: ifdef(`TARGET',`devel',`libs')
Priority: ifdef(`TARGET',`extra',PRI(optional))
Depends: BASEDEP, ${dep:libcbiarch}, ${misc:Depends}
ifdef(`TARGET',`Provides: libn32gcc1-TARGET-dcv1
',`')`'dnl
Conflicts: libgcc`'GCC_SO`'LS (<= 1:3.3-0pre9)
Description: GCC support library`'ifdef(`TARGET)',` (TARGET)', `') (n32)
 Shared version of the support library, a library of internal subroutines
 that GCC uses to overcome shortcomings of particular machines, or
 special needs for some languages.
ifdef(`TARGET', `dnl
 .
 This package contains files for TARGET architecture, for use in cross-compile
 environment.
')`'dnl

Package: libn32gcc1-dbg`'LS
Architecture: ifdef(`TARGET',`all',`biarchn32_archs')
Section: debug
Priority: extra
Depends: BASEDEP, libn32gcc1`'LS (= ${gcc:EpochVersion}), ${misc:Depends}
Description: GCC support library (debug symbols)`'ifdef(`TARGET)',` (TARGET)', `')
 Debug symbols for the GCC support library.
ifdef(`TARGET', `dnl
 .
 This package contains files for TARGET architecture, for use in cross-compile
 environment.
')`'dnl
')`'dnl libn32gcc

ifdef(`TARGET', `', `
ifenabled(`libgmath',`
Package: libgccmath`'GCCMATH_SO`'LS
Architecture: i386
ifdef(`MULTIARCH', `Multi-Arch: same
')`'dnl
Section: libs
Priority: PRI(optional)
Depends: BASEDEP, ${shlibs:Depends}, ${misc:Depends}
Description: GCC math support library
 Support library for GCC.

Package: lib32gccmath`'GCCMATH_SO`'LS
Architecture: amd64
Section: libs
Priority: PRI(optional)
Depends: BASEDEP, ${shlibs:Depends}, ${misc:Depends}
Description: GCC math support library (32bit)
 Support library for GCC.

Package: lib64gccmath`'GCCMATH_SO`'LS
Architecture: i386
Section: libs
Priority: PRI(optional)
Depends: BASEDEP, ${shlibs:Depends}, ${misc:Depends}
Description: GCC math support library (64bit)
 Support library for GCC.
')`'dnl
')`'dnl native

ifenabled(`cdev',`
Package: gcc`'PV`'TS
Architecture: any
Section: devel
Priority: ifdef(`TARGET',`extra',`PRI(optional)')
Depends: BASEDEP, cpp`'PV`'TS (= ${gcc:Version}), binutils`'TS (>= ${binutils:Version}), ${dep:libgcc}, ${dep:libssp}, ${dep:libgomp}, ${dep:libunwinddev}, ${shlibs:Depends}, ${misc:Depends}
Recommends: ${dep:libcdev}
Suggests: ${gcc:multilib}, libmudflap`'MF_SO`'PV-dev`'LS (>= ${gcc:Version}), gcc`'PV-doc (>= ${gcc:SoftVersion}), gcc`'PV-locales (>= ${gcc:SoftVersion}), libgcc`'GCC_SO-dbg`'LS, libgomp`'GOMP_SO-dbg`'LS, libmudflap`'MF_SO-dbg`'LS, ${dep:libcloog}, ${dep:gold}
Provides: c-compiler`'TS
Description: The GNU C compiler`'ifdef(`TARGET)',` (cross compiler for TARGET architecture)', `')
 This is the GNU C compiler, a fairly portable optimizing compiler for C.
ifdef(`TARGET', `dnl
 .
 This package contains C cross-compiler for TARGET architecture.
')`'dnl

ifenabled(`multilib',`
Package: gcc`'PV-multilib`'TS
Architecture: ifdef(`TARGET',`any',MULTILIB_ARCHS)
Section: devel
Priority: ifdef(`TARGET',`extra',`PRI(optional)')
Depends: BASEDEP, gcc`'PV`'TS (= ${gcc:Version}), ${dep:libcbiarchdev}, ${dep:libgccbiarch}, ${dep:libsspbiarch}, ${dep:libgompbiarch}, ${shlibs:Depends}, ${misc:Depends}
Suggests: ${dep:libmudflapbiarch}
Description: The GNU C compiler (multilib files)`'ifdef(`TARGET)',` (cross compiler for TARGET architecture)', `')
 This is the GNU C compiler, a fairly portable optimizing compiler for C.
 .
 On architectures with multilib support, the package contains files
 and dependencies for the non-default multilib architecture(s).
')`'dnl multilib

ifenabled(`plugindev',`
Package: gcc`'PV-plugin-dev`'TS
Architecture: any
Section: devel
Priority: ifdef(`TARGET',`extra',`PRI(optional)')
Depends: BASEDEP, gcc`'PV`'TS (= ${gcc:Version}), libgmp3-dev, ${shlibs:Depends}, ${misc:Depends}
Description: Files for GNU GCC plugin development.
 This package contains (header) files for GNU GCC plugin development. It
 is only used for the development of GCC plugins, but not needed to run
 plugins.
')`'dnl plugindev
')`'dnl cdev

ifenabled(`cdev',`
Package: gcc`'PV-hppa64
Architecture: ifdef(`TARGET',`any',hppa)
Section: devel
Priority: PRI(optional)
Depends: BASEDEP, ${shlibs:Depends}, ${misc:Depends}
Conflicts: gcc-3.3-hppa64 (<= 1:3.3.4-5), gcc-3.4-hppa64 (<= 3.4.1-3)
Description: The GNU C compiler (cross compiler for hppa64)
 This is the GNU C compiler, a fairly portable optimizing compiler for C.

ifdef(`TARGET', `', `
Package: gcc`'PV-spu
Architecture: powerpc ppc64
Section: devel
Priority: PRI(optional)
Depends: BASEDEP, binutils-spu (>= 2.18.1~cvs20080103-3), newlib-spu, ${shlibs:Depends}, ${misc:Depends}
Provides: spu-gcc
Description: SPU cross-compiler (preprocessor and C compiler)
 GNU Compiler Collection for the Cell Broadband Engine SPU (preprocessor
 and C compiler).

Package: g++`'PV-spu
Architecture: powerpc ppc64
Section: devel
Priority: PRI(optional)
Depends: BASEDEP, gcc`'PV-spu (= ${gcc:Version}), ${shlibs:Depends}, ${misc:Depends}
Provides: spu-g++
Description: SPU cross-compiler (C++ compiler)
 GNU Compiler Collection for the Cell Broadband Engine SPU (C++ compiler).

Package: gfortran`'PV-spu
Architecture: powerpc ppc64
Section: devel
Priority: PRI(optional)
Depends: BASEDEP, gcc`'PV-spu (= ${gcc:Version}), ${shlibs:Depends}, ${misc:Depends}
Provides: spu-gfortran
Description: SPU cross-compiler (Fortran compiler)
 GNU Compiler Collection for the Cell Broadband Engine SPU (Fortran compiler).

')`'dnl native
')`'dnl cdev

ifenabled(`cdev',`
Package: cpp`'PV`'TS
Architecture: any
Section: ifdef(`TARGET',`devel',`interpreters')
Priority: ifdef(`TARGET',`extra',`PRI(optional)')
Depends: BASEDEP, ${shlibs:Depends}, ${misc:Depends}
Suggests: gcc`'PV-locales (>= ${gcc:SoftVersion})
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
ifenabled(`gfdldoc',`
Package: cpp`'PV-doc
Architecture: all
Section: doc
Priority: PRI(optional)
Depends: gcc`'PV-base (>= ${gcc:SoftVersion}), dpkg (>= 1.15.4) | install-info, ${misc:Depends}
Description: Documentation for the GNU C preprocessor (cpp)
 Documentation for the GNU C preprocessor in info `format'.
')`'dnl gfdldoc
')`'dnl native

ifdef(`TARGET', `', `
Package: gcc`'PV-locales
Architecture: all
Section: devel
Priority: PRI(optional)
Depends: SOFTBASEDEP, cpp`'PV (>= ${gcc:SoftVersion}), ${misc:Depends}
Recommends: gcc`'PV (>= ${gcc:SoftVersion})
Description: The GNU C compiler (native language support files)
 Native language support for GCC. Lets GCC speak your language,
 if translations are available.
 .
 Please do NOT submit bug reports in other languages than "C".
 Always reset your language settings to use the "C" locales.
')`'dnl native
')`'dnl cdev

ifenabled(`c++',`
ifenabled(`c++dev',`
Package: g++`'PV`'TS
Architecture: any
Section: devel
Priority: ifdef(`TARGET',`extra',`PRI(optional)')
Depends: BASEDEP, gcc`'PV`'TS (= ${gcc:Version}), libstdc++CXX_SO`'PV-dev`'LS (= ${gcc:Version}), ${shlibs:Depends}, ${misc:Depends}
Provides: c++-compiler`'TS`'ifdef(`TARGET)',`',`, c++abi2-dev')
Suggests: ${gxx:multilib}, gcc`'PV-doc (>= ${gcc:SoftVersion}), libstdc++CXX_SO`'PV-dbg`'LS
Description: The GNU C++ compiler`'ifdef(`TARGET)',` (cross compiler for TARGET architecture)', `')
 This is the GNU C++ compiler, a fairly portable optimizing compiler for C++.
ifdef(`TARGET', `dnl
 .
 This package contains C++ cross-compiler for TARGET architecture.
')`'dnl

ifenabled(`multilib',`
Package: g++`'PV-multilib`'TS
Architecture: ifdef(`TARGET',`any',MULTILIB_ARCHS)
Section: devel
Priority: ifdef(`TARGET',`extra',`PRI(optional)')
Depends: BASEDEP, g++`'PV`'TS (= ${gcc:Version}), gcc`'PV-multilib`'TS (= ${gcc:Version}), ${dep:libcxxbiarch}, ${shlibs:Depends}, ${misc:Depends}
Suggests: ${dep:libcxxbiarchdbg}
Description: The GNU C++ compiler (multilib files)`'ifdef(`TARGET)',` (cross compiler for TARGET architecture)', `')
 This is the GNU C++ compiler, a fairly portable optimizing compiler for C++.
 .
 On architectures with multilib support, the package contains files
 and dependencies for the non-default multilib architecture(s).
')`'dnl multilib
')`'dnl c++dev
')`'dnl c++

ifenabled(`mudflap',`
ifenabled(`libmudf',`
Package: libmudflap`'MF_SO`'LS
Architecture: ifdef(`TARGET',`all',`any')
ifdef(`MULTIARCH', `Multi-Arch: same
')`'dnl
Section: ifdef(`TARGET',`devel',`libs')
Priority: ifdef(`TARGET',`extra',`PRI(optional)')
Depends: BASEDEP, ${shlibs:Depends}, ${misc:Depends}
Description: GCC mudflap shared support libraries
 The libmudflap libraries are used by GCC for instrumenting pointer and array
 dereferencing operations.

Package: libmudflap`'MF_SO-dbg`'LS
Architecture: ifdef(`TARGET',`all',`any')
ifdef(`MULTIARCH', `Multi-Arch: same
')`'dnl
Section: debug
Priority: extra
Depends: BASEDEP, libmudflap`'MF_SO`'LS (= ${gcc:Version}), ${misc:Depends}
Description: GCC mudflap shared support libraries (debug symbols)
 The libmudflap libraries are used by GCC for instrumenting pointer and array
 dereferencing operations.

Package: lib32mudflap`'MF_SO`'LS
Architecture: ifdef(`TARGET',`all',`biarch32_archs')
Section: ifdef(`TARGET',`devel',`libs')
Priority: ifdef(`TARGET',`extra',`PRI(optional)')
Depends: BASEDEP, ${dep:libcbiarch}, ${shlibs:Depends}, ${misc:Depends}
Replaces: libmudflap0 (<< 4.1)
Conflicts: ${confl:lib32}
Description: GCC mudflap shared support libraries (32bit)
 The libmudflap libraries are used by GCC for instrumenting pointer and array
 dereferencing operations.

Package: lib32mudflap`'MF_SO-dbg`'LS
Architecture: ifdef(`TARGET',`all',`biarch32_archs')
Section: debug
Priority: extra
Depends: BASEDEP, lib32mudflap`'MF_SO`'LS (= ${gcc:Version}), ${misc:Depends}
Description: GCC mudflap shared support libraries (32 bit debug symbols)
 The libmudflap libraries are used by GCC for instrumenting pointer and array
 dereferencing operations.

Package: lib64mudflap`'MF_SO`'LS
Architecture: ifdef(`TARGET',`all',`biarch64_archs')
Section: ifdef(`TARGET',`devel',`libs')
Priority: ifdef(`TARGET',`extra',`PRI(optional)')
Depends: BASEDEP, ${dep:libcbiarch}, ${shlibs:Depends}, ${misc:Depends}
Replaces: libmudflap0 (<< 4.1)
Description: GCC mudflap shared support libraries (64bit)
 The libmudflap libraries are used by GCC for instrumenting pointer and array
 dereferencing operations.

Package: lib64mudflap`'MF_SO-dbg`'LS
Architecture: ifdef(`TARGET',`all',`biarch64_archs')
Section: debug
Priority: extra
Depends: BASEDEP, lib64mudflap`'MF_SO`'LS (= ${gcc:Version}), ${misc:Depends}
Description: GCC mudflap shared support libraries (64 bit debug symbols)
 The libmudflap libraries are used by GCC for instrumenting pointer and array
 dereferencing operations.

Package: libn32mudflap`'MF_SO`'LS
Architecture: ifdef(`TARGET',`all',`biarchn32_archs')
Section: ifdef(`TARGET',`devel',`libs')
Priority: ifdef(`TARGET',`extra',`PRI(optional)')
Depends: BASEDEP, ${dep:libcbiarch}, ${shlibs:Depends}, ${misc:Depends}
Replaces: libmudflap0 (<< 4.1)
Description: GCC mudflap shared support libraries (n32)
 The libmudflap libraries are used by GCC for instrumenting pointer and array
 dereferencing operations.

Package: libn32mudflap`'MF_SO-dbg`'LS
Architecture: ifdef(`TARGET',`all',`biarchn32_archs')
Section: debug
Priority: extra
Depends: BASEDEP, libn32mudflap`'MF_SO`'LS (= ${gcc:Version}), ${misc:Depends}
Description: GCC mudflap shared support libraries (n32 debug symbols)
 The libmudflap libraries are used by GCC for instrumenting pointer and array
 dereferencing operations.
')`'dnl libmudf

Package: libmudflap`'MF_SO`'PV-dev`'LS
Architecture: ifdef(`TARGET',`all',`any')
Section: ifdef(`TARGET',`devel',`libdevel')
Priority: ifdef(`TARGET',`extra',`PRI(optional)')
Depends: BASEDEP, libmudflap`'MF_SO`'LS (>= ${gcc:Version}), ${dep:libcdev}, ${shlibs:Depends}, ${misc:Depends}
Suggests: ${sug:libmudflapdev}
Conflicts: libmudflap0-dev
Description: GCC mudflap support libraries (development files)
 The libmudflap libraries are used by GCC for instrumenting pointer and array
 dereferencing operations.
 .
 This package contains the headers and the static libraries.
')`'dnl mudflap

ifdef(`TARGET', `', `
ifenabled(`ssp',`
Package: libssp`'SSP_SO`'LS
Architecture: any
ifdef(`MULTIARCH', `Multi-Arch: same
')`'dnl
Section: libs
Priority: PRI(optional)
Depends: BASEDEP, ${shlibs:Depends}, ${misc:Depends}
Description: GCC stack smashing protection library
 GCC can now emit code for protecting applications from stack-smashing attacks.
 The protection is realized by buffer overflow detection and reordering of
 stack variables to avoid pointer corruption.

Package: lib32ssp`'SSP_SO`'LS
Architecture: biarch32_archs
Section: libs
Priority: PRI(optional)
Depends: BASEDEP, ${dep:libcbiarch}, ${shlibs:Depends}, ${misc:Depends}
Replaces: libssp0 (<< 4.1)
Conflicts: ${confl:lib32}
Description: GCC stack smashing protection library (32bit)
 GCC can now emit code for protecting applications from stack-smashing attacks.
 The protection is realized by buffer overflow detection and reordering of
 stack variables to avoid pointer corruption.

Package: lib64ssp`'SSP_SO`'LS
Architecture: biarch64_archs
Section: libs
Priority: PRI(optional)
Depends: BASEDEP, ${dep:libcbiarch}, ${shlibs:Depends}, ${misc:Depends}
Replaces: libssp0 (<< 4.1)
Description: GCC stack smashing protection library (64bit)
 GCC can now emit code for protecting applications from stack-smashing attacks.
 The protection is realized by buffer overflow detection and reordering of
 stack variables to avoid pointer corruption.

Package: libn32ssp`'SSP_SO`'LS
Architecture: biarchn32_archs
Section: libs
Priority: PRI(optional)
Depends: BASEDEP, ${dep:libcbiarch}, ${shlibs:Depends}, ${misc:Depends}
Replaces: libssp0 (<< 4.1)
Description: GCC stack smashing protection library (n32)
 GCC can now emit code for protecting applications from stack-smashing attacks.
 The protection is realized by buffer overflow detection and reordering of
 stack variables to avoid pointer corruption.
')`'dnl
')`'dnl native

ifenabled(`libgomp',`
Package: libgomp`'GOMP_SO`'LS
Section: ifdef(`TARGET',`devel',`libs')
Architecture: ifdef(`TARGET',`all',`any')
ifdef(`TARGET',`',ifdef(`MULTIARCH', `Multi-Arch: same
'))`'dnl
Priority: ifdef(`TARGET',`extra',`PRI(optional)')
Depends: BASEDEP, ${shlibs:Depends}, ${misc:Depends}
Description: GCC OpenMP (GOMP) support library
 GOMP is an implementation of OpenMP for the C, C++, and Fortran 95 compilers
 in the GNU Compiler Collection.

Package: libgomp`'GOMP_SO-dbg`'LS
Architecture: ifdef(`TARGET',`all',`any')
Section: debug
Priority: extra
Depends: BASEDEP, libgomp`'GOMP_SO`'LS (= ${gcc:Version}), ${misc:Depends}
ifdef(`TARGET',`',ifdef(`MULTIARCH', `Multi-Arch: same
'))`'dnl
Description: GCC OpenMP (GOMP) support library (debug symbols)
 GOMP is an implementation of OpenMP for the C, C++, and Fortran 95 compilers
 in the GNU Compiler Collection.

Package: lib32gomp`'GOMP_SO`'LS
Section: ifdef(`TARGET',`devel',`libs')
Architecture: ifdef(`TARGET',`all',`biarch32_archs')
Priority: ifdef(`TARGET',`extra',`PRI(optional)')
Depends: BASEDEP, ${dep:libcbiarch}, ${shlibs:Depends}, ${misc:Depends}
Conflicts: ${confl:lib32}
Description: GCC OpenMP (GOMP) support library (32bit)
 GOMP is an implementation of OpenMP for the C, C++, and Fortran 95 compilers
 in the GNU Compiler Collection.

Package: lib32gomp`'GOMP_SO-dbg`'LS
Architecture: ifdef(`TARGET',`all',`biarch32_archs')
Section: debug
Priority: extra
Depends: BASEDEP, lib32gomp`'GOMP_SO`'LS (= ${gcc:Version}), ${misc:Depends}
Description: GCC OpenMP (GOMP) support library (32 bit debug symbols)
 GOMP is an implementation of OpenMP for the C, C++, and Fortran 95 compilers
 in the GNU Compiler Collection.

Package: lib64gomp`'GOMP_SO`'LS
Section: ifdef(`TARGET',`devel',`libs')
Architecture: ifdef(`TARGET',`all',`biarch64_archs')
Priority: ifdef(`TARGET',`extra',`PRI(optional)')
Depends: BASEDEP, ${dep:libcbiarch}, ${shlibs:Depends}, ${misc:Depends}
Description: GCC OpenMP (GOMP) support library (64bit)
 GOMP is an implementation of OpenMP for the C, C++, and Fortran 95 compilers
 in the GNU Compiler Collection.

Package: lib64gomp`'GOMP_SO-dbg`'LS
Architecture: ifdef(`TARGET',`all',`biarch64_archs')
Section: debug
Priority: extra
Depends: BASEDEP, lib64gomp`'GOMP_SO`'LS (= ${gcc:Version}), ${misc:Depends}
Description: GCC OpenMP (GOMP) support library (64bit debug symbols)
 GOMP is an implementation of OpenMP for the C, C++, and Fortran 95 compilers
 in the GNU Compiler Collection.

Package: libn32gomp`'GOMP_SO`'LS
Section: ifdef(`TARGET',`devel',`libs')
Architecture: ifdef(`TARGET',`all',`biarchn32_archs')
Priority: ifdef(`TARGET',`extra',`PRI(optional)')
Depends: BASEDEP, ${dep:libcbiarch}, ${shlibs:Depends}, ${misc:Depends}
Description: GCC OpenMP (GOMP) support library (n32)
 GOMP is an implementation of OpenMP for the C, C++, and Fortran 95 compilers
 in the GNU Compiler Collection.

Package: libn32gomp`'GOMP_SO-dbg`'LS
Architecture: ifdef(`TARGET',`all',`biarchn32_archs')
Section: debug
Priority: extra
Depends: BASEDEP, libn32gomp`'GOMP_SO`'LS (= ${gcc:Version}), ${misc:Depends}
Description: GCC OpenMP (GOMP) support library (n32 debug symbols)
 GOMP is an implementation of OpenMP for the C, C++, and Fortran 95 compilers

ifenabled(`libneongomp',`
Package: libgomp`'GOMP_SO-neon`'LS
Architecture: NEON_ARCHS
Section: libs
Priority: extra
Depends: BASEDEP, libc6-neon`'LS, ${shlibs:Depends}, ${misc:Depends}
Description: GCC OpenMP (GOMP) support library [neon optimized]
 GOMP is an implementation of OpenMP for the C, C++, and Fortran 95 compilers
 in the GNU Compiler Collection.
 .
 This set of libraries is optimized to use a NEON coprocessor, and will
 be selected instead when running under systems which have one.
')`'dnl libneongomp
')`'dnl libgomp

ifenabled(`proto',`
Package: protoize
Architecture: any
Priority: PRI(optional)
Depends: BASEDEP, gcc`'PV (>= ${gcc:Version}), ${shlibs:Depends}, ${misc:Depends}
Description: Create/remove ANSI prototypes from C code
 "protoize" can be used to add prototypes to a program, thus converting
 the program to ANSI C in one respect.  The companion program "unprotoize"
 does the reverse: it removes argument types from any prototypes
 that are found.
')`'dnl proto

ifenabled(`objpp',`
ifenabled(`objppdev',`
Package: gobjc++`'PV`'TS
Architecture: any
Priority: ifdef(`TARGET',`extra',`PRI(optional)')
Depends: BASEDEP, gobjc`'PV`'TS (= ${gcc:Version}), g++`'PV`'TS (= ${gcc:Version}), ${shlibs:Depends}, libobjc`'OBJC_SO`'LS (>= ${gcc:Version}), ${misc:Depends}
Suggests: ${gobjcxx:multilib}, gcc`'PV-doc (>= ${gcc:SoftVersion})
Provides: objc++-compiler`'TS
Description: The GNU Objective-C++ compiler
 This is the GNU Objective-C++ compiler, which compiles
 Objective-C++ on platforms supported by the gcc compiler. It uses the
 gcc backend to generate optimized code.
')`'dnl obcppdev

ifenabled(`multilib',`
Package: gobjc++`'PV-multilib`'TS
Architecture: ifdef(`TARGET',`any',MULTILIB_ARCHS)
Section: devel
Priority: ifdef(`TARGET',`extra',`PRI(optional)')
Depends: BASEDEP, gobjc++`'PV`'TS (= ${gcc:Version}), g++`'PV-multilib`'TS (= ${gcc:Version}), gobjc`'PV-multilib`'TS (= ${gcc:Version}), ${shlibs:Depends}, ${misc:Depends}
Description: The GNU Objective-C++ compiler (multilib files)
 This is the GNU Objective-C++ compiler, which compiles Objective-C++ on
 platforms supported by the gcc compiler.
 .
 On architectures with multilib support, the package contains files
 and dependencies for the non-default multilib architecture(s).
')`'dnl multilib
')`'dnl obcpp

ifenabled(`objc',`
ifenabled(`objcdev',`
Package: gobjc`'PV`'TS
Architecture: any
Priority: ifdef(`TARGET',`extra',`PRI(optional)')
Depends: BASEDEP, gcc`'PV`'TS (= ${gcc:Version}), ${dep:libcdev}, ${shlibs:Depends}, libobjc`'OBJC_SO`'LS (>= ${gcc:Version}), ${misc:Depends}
Suggests: ${gobjc:multilib}, gcc`'PV-doc (>= ${gcc:SoftVersion}), libobjc`'OBJC_SO-dbg`'LS
Provides: objc-compiler`'TS
ifdef(`__sparc__',`Conflicts: gcc`'PV-sparc64', `dnl')
Description: The GNU Objective-C compiler
 This is the GNU Objective-C compiler, which compiles
 Objective-C on platforms supported by the gcc compiler. It uses the
 gcc backend to generate optimized code.

ifenabled(`multilib',`
Package: gobjc`'PV-multilib`'TS
Architecture: ifdef(`TARGET',`any',MULTILIB_ARCHS)
Section: devel
Priority: ifdef(`TARGET',`extra',`PRI(optional)')
Depends: BASEDEP, gobjc`'PV`'TS (= ${gcc:Version}), gcc`'PV-multilib`'TS (= ${gcc:Version}), ${dep:libobjcbiarch}, ${shlibs:Depends}, ${misc:Depends}
Description: The GNU Objective-C compiler (multilib files)`'ifdef(`TARGET)',` (cross compiler for TARGET architecture)', `')
 This is the GNU Objective-C compiler, which compiles Objective-C on platforms
 supported by the gcc compiler.
 .
 On architectures with multilib support, the package contains files
 and dependencies for the non-default multilib architecture(s).
')`'dnl multilib
')`'dnl objcdev

ifenabled(`libobjc',`
Package: libobjc`'OBJC_SO`'LS
Section: ifdef(`TARGET',`devel',`libs')
Architecture: ifdef(`TARGET',`all',`any')
ifdef(`TARGET',`',ifdef(`MULTIARCH', `Multi-Arch: same
'))`'dnl
Priority: ifdef(`TARGET',`extra',`PRI(optional)')
Depends: BASEDEP, ${shlibs:Depends}, ${misc:Depends}
Description: Runtime library for GNU Objective-C applications
 Library needed for GNU ObjC applications linked against the shared library.

Package: libobjc`'OBJC_SO-dbg`'LS
Section: debug
Architecture: ifdef(`TARGET',`all',`any')
ifdef(`TARGET',`',ifdef(`MULTIARCH', `Multi-Arch: same
'))`'dnl
Priority: extra
Depends: BASEDEP, libobjc`'OBJC_SO`'LS (= ${gcc:Version}), libgcc`'GCC_SO-dbg`'LS, ${misc:Depends}
Description: Runtime library for GNU Objective-C applications (debug symbols)
 Library needed for GNU ObjC applications linked against the shared library.
')`'dnl libobjc

ifenabled(`lib64objc',`
Package: lib64objc`'OBJC_SO`'LS
Section: ifdef(`TARGET',`devel',`libs')
Architecture: ifdef(`TARGET',`all',`biarch64_archs')
Priority: ifdef(`TARGET',`extra',`PRI(optional)')
Depends: BASEDEP, ${dep:libcbiarch}, ${shlibs:Depends}, ${misc:Depends}
Description: Runtime library for GNU Objective-C applications (64bit)
 Library needed for GNU ObjC applications linked against the shared library.

Package: lib64objc`'OBJC_SO-dbg`'LS
Section: debug
Architecture: ifdef(`TARGET',`all',`biarch64_archs')
Priority: extra
Depends: BASEDEP, lib64objc`'OBJC_SO`'LS (= ${gcc:Version}), lib64gcc`'GCC_SO-dbg`'LS, ${misc:Depends}
Description: Runtime library for GNU Objective-C applications (64 bit debug symbols)
 Library needed for GNU ObjC applications linked against the shared library.
')`'dnl lib64objc

ifenabled(`lib32objc',`
Package: lib32objc`'OBJC_SO`'LS
Section: ifdef(`TARGET',`devel',`libs')
Architecture: ifdef(`TARGET',`all',`biarch32_archs')
Priority: ifdef(`TARGET',`extra',`PRI(optional)')
Depends: gcc`'PV-base (>= ${gcc:Version}), ${dep:libcbiarch}, ${shlibs:Depends}, ${misc:Depends}
Conflicts: ${confl:lib32}
Description: Runtime library for GNU Objective-C applications (32bit)
 Library needed for GNU ObjC applications linked against the shared library.

Package: lib32objc`'OBJC_SO-dbg`'LS
Section: debug
Architecture: ifdef(`TARGET',`all',`biarch32_archs')
Priority: extra
Depends: BASEDEP, lib32objc`'OBJC_SO`'LS (= ${gcc:Version}), lib32gcc`'GCC_SO-dbg`'LS, ${misc:Depends}
Description: Runtime library for GNU Objective-C applications (32 bit debug symbols)
 Library needed for GNU ObjC applications linked against the shared library.
')`'dnl lib32objc

ifenabled(`libn32objc',`
Package: libn32objc`'OBJC_SO`'LS
Section: ifdef(`TARGET',`devel',`libs')
Architecture: ifdef(`TARGET',`all',`biarchn32_archs')
Priority: ifdef(`TARGET',`extra',`PRI(optional)')
Depends: gcc`'PV-base (>= ${gcc:Version}), ${dep:libcbiarch}, ${shlibs:Depends}, ${misc:Depends}
Description: Runtime library for GNU Objective-C applications (n32)
 Library needed for GNU ObjC applications linked against the shared library.

Package: libn32objc`'OBJC_SO-dbg`'LS
Section: debug
Architecture: ifdef(`TARGET',`all',`biarchn32_archs')
Priority: extra
Depends: BASEDEP, libn32objc`'OBJC_SO`'LS (= ${gcc:Version}), libn32gcc`'GCC_SO-dbg`'LS, ${misc:Depends}
Description: Runtime library for GNU Objective-C applications (n32 debug symbols)
 Library needed for GNU ObjC applications linked against the shared library.
')`'dnl libn32objc

ifenabled(`libneonobjc',`
Package: libobjc`'OBJC_SO-neon`'LS
Section: libs
Architecture: NEON_ARCHS
Priority: PRI(optional)
Depends: BASEDEP, libc6-neon`'LS, ${shlibs:Depends}, ${misc:Depends}
Description: Runtime library for GNU Objective-C applications  [NEON version]
 Library needed for GNU ObjC applications linked against the shared library.
 .
 This set of libraries is optimized to use a NEON coprocessor, and will
 be selected instead when running under systems which have one.
')`'dnl libneonobjc
')`'dnl objc

ifenabled(`fortran',`
ifenabled(`fdev',`
Package: gfortran`'PV`'TS
Architecture: any
Priority: ifdef(`TARGET',`extra',`PRI(optional)')
Depends: BASEDEP, gcc`'PV`'TS (= ${gcc:Version}), libgfortran`'FORTRAN_SO`'LS (>= ${gcc:Version}), ${dep:libcdev}, ${shlibs:Depends}, ${misc:Depends}
Provides: fortran95-compiler
Suggests: ${gfortran:multilib}, gfortran`'PV-doc, libgfortran`'FORTRAN_SO-dbg`'LS
Replaces: libgfortran`'FORTRAN_SO-dev
Description: The GNU Fortran 95 compiler
 This is the GNU Fortran compiler, which compiles
 Fortran 95 on platforms supported by the gcc compiler. It uses the
 gcc backend to generate optimized code.

ifenabled(`multilib',`
Package: gfortran`'PV-multilib`'TS
Architecture: ifdef(`TARGET',`any',MULTILIB_ARCHS)
Section: devel
Priority: ifdef(`TARGET',`extra',`PRI(optional)')
Depends: BASEDEP, gfortran`'PV`'TS (= ${gcc:Version}), gcc`'PV-multilib`'TS (= ${gcc:Version}), ${dep:libgfortranbiarch}, ${shlibs:Depends}, ${misc:Depends}
Description: The GNU Fortran 95 compiler (multilib files)`'ifdef(`TARGET)',` (cross compiler for TARGET architecture)', `')
 This is the GNU Fortran compiler, which compiles Fortran 95 on platforms
 supported by the gcc compiler.
 .
 On architectures with multilib support, the package contains files
 and dependencies for the non-default multilib architecture(s).
')`'dnl multilib

ifenabled(`gfdldoc',`
Package: gfortran`'PV-doc
Architecture: all
Section: doc
Priority: PRI(optional)
Depends: gcc`'PV-base (>= ${gcc:SoftVersion}), dpkg (>= 1.15.4) | install-info, ${misc:Depends}
Description: Documentation for the GNU Fortran compiler (gfortran)
 Documentation for the GNU Fortran 95 compiler in info `format'.
')`'dnl gfdldoc
')`'dnl fdev

ifenabled(`libgfortran',`
Package: libgfortran`'FORTRAN_SO`'LS
Section: ifdef(`TARGET',`devel',`libs')
Architecture: ifdef(`TARGET',`all',`any')
ifdef(`TARGET',`',ifdef(`MULTIARCH', `Multi-Arch: same
'))`'dnl
Priority: ifdef(`TARGET',`extra',PRI(optional))
Depends: BASEDEP, ${shlibs:Depends}, ${misc:Depends}
Description: Runtime library for GNU Fortran applications
 Library needed for GNU Fortran applications linked against the
 shared library.

Package: libgfortran`'FORTRAN_SO-dbg`'LS
Section: debug
Architecture: ifdef(`TARGET',`all',`any')
ifdef(`TARGET',`',ifdef(`MULTIARCH', `Multi-Arch: same
'))`'dnl
Priority: extra
Depends: BASEDEP, libgfortran`'FORTRAN_SO`'LS (= ${gcc:Version}), ${misc:Depends}
Description: Runtime library for GNU Fortran applications (debug symbols)
 Library needed for GNU Fortran applications linked against the
 shared library.
')`'dnl libgfortran

ifenabled(`lib64gfortran',`
Package: lib64gfortran`'FORTRAN_SO`'LS
Section: ifdef(`TARGET',`devel',`libs')
Architecture: ifdef(`TARGET',`all',`biarch64_archs')
Priority: ifdef(`TARGET',`extra',PRI(optional))
Depends: BASEDEP, ${dep:libcbiarch}, ${shlibs:Depends}, ${misc:Depends}
Description: Runtime library for GNU Fortran applications (64bit)
 Library needed for GNU Fortran applications linked against the
 shared library.

Package: lib64gfortran`'FORTRAN_SO-dbg`'LS
Section: debug
Architecture: ifdef(`TARGET',`all',`biarch64_archs')
Priority: extra
Depends: BASEDEP, lib64gfortran`'FORTRAN_SO`'LS (= ${gcc:Version}), ${misc:Depends}
Description: Runtime library for GNU Fortran applications (64bit debug symbols)
 Library needed for GNU Fortran applications linked against the
 shared library.
')`'dnl lib64gfortran

ifenabled(`lib32gfortran',`
Package: lib32gfortran`'FORTRAN_SO`'LS
Section: ifdef(`TARGET',`devel',`libs')
Architecture: ifdef(`TARGET',`all',`biarch32_archs')
Priority: ifdef(`TARGET',`extra',PRI(optional))
Depends: BASEDEP, ${dep:libcbiarch}, ${shlibs:Depends}, ${misc:Depends}
Conflicts: ${confl:lib32}
Description: Runtime library for GNU Fortran applications (32bit)
 Library needed for GNU Fortran applications linked against the
 shared library.

Package: lib32gfortran`'FORTRAN_SO-dbg`'LS
Section: debug
Architecture: ifdef(`TARGET',`all',`biarch32_archs')
Priority: extra
Depends: BASEDEP, lib32gfortran`'FORTRAN_SO`'LS (= ${gcc:Version}), ${misc:Depends}
Description: Runtime library for GNU Fortran applications (32 bit debug symbols)
 Library needed for GNU Fortran applications linked against the
 shared library.
')`'dnl lib32gfortran

ifenabled(`libn32gfortran',`
Package: libn32gfortran`'FORTRAN_SO`'LS
Section: ifdef(`TARGET',`devel',`libs')
Architecture: ifdef(`TARGET',`all',`biarchn32_archs')
Priority: ifdef(`TARGET',`extra',PRI(optional))
Depends: BASEDEP, ${dep:libcbiarch}, ${shlibs:Depends}, ${misc:Depends}
Description: Runtime library for GNU Fortran applications (n32)
 Library needed for GNU Fortran applications linked against the
 shared library.

Package: libn32gfortran`'FORTRAN_SO-dbg`'LS
Section: debug
Architecture: ifdef(`TARGET',`all',`biarchn32_archs')
Priority: extra
Depends: BASEDEP, libn32gfortran`'FORTRAN_SO`'LS (= ${gcc:Version}), ${misc:Depends}
Description: Runtime library for GNU Fortran applications (n32 debug symbols)
 Library needed for GNU Fortran applications linked against the
 shared library.
')`'dnl libn32gfortran

ifenabled(`libneongfortran',`
Package: libgfortran`'FORTRAN_SO-neon`'LS
Section: libs
Architecture: NEON_ARCHS
ifdef(`MULTIARCH', `Multi-Arch: same
')`'dnl
Priority: extra
Depends: BASEDEP, libgcc1-neon`'LS, ${shlibs:Depends}, ${misc:Depends}
Description: Runtime library for GNU Fortran applications [NEON version]
 Library needed for GNU Fortran applications linked against the
 shared library.
 .
 This set of libraries is optimized to use a NEON coprocessor, and will
 be selected instead when running under systems which have one.
')`'dnl libneongfortran
')`'dnl fortran

ifenabled(`java',`
ifenabled(`gcj',`
Package: gcj`'PV-jdk
Section: java
Architecture: any
Priority: PRI(optional)
Depends: gcj`'PV-base (= ${gcj:Version}), ${dep:gcj}, ${dep:libcdev}, gcj`'PV-jre (= ${gcj:Version}), libgcj`'GCJ_SO-dev (= ${gcj:Version}), gcj`'PV-jre-lib (>= ${gcj:SoftVersion}), ${dep:ecj}, fastjar, libgcj-bc, java-common, libantlr-java, ${shlibs:Depends}, dpkg (>= 1.15.4) | install-info, ${misc:Depends}
Recommends: libecj-java-gcj
Suggests: gcj`'PV-source (>= ${gcj:SoftVersion}), libgcj`'GCJ_SO-dbg
Provides: java-compiler, java-sdk, java2-sdk, java5-sdk
Conflicts: gcj-4.4, cpp-4.1 (<< 4.1.1), gcc-4.1 (<< 4.1.1)
Replaces: libgcj10 (<< 4.4.2-8)
Description: gcj and classpath development tools for Java(TM)
 GCJ is a front end to the GCC compiler which can natively compile both
 Java(tm) source and bytecode files. The compiler can also generate class
 files. Other java development tools from classpath are included in this
 package.
 .
 The package contains as well a collection of wrapper scripts and symlinks.
 It is meant to provide a Java-SDK-like interface to the GCJ tool set.
')`'dnl gcj

ifenabled(`libgcj',`
ifenabled(`libgcjcommon',`
Package: libgcj-common
Section: java
Architecture: all
Priority: PRI(optional)
Depends: gcj`'PV-base (>= ${gcj:SoftVersion}), ${misc:Depends}
Conflicts: classpath (<= 0.04-4)
Replaces: java-gcj-compat (<< 1.0.65-3), java-gcj-compat-dev (<< 1.0.65-3)
Description: Java runtime library (common files)
 This package contains files shared by classpath and libgcj libraries.
')`'dnl libgcjcommon

Package: gcj`'PV-jre-headless
Priority: optional
Section: java
Architecture: any
Depends: gcj`'PV-base (= ${gcj:Version}), libgcj`'LIBGCJ_EXT (= ${gcj:Version}), ${dep:prctl}, ${shlibs:Depends}, ${misc:Depends}
Suggests: fastjar, gcj`'PV-jdk (= ${gcj:Version}), libgcj`'LIBGCJ_EXT-awt (= ${gcj:Version})
Conflicts: gij-4.4, java-gcj-compat (<< 1.0.76-4)
Provides: java5-runtime-headless, java2-runtime-headless, java1-runtime-headless, java-runtime-headless
Description: Java runtime environment using GIJ/classpath (headless version)
 GIJ is a Java bytecode interpreter, not limited to interpreting bytecode.
 It includes a class loader which can dynamically load shared objects, so
 it is possible to give it the name of a class which has been compiled and
 put into a shared library on the class path.
 .
 The package contains as well a collection of wrapper scripts and symlinks.
 It is meant to provide a Java-RTE-like interface to the GIJ/GCJ tool set,
 limited to the headless tools and libraries.

Package: gcj`'PV-jre
Section: java
Architecture: any
Priority: PRI(optional)
Depends: gcj`'PV-base (= ${gcj:Version}), gcj`'PV-jre-headless (= ${gcj:Version}), libgcj`'LIBGCJ_EXT-awt (= ${gcj:Version}), ${shlibs:Depends}, ${misc:Depends}
Provides: java5-runtime, java2-runtime, java1-runtime, java-runtime
Description: Java runtime environment using GIJ/classpath
 GIJ is a Java bytecode interpreter, not limited to interpreting bytecode.
 It includes a class loader which can dynamically load shared objects, so
 it is possible to give it the name of a class which has been compiled and
 put into a shared library on the class path.
 .
 The package contains as well a collection of wrapper scripts and symlinks.
 It is meant to provide a Java-RTE-like interface to the GIJ/GCJ tool set.

Package: libgcj`'LIBGCJ_EXT
Section: libs
Architecture: any
Priority: PRI(optional)
Depends: gcj`'PV-base (>= ${gcj:Version}), libgcj-common (>= 1:4.1.1-21), ${shlibs:Depends}, ${misc:Depends}
Recommends: gcj`'PV-jre-lib (>= ${gcj:SoftVersion})
Suggests: libgcj`'GCJ_SO-dbg, libgcj`'LIBGCJ_EXT-awt (= ${gcj:Version})
Replaces: gij-4.4 (<< 4.4.0-1)
Description: Java runtime library for use with gcj
 This is the runtime that goes along with the gcj front end to
 gcc. libgcj includes parts of the Java Class Libraries, plus glue to
 connect the libraries to the compiler and the underlying OS.
 .
 To show file names and line numbers in stack traces, the packages
 libgcj`'GCJ_SO-dbg and binutils are required.

Package: gcj`'PV-jre-lib
Section: java
Architecture: all
Priority: PRI(optional)
Depends: gcj`'PV-base (>= ${gcj:SoftVersion}), libgcj`'LIBGCJ_EXT (>= ${gcj:SoftVersion}), ${misc:Depends}
Description: Java runtime library for use with gcj (jar files)
 This is the jar file that goes along with the gcj front end to gcc.

ifenabled(`gcjbc',`
Package: libgcj-bc
Section: java
Architecture: any
Priority: PRI(optional)
Depends: gcj`'PV-base (>= ${gcj:Version}), libgcj`'LIBGCJ_EXT (>= ${gcj:Version}), ${misc:Depends}
Description: Link time only library for use with gcj
 A fake library that is used at link time only.  It ensures that
 binaries built with the BC-ABI link against a constant SONAME.
 This way, BC-ABI binaries continue to work if the SONAME underlying
 libgcj.so changes.
')`'dnl gcjbc

Package: libgcj`'LIBGCJ_EXT-awt
Section: libs
Architecture: any
Priority: PRI(optional)
Depends: gcj`'PV-base (>= ${gcj:Version}), libgcj`'LIBGCJ_EXT (= ${gcj:Version}), ${shlibs:Depends}, ${misc:Depends}
Suggests: ${pkg:gcjqt}
Description: AWT peer runtime libraries for use with gcj
 These are runtime libraries holding the AWT peer implementations
 for libgcj (currently the GTK+ based peer library is required, the
 QT bases library is not built).

ifenabled(`gtkpeer',`
Package: libgcj`'GCJ_SO-awt-gtk
Section: libs
Architecture: any
Priority: PRI(optional)
Depends: gcj`'PV-base (= ${gcj:Version}), libgcj`'LIBGCJ_EXT-awt (= ${gcj:Version}), ${shlibs:Depends}, ${misc:Depends}
Description: AWT GTK+ peer runtime library for use with libgcj
 This is the runtime library holding the GTK+ based AWT peer
 implementation for libgcj.
')`'dnl gtkpeer

ifenabled(`qtpeer',`
Package: libgcj`'GCJ_SO-awt-qt
Section: libs
Architecture: any
Priority: PRI(optional)
Depends: gcj`'PV-base (= ${gcj:Version}), libgcj`'LIBGCJ_EXT-awt (= ${gcj:Version}), ${shlibs:Depends}, ${misc:Depends}
Description: AWT QT peer runtime library for use with libgcj
 This is the runtime library holding the QT based AWT peer
 implementation for libgcj.
')`'dnl qtpeer
')`'dnl libgcj

ifenabled(`libgcjdev',`
Package: libgcj`'GCJ_SO-dev
Section: libdevel
Architecture: any
Priority: PRI(optional)
Depends: gcj`'PV-base (= ${gcj:Version}), gcj`'PV-jdk (= ${gcj:Version}), gcj`'PV-jre-lib (>= ${gcj:SoftVersion}), libgcj`'LIBGCJ_EXT-awt (= ${gcj:Version}), libgcj-bc, ${pkg:gcjgtk}, ${pkg:gcjqt}, zlib1g-dev, ${shlibs:Depends}, ${misc:Depends}
Suggests: libgcj-doc
Description: Java development headers for use with gcj
 These are the development headers that go along with the gcj front end
 to gcc. libgcj includes parts of the Java Class Libraries, plus glue
 to connect the libraries to the compiler and the underlying OS.

Package: libgcj`'GCJ_SO-dbg
Section: debug
Architecture: any
Priority: extra
Depends: gcj`'PV-base (= ${gcj:Version}), libgcj`'LIBGCJ_EXT (= ${gcj:Version}), ${misc:Depends}
Recommends: binutils, libc6-dbg | libc-dbg
Description: Debugging symbols for libraries provided in libgcj`'GCJ_SO-dev
 The package provides debugging symbols for the libraries provided
 in libgcj`'GCJ_SO-dev.
 .
 binutils is required to show file names and line numbers in stack traces.

Package: gcj`'PV-source
Section: java
Architecture: all
Priority: PRI(optional)
Depends: gcj`'PV-base (>= ${gcj:SoftVersion}), gcj`'PV-jdk (>= ${gcj:SoftVersion}), ${misc:Depends}
Description: GCJ java sources for use in IDEs like eclipse and netbeans
 These are the java source files packaged as a zip file for use in development
 environments like eclipse and netbeans.

ifenabled(`gcjdoc',`
Package: libgcj-doc
Section: doc
Architecture: all
Priority: PRI(optional)
Depends: gcj`'PV-base (>= ${gcj:SoftVersion}), ${misc:Depends}
Enhances: libgcj`'GCJ_SO-dev
Provides: classpath-doc
Description: libgcj API documentation and example programs
 Autogenerated documentation describing the API of the libgcj library.
 Sources and precompiled example programs from the classpath library.
')`'dnl gcjdoc
')`'dnl libgcjdev
')`'dnl java

ifenabled(`c++',`
ifenabled(`libcxx',`
Package: libstdc++CXX_SO`'LS
Architecture: ifdef(`TARGET',`all',`any')
Section: ifdef(`TARGET',`devel',`libs')
Priority: ifdef(`TARGET',`extra',PRI(required))
Depends: BASEDEP, ${shlibs:Depends}, ${misc:Depends}
ifdef(`TARGET',`Provides: libstdc++CXX_SO-TARGET-dcv1
',ifdef(`MULTIARCH', `Multi-Arch: same
'))`'dnl
Conflicts: scim (<< 1.4.2-1)
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
')`'dnl libcxx

ifenabled(`lib32cxx',`
Package: lib32stdc++CXX_SO`'LS
Architecture: ifdef(`TARGET',`all',`biarch32_archs')
Section: ifdef(`TARGET',`devel',`libs')
Priority: ifdef(`TARGET',`extra',PRI(optional))
Depends: BASEDEP, lib32gcc1`'LS, ${shlibs:Depends}, ${misc:Depends}
Conflicts: ${confl:lib32}
ifdef(`TARGET',`Provides: lib32stdc++CXX_SO-TARGET-dcv1
',`')`'dnl
Description: The GNU Standard C++ Library v3 (32 bit Version)
 This package contains an additional runtime library for C++ programs
 built with the GNU compiler.
ifdef(`TARGET', `dnl
 .
 This package contains files for TARGET architecture, for use in cross-compile
 environment.
')`'dnl
')`'dnl lib32cxx

ifenabled(`lib64cxx',`
Package: lib64stdc++CXX_SO`'LS
Architecture: ifdef(`TARGET',`all',`biarch64_archs')
Section: ifdef(`TARGET',`devel',`libs')
Priority: ifdef(`TARGET',`extra',PRI(optional))
Depends: BASEDEP, ${shlibs:Depends}, lib64gcc1`'LS, ${misc:Depends}
ifdef(`TARGET',`Provides: lib64stdc++CXX_SO-TARGET-dcv1
',`')`'dnl
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

ifenabled(`libn32cxx',`
Package: libn32stdc++CXX_SO`'LS
Architecture: ifdef(`TARGET',`all',`biarchn32_archs')
Section: ifdef(`TARGET',`devel',`libs')
Priority: ifdef(`TARGET',`extra',PRI(optional))
Depends: BASEDEP, ${shlibs:Depends}, libn32gcc1`'LS, ${misc:Depends}
ifdef(`TARGET',`Provides: libn32stdc++CXX_SO-TARGET-dcv1
',`')`'dnl
Description: The GNU Standard C++ Library v3`'ifdef(`TARGET)',` (TARGET)', `') (n32)
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
')`'dnl libn32cxx

ifenabled(`libneoncxx',`
Package: libstdc++CXX_SO-neon
Architecture: NEON_ARCHS
Section: libs
Priority: extra
Depends: BASEDEP, libc6-neon, libgcc1-neon, ${shlibs:Depends}, ${misc:Depends}
Description: The GNU Standard C++ Library v3 [NEON version]
 This package contains an additional runtime library for C++ programs
 built with the GNU compiler.
 .
 This set of libraries is optimized to use a NEON coprocessor, and will
 be selected instead when running under systems which have one.
')`'dnl

ifenabled(`c++dev',`
Package: libstdc++CXX_SO`'PV-dev`'LS
Architecture: ifdef(`TARGET',`all',`any')
Section: ifdef(`TARGET',`devel',`libdevel')
Priority: ifdef(`TARGET',`extra',PRI(optional))
Depends: BASEDEP, g++`'PV`'TS (= ${gcc:Version}), libstdc++CXX_SO`'LS (>= ${gcc:Version}), ${dep:libcdev}, ${misc:Depends}
ifdef(`TARGET',`',`dnl native
Conflicts: libg++27-dev, libg++272-dev (<< 2.7.2.8-1), libstdc++2.8-dev, libg++2.8-dev, libstdc++2.9-dev, libstdc++2.9-glibc2.1-dev, libstdc++2.10-dev (<< 1:2.95.3-2), libstdc++3.0-dev
Suggests: libstdc++CXX_SO`'PV-doc
')`'dnl native
Provides: libstdc++-dev`'LS`'dnl
ifdef(`TARGET',`, libstdc++-dev-TARGET-dcv1, libstdc++CXX_SO-dev-TARGET-dcv1
',`
')`'dnl
Description: The GNU Standard C++ Library v3 (development files)`'ifdef(`TARGET)',` (TARGET)', `')
 This package contains the headers and static library files necessary for
 building C++ programs which use libstdc++.
 .
 libstdc++-v3 is a complete rewrite from the previous libstdc++-v2, which
 was included up to g++-2.95. The first version of libstdc++-v3 appeared
 in g++-3.0.
ifdef(`TARGET', `dnl
 .
 This package contains files for TARGET architecture, for use in cross-compile
 environment.
')`'dnl

Package: libstdc++CXX_SO`'PV-pic`'LS
Architecture: ifdef(`TARGET',`all',`any')
Section: ifdef(`TARGET',`devel',`libdevel')
Priority: extra
Depends: BASEDEP, libstdc++CXX_SO`'LS (>= ${gcc:Version}), libstdc++CXX_SO`'PV-dev`'LS (= ${gcc:Version}), ${misc:Depends}
ifdef(`TARGET',`Provides: libstdc++CXX_SO-pic-TARGET-dcv1
',`')`'dnl
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

Package: libstdc++CXX_SO`'PV-dbg`'LS
Architecture: ifdef(`TARGET',`all',`any')
Section: debug
Priority: extra
Depends: BASEDEP, libstdc++CXX_SO`'LS (>= ${gcc:Version}), libgcc`'GCC_SO-dbg`'LS, ${shlibs:Depends}, ${misc:Depends}
ifdef(`TARGET',`Provides: libstdc++CXX_SO-dbg-TARGET-dcv1
',`')`'dnl
Recommends: libstdc++CXX_SO`'PV-dev`'LS (= ${gcc:Version})
Conflicts: libstdc++5-dbg`'LS, libstdc++5-3.3-dbg`'LS, libstdc++6-dbg`'LS, libstdc++6-4.0-dbg`'LS, libstdc++6-4.1-dbg`'LS, libstdc++6-4.2-dbg`'LS, libstdc++6-4.3-dbg`'LS
Description: The GNU Standard C++ Library v3 (debugging files)`'ifdef(`TARGET)',` (TARGET)', `')
 This package contains the shared library of libstdc++ compiled with
 debugging symbols.
ifdef(`TARGET', `dnl
 .
 This package contains files for TARGET architecture, for use in cross-compile
 environment.
')`'dnl

Package: lib32stdc++CXX_SO`'PV-dbg`'LS
Architecture: ifdef(`TARGET',`all',`biarch32_archs')
Section: debug
Priority: extra
Depends: BASEDEP, lib32stdc++CXX_SO`'LS (>= ${gcc:Version}), libstdc++CXX_SO`'PV-dev`'LS (= ${gcc:Version}), lib32gcc`'GCC_SO-dbg`'LS, ${shlibs:Depends}, ${misc:Depends}
ifdef(`TARGET',`Provides: lib32stdc++CXX_SO-dbg-TARGET-dcv1
',`')`'dnl
Conflicts: lib32stdc++6-dbg`'LS, lib32stdc++6-4.0-dbg`'LS, lib32stdc++6-4.1-dbg`'LS, lib32stdc++6-4.2-dbg`'LS, lib32stdc++6-4.3-dbg`'LS
Description: The GNU Standard C++ Library v3 (debugging files)`'ifdef(`TARGET)',` (TARGET)', `')
 This package contains the shared library of libstdc++ compiled with
 debugging symbols.
ifdef(`TARGET', `dnl
 .
 This package contains files for TARGET architecture, for use in cross-compile
 environment.
')`'dnl

Package: lib64stdc++CXX_SO`'PV-dbg`'LS
Architecture: ifdef(`TARGET',`all',`biarch64_archs')
Section: debug
Priority: extra
Depends: BASEDEP, lib64stdc++CXX_SO`'LS (>= ${gcc:Version}), libstdc++CXX_SO`'PV-dev`'LS (= ${gcc:Version}), lib64gcc`'GCC_SO-dbg`'LS, ${shlibs:Depends}, ${misc:Depends}
ifdef(`TARGET',`Provides: lib64stdc++CXX_SO-dbg-TARGET-dcv1
',`')`'dnl
Conflicts: lib64stdc++6-dbg`'LS, lib64stdc++6-4.0-dbg`'LS, lib64stdc++6-4.1-dbg`'LS, lib64stdc++6-4.2-dbg`'LS, lib64stdc++6-4.3-dbg`'LS
Description: The GNU Standard C++ Library v3 (debugging files)`'ifdef(`TARGET)',` (TARGET)', `')
 This package contains the shared library of libstdc++ compiled with
 debugging symbols.
ifdef(`TARGET', `dnl
 .
 This package contains files for TARGET architecture, for use in cross-compile
 environment.
')`'dnl

Package: libn32stdc++CXX_SO`'PV-dbg`'LS
Architecture: ifdef(`TARGET',`all',`biarchn32_archs')
Section: debug
Priority: extra
Depends: BASEDEP, libn32stdc++CXX_SO`'LS (>= ${gcc:Version}), libstdc++CXX_SO`'PV-dev`'LS (= ${gcc:Version}), libn32gcc`'GCC_SO-dbg`'LS, ${shlibs:Depends}, ${misc:Depends}
ifdef(`TARGET',`Provides: libn32stdc++CXX_SO-dbg-TARGET-dcv1
',`')`'dnl
Conflicts: libn32stdc++6-dbg`'LS, libn32stdc++6-4.0-dbg`'LS, libn32stdc++6-4.1-dbg`'LS, libn32stdc++6-4.2-dbg`'LS, libn32stdc++6-4.3-dbg`'LS
Description: The GNU Standard C++ Library v3 (debugging files)`'ifdef(`TARGET)',` (TARGET)', `')
 This package contains the shared library of libstdc++ compiled with
 debugging symbols.
ifdef(`TARGET', `dnl
 .
 This package contains files for TARGET architecture, for use in cross-compile
 environment.
')`'dnl

ifdef(`TARGET', `', `
Package: libstdc++CXX_SO`'PV-doc
Architecture: all
Section: doc
Priority: PRI(optional)
Depends: gcc`'PV-base (>= ${gcc:SoftVersion}), ${misc:Depends}
Conflicts: libstdc++5-doc, libstdc++5-3.3-doc, libstdc++6-doc, libstdc++6-4.0-doc, libstdc++6-4.1-doc, libstdc++6-4.2-doc, libstdc++6-4.3-doc
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
Depends: gnat`'PV-base (= ${gnat:Version}), gcc`'PV (>= ${gcc:SoftVersion}), ${dep:libgnat}, ${dep:libcdev}, ${shlibs:Depends}, ${misc:Depends}
Suggests: gnat`'PV-doc, ada-reference-manual, gprbuild, gnat-gps
Provides: ada-compiler
Conflicts: gnat (<< 4.1), gnat-3.1, gnat-3.2, gnat-3.3, gnat-3.4, gnat-4.0, gnat-4.1, gnat-4.2, gnat-4.3
Replaces: gnat (<< 4.1), gnat-3.1, gnat-3.2, gnat-3.3, gnat-3.4, gnat-4.0, gnat-4.1, gnat-4.2, gnat-4.3
Description: The GNU Ada compiler
 This is the GNU Ada compiler, which compiles Ada on platforms supported
 by the gcc compiler. It uses the gcc backend to generate optimized code.

ifenabled(`libgnat',`
Package: libgnat`'-GNAT_V
Section: libs
Architecture: any
Priority: PRI(optional)
Depends: gnat`'PV-base (= ${gnat:Version}), ${shlibs:Depends}, ${misc:Depends}
Description: Runtime library for GNU Ada applications
 Library needed for GNU Ada applications linked against the shared library.

Package: libgnat`'-GNAT_V-dbg
Section: debug
Architecture: any
Priority: extra
Depends: gnat`'PV-base (= ${gnat:Version}), gnat`'-GNAT_V (= ${gnat:Version}), libgnat`'-GNAT_V (= ${gnat:Version}), ${misc:Depends}
Description: Runtime library for GNU Ada applications
 Debugging symbols for the library needed for GNU Ada applications linked
 against the shared library.

Package: libgnatvsn-dev
Section: libdevel
Architecture: all
Priority: PRI(optional)
Depends: libgnatvsn`'GNAT_V-dev (= ${gnat:Version}), gnat`'PV-base (= ${gnat:Version})
Description: GNU Ada compiler version library - development files
 This is a dummy transition package to ease upgrades from Debian 5.0
 Lenny.  You can safely remove it.

Package: libgnatvsn`'GNAT_V-dev
Section: libdevel
Architecture: any
Priority: PRI(optional)
Depends: gnat`'PV-base (= ${gnat:Version}), gnat`'PV (= ${gnat:Version}), libgnatvsn`'GNAT_V (= ${gnat:Version}), ${misc:Depends}
Conflicts: libgnatvsn-dev (<< `'GNAT_V), libgnatvsn4.1-dev, libgnatvsn4.3-dev
Replaces: libgnatvsn-dev (<< `'GNAT_V), libgnatvsn4.1-dev, libgnatvsn4.3-dev
Suggests: libgnatvsn`'GNAT_V-dbg
Description: GNU Ada compiler version library - development files
 This library exports selected components of GNAT, the GNU Ada compiler, for use
 in other packages, most notably ASIS and ASIS-based packages.  It is licensed
 under the GNAT-Modified GPL, allowing to link proprietary programs with it.
 .
 This package contains the development files and static library.

Package: libgnatvsn`'GNAT_V
Architecture: any
Priority: PRI(optional)
Section: libs
Depends: gnat`'PV-base (= ${gnat:Version}), libgnat`'-GNAT_V (= ${gnat:Version}), ${misc:Depends}
Description: GNU Ada compiler version library
 This library exports selected components of GNAT, the GNU Ada compiler, for use
 in other packages, most notably ASIS and ASIS-based packages.  It is licensed
 under the GNAT-Modified GPL, allowing to link proprietary programs with it.
 .
 This package contains the run-time shared library.

Package: libgnatvsn`'GNAT_V-dbg
Architecture: any
Priority: extra
Section: debug
Depends: gnat`'PV-base (= ${gnat:Version}), libgnatvsn`'GNAT_V (= ${gnat:Version}), libgnatvsn`'GNAT_V-dev (= ${gnat:Version}), ${misc:Depends}
Description: GNU Ada compiler version library
 This library exports selected components of GNAT, the GNU Ada compiler, for use
 in other packages, most notably ASIS and ASIS-based packages.  It is licensed
 under the GNAT-Modified GPL, allowing to link proprietary programs with it.
 .
 This package contains the debugging symbols for the run-time shared library.

Package: libgnatprj-dev
Section: libdevel
Architecture: all
Priority: PRI(optional)
Depends: libgnatprj`'GNAT_V-dev (= ${gnat:Version}), gnat`'PV-base (= ${gnat:Version})
Description: GNU Ada compiler version library - development files
 This is a dummy transition package to ease upgrades from Debian 5.0
 Lenny.  You can safely remove it.

Package: libgnatprj`'GNAT_V-dev
Section: libdevel
Architecture: any
Priority: PRI(optional)
Depends: gnat`'PV-base (= ${gnat:Version}), gnat`'PV (= ${gnat:Version}), libgnatprj`'GNAT_V (= ${gnat:Version}), libgnatvsn`'GNAT_V-dev (= ${gnat:Version}), ${misc:Depends}
Conflicts: libgnatprj-dev (<< `'GNAT_V), libgnatprj4.1-dev, libgnatprj4.3-dev
Replaces: libgnatprj-dev (<< `'GNAT_V), libgnatprj4.1-dev, libgnatprj4.3-dev
Suggests: libgnatprj`'GNAT_V-dbg
Description: GNU Ada Project Manager development files
 GNAT, the GNU Ada compiler, uses project files to organise source and object
 files in large-scale development efforts.  Several other tools, such as
 ASIS tools (package asis-programs) and GNAT Programming Studio (package
 gnat-gps) also use project files.  This library contains the necessary
 support; it was built from GNAT itself.  It is licensed under the pure GPL;
 all programs that use it must also be distributed under the GPL, or not
 distributed at all.
 .
 This package contains development files: install it to develop applications
 that understand GNAT project files.

Package: libgnatprj`'GNAT_V
Architecture: any
Priority: PRI(optional)
Section: libs
Depends: gnat`'PV-base (= ${gnat:Version}), libgnat`'-GNAT_V (= ${gnat:Version}), libgnatvsn`'GNAT_V (= ${gnat:Version}), ${misc:Depends}
Description: GNU Ada Project Manager
 GNAT, the GNU Ada compiler, uses project files to organise source and object
 files in large-scale development efforts.  Several other tools, such as
 ASIS tools (package asis-programs) and GNAT Programming Studio (package
 gnat-gps) also use project files.  This library contains the necessary
 support; it was built from GNAT itself.  It is licensed under the pure GPL;
 all programs that use it must also be distributed under the GPL, or not
 distributed at all.
 .
 This package contains the run-time shared library.

Package: libgnatprj`'GNAT_V-dbg
Architecture: any
Priority: extra
Section: debug
Depends: gnat`'PV-base (= ${gnat:Version}), libgnatprj`'GNAT_V (= ${gnat:Version}), libgnatprj`'GNAT_V-dev (= ${gnat:Version}), ${misc:Depends}
Description: GNU Ada Project Manager
 GNAT, the GNU Ada compiler, uses project files to organise source and object
 files in large-scale development efforts.  Several other tools, such as
 ASIS tools (package asis-programs) and GNAT Programming Studio (package
 gnat-gps) also use project files.  This library contains the necessary
 support; it was built from GNAT itself.  It is licensed under the pure GPL;
 all programs that use it must also be distributed under the GPL, or not
 distributed at all.
 .
 This package contains the debugging symbols for the run-time shared library.
')`'dnl libgnat

ifenabled(`lib64gnat',`
Package: lib64gnat`'-GNAT_V
Section: libs
Architecture: biarch64_archs
Priority: PRI(optional)
Depends: gnat`'PV-base (= ${gnat:Version}), ${dep:libcbiarch}, ${shlibs:Depends}, ${misc:Depends}
Description: Runtime library for GNU Ada applications
 Library needed for GNU Ada applications linked against the shared library.
')`'dnl libgnat

ifenabled(`gfdldoc',`
Package: gnat`'PV-doc
Architecture: all
Section: doc
Priority: PRI(optional)
Depends: dpkg (>= 1.15.4) | install-info, ${misc:Depends}
Suggests: gnat`'PV
Conflicts: gnat-4.1-doc, gnat-4.2-doc, gnat-4.3-doc
Description: Documentation for the GNU Ada compiler (gnat)
 Documentation for the GNU Ada compiler in info `format'.
')`'dnl gfdldoc
')`'dnl ada

ifenabled(`pascal',`
Package: gpc`'PV
Architecture: any
Priority: PRI(optional)
Depends: SOFTBASEDEP, gcc`'PV (>= ${gcc:SoftVersion}), ${dep:libcdev}, ${shlibs:Depends}, ${misc:Depends}
Recommends: libgmp3-dev, libncurses5-dev
Suggests: gpc`'PV-doc (>= ${gpc:Version})
Provides: pascal-compiler
Description: The GNU Pascal compiler
 This is the GNU Pascal compiler, which compiles Pascal on platforms supported
 by the gcc compiler. It uses the gcc backend to generate optimized code.
Homepage: http://www.gnu-pascal.de/gpc/h-index.html

Package: gpc`'PV-doc
Architecture: all
Section: doc
Priority: PRI(optional)
Depends: SOFTBASEDEP, dpkg (>= 1.15.4) | install-info, ${misc:Depends}
Replaces: gpc (<= 2.91.58-3)
Suggests: gpc`'PV
Description: Documentation for the GNU Pascal compiler (gpc)
 Documentation for the GNU Pascal compiler in info `format'.
Homepage: http://www.gnu-pascal.de/gpc/h-index.html
')`'dnl pascal

ifenabled(`d ',`
Package: gdc`'PV
Architecture: any
Priority: PRI(optional)
Depends: SOFTBASEDEP, g++`'PV (>= ${gcc:SoftVersion}), libphobos`'PHOBOS_V`'PV-dev (= ${gdc:Version}) [libphobos_no_archs], ${shlibs:Depends}, ${misc:Depends}
Provides: d-compiler
Description: The D compiler
 This is the D compiler, which compiles D on platforms supported by the gcc
 compiler. It uses the GCC backend to generate optimised code.
Homepage: https://bitbucket.org/goshawk/gdc

ifenabled(`libphobos',`
Package: libphobos`'PHOBOS_V`'PV`'TS-dev
Architecture: any
Section: libdevel
Priority: PRI(optional)
Depends: gdc`'PV`'TS (= ${gdc:Version}), zlib1g-dev, ${shlibs:Depends}, ${misc:Depends}
Provides: libphobos`'PHOBOS_V`'TS-dev
Description: The phobos D standard library
 This is the Phobos standard library that comes with the D compiler.
 .
 For more information check http://www.digitalmars.com/d/phobos/phobos.html
')`'dnl libphobos
')`'dnl d

ifdef(`TARGET',`',`dnl
ifenabled(`libs',`
Package: gcc`'PV-soft-float
Architecture: arm armel
Priority: PRI(optional)
Depends: BASEDEP, ifenabled(`cdev',`gcc`'PV (= ${gcc:Version}),') ${shlibs:Depends}, ${misc:Depends}
Replaces: gcc-soft-float-ss
Description: The soft-floating-point gcc libraries (arm)
 These are versions of basic static libraries such as libgcc.a compiled
 with the -msoft-float option, for CPUs without a floating-point unit.
')`'dnl commonlibs
')`'dnl

ifenabled(`fixincl',`
Package: fixincludes
Architecture: any
Priority: PRI(optional)
Depends: BASEDEP, gcc`'PV (= ${gcc:Version}), ${shlibs:Depends}, ${misc:Depends}
Description: Fix non-ANSI header files
 FixIncludes was created to fix non-ANSI system header files. Many
 system manufacturers supply proprietary headers that are not ANSI compliant.
 The GNU compilers cannot compile non-ANSI headers. Consequently, the
 FixIncludes shell script was written to fix the header files.
 .
 Not all packages with header files are installed on the system, when the
 package is built, so we make fixincludes available at build time of other
 packages, such that checking tools like lintian can make use of it.
')`'dnl fixincl

ifenabled(`cdev',`
ifdef(`TARGET', `', `
ifenabled(`gfdldoc',`
Package: gcc`'PV-doc
Architecture: all
Section: doc
Priority: PRI(optional)
Depends: gcc`'PV-base (>= ${gcc:SoftVersion}), dpkg (>= 1.15.4) | install-info, ${misc:Depends}
Conflicts: gcc-docs (<< 2.95.2)
Replaces: gcc (<=2.7.2.3-4.3), gcc-docs (<< 2.95.2)
Description: Documentation for the GNU compilers (gcc, gobjc, g++)
 Documentation for the GNU compilers in info `format'.
')`'dnl gfdldoc
')`'dnl native
')`'dnl cdev

ifdef(`TARGET',`',`dnl
ifenabled(`libnof',`
Package: gcc`'PV-nof
Architecture: powerpc
Priority: PRI(optional)
Depends: BASEDEP, ${shlibs:Depends}ifenabled(`cdev',`, gcc`'PV (= ${gcc:Version})'), ${misc:Depends}
Conflicts: gcc-3.2-nof
Description: The no-floating-point gcc libraries (powerpc)
 These are versions of basic static libraries such as libgcc.a compiled
 with the -msoft-float option, for CPUs without a floating-point unit.
')`'dnl libnof
')`'dnl

ifenabled(`source',`
Package: gcc`'PV-source
Architecture: all
Priority: PRI(optional)
Depends: make (>= 3.81), autoconf2.59, automake1.9, quilt, patchutils, ${misc:Depends}
Description: Source of the GNU Compiler Collection
 This package contains the sources and patches which are needed to
 build the GNU Compiler Collection (GCC).
')`'dnl source
dnl
')`'dnl gcc-X.Y
dnl last line in file
