lib_LTLIBRARIES = libGLC.la

libGLC_la_SOURCES = $(top_builddir)/src/context.c \
                    $(top_builddir)/src/font.c \
                    $(top_builddir)/src/internal.h \
                    $(top_builddir)/src/global.c \
                    $(top_builddir)/src/master.c \
                    $(top_builddir)/src/measure.c \
                    $(top_builddir)/src/misc.c \
                    $(top_builddir)/src/oarray.c \
                    $(top_builddir)/src/oarray.h \
                    $(top_builddir)/src/ocharmap.c \
                    $(top_builddir)/src/ocharmap.h \
                    $(top_builddir)/src/ocontext.c \
                    $(top_builddir)/src/ocontext.h \
                    $(top_builddir)/src/ofacedesc.c \
                    $(top_builddir)/src/ofacedesc.h \
                    $(top_builddir)/src/ofont.c \
                    $(top_builddir)/src/ofont.h \
                    $(top_builddir)/src/omaster.c \
                    $(top_builddir)/src/omaster.h \
                    $(top_builddir)/src/render.c \
                    $(top_builddir)/src/scalable.c \
                    $(top_builddir)/src/transform.c \
                    $(top_builddir)/src/oglyph.h \
                    $(top_builddir)/src/oglyph.c \
                    $(top_builddir)/src/texture.h \
                    $(top_builddir)/src/texture.c \
                    $(top_builddir)/src/unicode.c

EXTRA_libGLC_la_SOURCES = $(top_builddir)/src/fribidi/fribidi.h \
                          $(top_builddir)/src/fribidi/fribidi.c \
                          $(top_builddir)/src/fribidi/fribidi_char_type.c \
                          $(top_builddir)/src/fribidi/fribidi_mirroring.c \
                          $(top_builddir)/src/fribidi/fribidi_types.c \
                          $(top_builddir)/src/fribidi/fribidi_types.h \
                          $(top_builddir)/src/fribidi/fribidi_types.i \
                          $(top_builddir)/src/fribidi/fribidi_unicode.h \
                          $(top_builddir)/src/fribidi/fribidi_tab_mirroring.i \
                          $(top_builddir)/src/fribidi/fribidi_tab_char_type_9.i \
                          $(top_builddir)/src/glew.c \
                          $(top_builddir)/src/sqlite3/sqlite3.h \
                          $(top_builddir)/src/sqlite3/sqlite3ext.h \
                          $(top_builddir)/src/sqlite3/sqlite3.c

libGLC_la_CFLAGS = -I$(top_builddir)/src \
                   -D_REENTRANT \
                   -DDATAPATH=\"$(datadir)\" \
                   @GLEW_CFLAGS@ \
                   @SQLITE3_CFLAGS@ \
                   @PTHREAD_CFLAGS@ \
                   @FONTCONFIG_CFLAGS@ \
                   @FREETYPE2_CFLAGS@ \
                   @GL_CFLAGS@ \
                   @GLU_CFLAGS@ \
                   @FRIBIDI_CFLAGS@

libGLC_la_LIBADD = @GLU_LIBS@ \
                   @FONTCONFIG_LIBS@ \
                   @FREETYPE2_LIBS@ \
                   @PTHREAD_LIBS@ \
                   @FRIBIDI_LIBS@ \
                   @EMBEDDED_OBJ@

libGLC_la_DEPENDENCIES = @EMBEDDED_OBJ@

libGLC_la_LDFLAGS = -version-info 0:8:0

libGLC_ladir = $(datadir)
libGLC_la_DATA = $(top_builddir)/database/quesoglc.db

$(top_builddir)/database/quesoglc.db: $(top_builddir)/database/buildDB.py
	cd $(top_builddir)/database && python buildDB.py && cd $(top_builddir)/src

clean-local:
	rm -f *.gcno *.gcda *.gcov
