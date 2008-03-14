include $(top_srcdir)/Makefile.decl

SUBDIRS = . tests

DIST_SUBDIRS = tests

NULL =

INCLUDES = \
	-I$(top_srcdir) 		\
	-DPREFIX=\""$(prefix)"\" 	\
	-DLIBDIR=\""$(libdir)"\" 	\
	-DG_DISABLE_DEPRECATED 		\
	-DG_LOG_DOMAIN=\"XmlReader\"	\
	$(XMLR_CFLAGS) 			\
	$(XMLR_DEBUG_CFLAGS) 		\
	$(NULL)

LDADD = $(XMLR_LT_LDFLAGS) -export-dynamic -rpath $(libdir)

BUILT_SOURCES =

source_h = \
	$(top_srcdir)/xml-reader/xml-reader.h \
	$(NULL)

source_h_private =

source_c = \
	xml-reader.c \
	$(NULL)

lib_LTLIBRARIES = libxml-reader-1.0.la

libxml_reader_1_0_la_LIBADD = $(XMLR_LIBS)
libxml_reader_1_0_la_SOURCES = \
	$(source_c) \
	$(source_h) \
	$(source_h_priv) \
	$(BUILT_SOURCES) \
	$(NULL)
libxml_reader_1_0_la_LDFLAGS = $(LDADD)

xmlreaderincludedir = $(includedir)/xml-reader-1.0/xml-reader
xmlreaderinclude_DATA = $(source_h)

CLEANFILES = $(STAMP_FILES) $(BUILT_SOURCES)

DISTCLEANFILES =

TESTS_ENVIRONMENT = srcdir="$(srcdir)" xml_reader_all_c_sources="$(source_c)"
