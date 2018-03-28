# **************************************************************************

noinst_LTLIBRARIES = libmpeg2enc.la

# Remove any linklibraries detected by configure, as they are not
# needed in the submodules -- only in the toplevel libsimage build.
LIBS = 

AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_builddir)/include

MPEG2ENCSOURCES = \
	api.c api.h \
	conform.c \
	fdctref.c \
	global.h \
	idct.c \
	motion.c \
	mpeg2enc.h \
	predict.c \
	putbits.c \
	puthdr.c \
	putmpg.c \
	putpic.c \
	putseq.c putseq.h \
	putvlc.c \
	quantize.c \
	ratectl.c \
	readpic.c \
	stats.c \
	transfrm.c \
	vlc.h \
	writepic.c

libmpeg2enc_la_SOURCES = \
	$(MPEG2ENCSOURCES)

# EOF **********************************************************************
