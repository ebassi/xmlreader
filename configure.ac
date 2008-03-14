m4_define([xmlr_major_version], [0])
m4_define([xmlr_minor_version], [1])
m4_define([xmlr_micro_version], [0])

m4_define([xmlr_version], [xmlr_major_version.xmlr_minor_version.xmlr_micro_version])

dnl increment by one for every release that doesn't change the API (addition,
dnl changes or removal); set to 0 in case of changes and/or removals.
m4_define([xmlr_interface_age], [0])
m4_define([xmlr_binary_age], [m4_eval(100 * xmlr_minor_version + xmlr_micro_version)])

m4_define([lt_current], [m4_eval(100 * xmlr_minor_version + xmlr_micro_version - xmlr_interface_age)])
m4_define([lt_revision], [xmlr_interface_age])
m4_define([lt_age], [m4_eval(xmlr_binary_age - xmlr_interface_age)])

m4_define([glib_req_version], [2.15])

AC_PREREQ([2.59])
AC_INIT([xmlr-glib], [xmlr_version], [], [xmlr-glib])
AC_CONFIG_SRCDIR([xmlr-glib/xmlr-glib.h])

AM_INIT_AUTOMAKE([1.9])
AM_CONFIG_HEADER([config.h])

AM_DISABLE_STATIC
AM_PATH_GLIB_2_0
AM_PROG_LIBTOOL

XMLR_MAJOR_VERSION=xmlr_major_version
XMLR_MINOR_VERSION=xmlr_minor_version
XMLR_MICRO_VERSION=xmlr_micro_version
XMLR_VERSION=xmlr_version
AC_SUBST(XMLR_MAJOR_VERSION)
AC_SUBST(XMLR_MICRO_VERSION)
AC_SUBST(XMLR_MINOR_VERSION)
AC_SUBST(XMLR_VERSION)

XMLR_LT_CURRENT=lt_current
XMLR_LT_REVISION=lt_revision
XMLR_LT_AGE=lt_age
XMLR_LT_VERSION="$XMLR_LT_CURRENT:$XMLR_LT_REVISION:$XMLR_LT_AGE"
XMLR_LT_LDFLAGS="-versio-info $XMLR_LT_VERSION"
AC_SUBST(XMLR_LT_LDFLAGS)

AC_PROG_CC
AC_HEADER_STDC
AC_CHECK_HEADERS([unistd.h])
AC_C_CONST
AC_FUNC_MALLOC
AC_FUNC_MMAP

PKG_CHECK_MODULES(XMLR, gobject-2.0 >= glib_req_version)

dnl = Enable debug level ===================================================

m4_define([debug_default], m4_if(m4_eval(xmlr_minor_version % 2), [1], [yes], [minimum]))

AC_ARG_ENABLE(debug,
              AC_HELP_STRING([--enable-debug=@<:@no/minimum/yes@:>@],
                             [turn on debugging @<:@default=debug_default@:>@]),
,
              enable_debug=debug_default)

if test "x$enable_debug" = "xyes"; then
  test "$cflags_set" = set || CFLAGS="$CFLAGS -g"
  XMLR_DEBUG_CFLAGS="-DXMLR_ENABLE_DEBUG"
else
  if test "x$enable_debug" = "xno"; then
    XMLR_DEBUG_CFLAGS="-DG_DISABLE_ASSERT -DG_DISABLE_CHECKS -DG_DISABLE_CAST_CHECKS"
  else # minimum
    XMLR_DEBUG_CFLAGS="-DXMLR_ENABLE_DEBUG -DG_DISABLE_CAST_CHECKS"
  fi
fi

AC_SUBST(XMLR_DEBUG_CFLAGS)

dnl = Enable strict compiler flags =========================================

# use strict compiler flags only on development releases
m4_define([maintainer_flags_default],
          m4_if(m4_eval(xmlr_minor_version % 2), [1], [yes], [no]))
AC_ARG_ENABLE([maintainer-flags],
              AC_HELP_STRING([--enable-maintainer-flags=@<:@no/yes@:>@],
                             [Use strict compiler flags @<:@default=maintainer_flags_default@:>@]),,
              enable_maintainer_flags=maintainer_flags_default)

if test "x$enable_maintainer_flags" = "xyes"; then
  CPPFLAGS="$CPPFLAGS -g -Wall -Wshadow -Wcast-align -Wno-uninitialized -Werror"
else
  CPPFLAGS="$CPPFLAGS -g -Wall"
fi

GTK_DOC_CHECK([1.8])

AC_CONFIG_FILES([
        Makefile
        xml-reader/Makefile
        xml-reader/tests/Makefile
        tests/Makefile
        doc/Makefile
        doc/reference/Makefile
        doc/reference/version.xml
        xml-reader-1.0.pc
])

AC_OUTPUT

echo ""
echo " XML Reader $VERSION"
echo ""
echo " Prefix: ${prefix}"
echo " Debug level: ${enable_debug}"
echo " Compiler flags: ${CPPFLAGS}"
echo " API reference: ${enable_gtk_doc}"
echo ""
