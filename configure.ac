AC_INIT(pnacl-hello-world,1.0.0)
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_CONFIG_MACRO_DIR([m4])

m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_CANONICAL_HOST

SYSTEM_PEPPER=0
SYSTEM_SDL=0

case "$host" in
  pnacl-*-pepper)
    SYSTEM_PEPPER=1
    NACL_SDK_ROOT="$HOME/src/nacl_sdk/pepper_35"
    PNACL_TOOLCHAIN_PATH="$NACL_SDK_ROOT/toolchain/linux_pnacl"
    CXX="$PNACL_TOOLCHAIN_PATH/bin/pnacl-clang++"
    STRIP="$PNACL_TOOLCHAIN_PATH/bin/pnacl-strip"
    PNACL_FINALIZE="$PNACL_TOOLCHAIN_PATH/bin/pnacl-finalize"
    HTTPD_PY="$NACL_SDK_ROOT/tools/httpd.py"

    PACKAGES_CFLAGS="-I$NACL_SDK_ROOT/include"
    PACKAGES_LIBS="-L$NACL_SDK_ROOT/lib/pnacl/Release -lppapi_cpp -lppapi -lppapi_gles2"
    ;;


  *-gnu)
    SYSTEM_SDL=1
    PKG_CHECK_MODULES([PACKAGES], [sdl gl glew])
    ;;

  *)
    AC_MSG_ERROR([Unsupported host '$host'])
    ;;
esac

AX_PYTHON

AC_SUBST(PACKAGES_CFLAGS)
AC_SUBST(PACKAGES_LIBS)
AC_SUBST(PNACL_FINALIZE)
AC_SUBST(HTTPD_PY)

AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_MAKE_SET

AC_LANG_PUSH([C++])
AX_CXX_COMPILE_STDCXX_11([ext])
AC_LANG_POP([C++])

AM_CONDITIONAL(SYSTEM_PEPPER, [test $SYSTEM_PEPPER = 1])
AM_CONDITIONAL(SYSTEM_SDL, [test $SYSTEM_SDL = 1])

AC_OUTPUT(Makefile)
