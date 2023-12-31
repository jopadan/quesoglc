#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)

# Init autoconf.
# --------------

AC_INIT([QuesoGLC], [0.7.9],
        [quesoglc-general@lists.sourceforge.net],
        [quesoglc])

AC_CONFIG_HEADER([include/qglc_config.h:include/qglc_config.hin])
AC_CONFIG_MACRO_DIR([build/m4])

AC_CONFIG_SRCDIR([src/global.c])
AC_CONFIG_AUX_DIR([build])

# Optional parameters definition.
# -------------------------------

AC_ARG_WITH([fribidi],
  AC_HELP_STRING([--with-fribidi],
   [use the FriBiDi system library @<:@default=yes@:>@]),
   [], [with_fribidi=yes]
)
AC_ARG_WITH([glew],
  AC_HELP_STRING([--with-glew],
   [use the GLEW system library @<:@default=yes@:>@]),
   [], [with_glew=yes]
)
AC_ARG_ENABLE([glew-multiple-contexts],
  AC_HELP_STRING([--enable-glew-multiple-contexts],
   [enable GLEW thread-safe support for multiple rendering contexts \
    @<:@default=no@:>@]),
   [], [enable_glew_multiple_contexts=no]
)
AC_ARG_WITH([sqlite3],
  AC_HELP_STRING([--with-sqlite3],
   [use the SQLite3 system library @<:@default=yes@:>@]),
   [], [with_sqlite3=yes]
)
AC_ARG_ENABLE([executables],
  AC_HELP_STRING([--enable-executables],
   [build example and test executables @<:@default=yes@:>@]),
   [], [enable_executables=yes]
)
AC_ARG_ENABLE([debug],
  AC_HELP_STRING([--enable-debug],
   [compile debug version @<:@default=no@:>@]),
   [], [enable_debug=no]
)

# Init automake.
# --------------

AM_INIT_AUTOMAKE([foreign dist-bzip2])

# Init libtool
# ------------

LT_INIT([disable-static])


# Checks for programs.
# --------------------

AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_LIBTOOL


# Checks for multithreading support.
# ----------------------------------

ACX_PTHREAD

CC="$PTHREAD_CC"


# Checks for TLS support.
# -----------------------

AX_CHECK_TLS


# Checks for typedefs, structures, and compiler characteristics.
# --------------------------------------------------------------

AC_C_CONST
AC_C_INLINE

AC_TYPE_SIZE_T

AC_CHECK_SIZEOF(short, 2)
AC_CHECK_SIZEOF(int, 4)
AC_CHECK_SIZEOF(long, 4)


# Checks for header files.
# ------------------------

AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h unistd.h], [],
                 [AC_MSG_ERROR([Unable to locate a required header])])


# Checks for C library features.
# ------------------------------

case $target_alias in
*mingw32*) ;;
*)
  AC_FUNC_MALLOC
  AC_FUNC_MEMCMP
  AC_FUNC_REALLOC
  ;;
esac
AC_FUNC_STAT
AC_CHECK_FUNCS([atexit memmove memset strdup])

AC_CHECK_LIB([m], [sqrt])


# Checks for font libraries.
# --------------------------

AX_CHECK_FREETYPE2

if (test "x$no_ft2" = "xyes"); then
  AC_MSG_ERROR([Unable to locate the required FreeType library])
fi

AC_CHECK_LIB(freetype, FTC_Manager_New,
             AC_DEFINE([HAVE_FT_CACHE], [1],
                       [Define if FreeType supports the caching routines]))

AX_CHECK_FONTCONFIG

if (test "x$no_fc" = "xyes"); then
  AC_MSG_ERROR([Unable to locate the required Fontconfig library])
fi


# Checks for Unicode libraries.
# -----------------------------

if (test "x$with_fribidi" = "xyes"); then
  AX_CHECK_FRIBIDI
  if (test "x$no_fribidi" = "xyes"); then
    AC_MSG_NOTICE([FriBiDi will be built and embedded])
  fi
else
  no_fribidi="yes"
fi

if (test "x$no_fribidi" = "xyes"); then
  EMBEDDED_OBJ="libGLC_la-fribidi.lo libGLC_la-fribidi_char_type.lo \
               libGLC_la-fribidi_mirroring.lo libGLC_la-fribidi_types.lo"
else
  PKGCONFIG_REQUIREMENTS="fribidi"
fi

# Checks for SQLite 3.
# --------------------

if (test "x$with_sqlite3" = "xyes"); then
  AC_CHECK_HEADER(sqlite3.h, AC_CHECK_LIB(sqlite3, sqlite3_open, ,
			     		  no_sqlite3="yes"), no_sqlite3="yes")
  if (test "x$no_sqlite3" = "xyes"); then
    AC_MSG_NOTICE([SQLite3 will be built and embedded])
  fi
else
  no_sqlite3="yes"
fi

if (test "x$no_sqlite3" = "xyes"); then
  EMBEDDED_OBJ="$EMBEDDED_OBJ libGLC_la-sqlite3.lo"
  SQLITE3_CFLAGS="-I$(top_builddir)/src/sqlite3"
else
  PKGCONFIG_LIBS_PRIVATE="-lsqlite3 $PKGCONFIG_LIBS_PRIVATE"
fi

# Checks for OpenGL and related libraries.
# ----------------------------------------

AX_CHECK_GL

if (test "x$no_gl" = "xyes"); then
  AC_MSG_ERROR([OpenGL not found])
fi

AX_CHECK_GLU

if (test "x$no_glu" = "xyes"); then
  AC_MSG_ERROR([GLU library not found])
fi

PKGCONFIG_LIBS_PRIVATE="$PHREAD_LIBS $GLU_LIBS"
PKGCONFIG_INCLUDE="$GLU_CFLAGS"

# Checks for GLEW library.
# ------------------------

if (test "x$with_glew" = "xyes"); then
  AX_CHECK_GLEW
  if (test "x$no_glew" = "xyes"); then
    AC_MSG_NOTICE([GLEW will be built and embedded])
  fi
else
  no_glew="yes"
fi

if (test "x$enable_glew_multiple_contexts" = "xyes"); then
  GLEW_CFLAGS="-DGLEW_MX"
fi

if (test "x$no_glew" = "xyes"); then
  EMBEDDED_OBJ="$EMBEDDED_OBJ libGLC_la-glew.lo"
else
  PKGCONFIG_LIBS_PRIVATE="-lGLEW $PKGCONFIG_LIBS_PRIVATE"
fi

# Special flags for MinGW32 build.
# --------------------------------

case $target_alias in
*mingw32*)
  if (test "x$enable_static" = "xyes"); then
    GLEW_CFLAGS+=" -DGLEW_STATIC"
  else
    GLEW_CFLAGS+=" -DGLEW_BUILD"
  fi
  ;;
esac

# Determine whether to build (or not) test and examples executables.
# ------------------------------------------------------------------

if (test "x$enable_executables" = "xyes"); then

  AX_CHECK_GLUT

  if (test "x$no_glut" = "xyes"); then
    AC_MSG_WARN([The GLUT library could not be found: example programs and \
                 some tests will not be built])
    EXECUTABLES="tests"
  else
    EXECUTABLES="tests examples"
    case $target_alias in
    *mingw32*)
      TESTS_WITH_GLUT="test1 test5 test6 test7 test8 test9.1 test9.2 test9.3 \
                       test9.4 test9.5 test9.6 test9.7 test9.8 test10 test11.1 \
                       test11.2 test11.3 test11.4 test11.5 test11.6 test11.7 \
                       test11.8 test12 test13 test14 test15 test16 test18 \
		       testcontex testfont testmaster testrender"
      ;;
    *)
      TESTS_WITH_GLUT="test1 test2 test3 test5 test6 test7 test8 test9.1 \
                       test9.2 test9.3 test9.4 test9.5 test9.6 test9.7 test9.8 \
                       test10 test11.1 test11.2 test11.3 test11.4 test11.5 \
                       test11.6 test11.7 test11.8 test12 test13 test14 test15 \
		       test16 test18 testcontex testfont testmaster testrender"
      ;;
    esac

  fi
fi

# Compile debug version
# ---------------------

if (test "x$enable_debug" = "xyes"); then
  CFLAGS+=" -g -DDEBUGMODE"
  DEBUG_TESTS="test17"
fi

# Output.
# -------

AC_CONFIG_FILES([Makefile
                 build/Makefile
                 include/Makefile
                 tests/Makefile
                 examples/Makefile
                 quesoglc.pc])

AC_SUBST(EXECUTABLES)
AC_SUBST(DEBUG_TESTS)
AC_SUBST(TESTS_WITH_GLUT)
AC_SUBST(EMBEDDED_OBJ)
AC_SUBST(SQLITE3_CFLAGS)
AC_SUBST(GLEW_CFLAGS)
AC_SUBST(PKGCONFIG_REQUIREMENTS)
AC_SUBST(PKGCONFIG_LIBS_PRIVATE)
AC_SUBST(PKGCONFIG_INCLUDE)
AC_OUTPUT


echo ""
echo "Configure Summary"
echo "================="

echo "Prefix: $prefix"
echo "CFLAGS: $CFLAGS"
echo ""

if (test "x$no_fribidi" = "xyes"); then
  echo "FriBiDi: Build own copy"
else
  echo "FriBiDi: Use system library"
fi

if (test "x$no_glew" = "xyes"); then
  if (test "x$enable_glew_multiple_contexts" = "xyes"); then
    echo "GLEW: Build own copy with support for multiple rendering contexts"
  else
    echo "GLEW: Build own copy with support for a single rendering context"
  fi
else
  if (test "x$enable_glew_multiple_contexts" = "xyes"); then
    echo "GLEW: Use system library with support for multiple rendering contexts"
  else
    echo "GLEW: Use system library with support for a single rendering context"
  fi
fi

if (test "x$no_sqlite3" = "xyes"); then
  echo "SQLite3: Build own copy"
else
  echo "SQLite3: Use system library"
fi

if (test "x$enable_tls" = "xyes"); then
    echo "Support for ELF TLS: Enabled"
else
    echo "Support for ELF TLS: Disabled"
fi

if (test "x$enable_executables" = "xyes"); then
    echo "Build examples and tests: Yes"
else
    echo "Build examples and tests: No"
fi

echo ""
