-include ef.gy/base.mk include/ef.gy/base.mk

NAME:=topologic
VERSION:=11

LIBRARIES:=libxml-2.0
FRAMEWORKS:=

ifeq ($(UNAME),Darwin)
PCCFLAGS:=-I/usr/include/libxml2
PCLDFLAGS:=-lxml2 $(addprefix -framework ,$(FRAMEWORKS))
endif
CXXFLAGS:=$(CFLAGS) -fno-exceptions

libxml/tree.h:: include/libxml/tree.h
libxml/parser.h:: include/libxml/parser.h
libxml/xpath.h:: include/libxml/xpath.h
libxml/xpathInternals.h:: include/libxml/xpathInternals.h

include/libxml/tree.h include/libxml/parser.h include/libxml/xpath.h include/libxml/xpathInternals.h: makefile
	mkdir -p include/libxml || true
	echo "#if !defined(FAKE_LIBXML_H)" > $@
	echo "#define FAKE_LIBXML_H" >> $@
	echo "#define NOLIBRARIES" >> $@
	echo "#endif" >> $@
