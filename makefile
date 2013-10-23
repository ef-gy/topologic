DESTDIR:=
PREFIX:=/usr/local
BINDIR:=$(DESTDIR)$(PREFIX)/bin
INCLUDEDIR:=$(DESTDIR)$(PREFIX)/include
MANDIR:=$(DESTDIR)$(PREFIX)/share/man

NAME:=topologic
VERSION:=7

CC:=clang
CXX:=clang++
EMXX:=em++
PKGCONFIG:=pkg-config
INSTALL:=install
XSLTPROC:=xsltproc

UNAME:=$(shell uname)
LIBRARIES:=libxml-2.0
LIBRARIES_GL:=libxml-2.0 glew
FRAMEWORKS:=
FRAMEWORKS_GL:=GLUT OpenGL Cocoa

DEBUG:=false

ifneq ($(UNAME),Darwin)
PCCFLAGS:=$(shell $(PKGCONFIG) --cflags $(LIBRARIES) 2>/dev/null)
PCLDFLAGS:=$(shell $(PKGCONFIG) --libs $(LIBRARIES) 2>/dev/null)
GLCFLAGS:=$(shell $(PKGCONFIG) --cflags $(LIBRARIES_GL) 2>/dev/null)
GLLDFLAGS:=$(shell $(PKGCONFIG) --libs $(LIBRARIES_GL) 2>/dev/null) -lglut
else
PCCFLAGS:=-I/usr/include/libxml2
PCLDFLAGS:=-lxml2 $(addprefix -framework ,$(FRAMEWORKS))
GLCFLAGS:=
GLLDFLAGS:=$(addprefix -framework ,$(FRAMEWORKS_GL))
endif
CFLAGS:=-O2 $(shell if $(DEBUG); then echo '-g'; fi)
CXXFLAGS:=$(CFLAGS)
EMCFLAGS:=-O2 --llvm-lto 3 -s TOTAL_MEMORY=33554432 -s OUTLINING_LIMIT=20480 -DNOVAO
EMXXFLAGS:=$(EMCFLAGS)
LDFLAGS:=

JSFUNCTIONS:=['_main','_setRadius','_setPrecision','_updateModel','_updateProjection','_getProjection','_interpretDrag','_setActiveDimension','cwrap']

DATABASE:=
BINARIES:=$(filter-out %-glut %-gles %-json,$(basename $(notdir $(wildcard src/*.cpp))))
GLBINARIES:=$(filter %-glut,$(basename $(notdir $(wildcard src/*.cpp))))
JSBINARIES:=$(addsuffix .js,$(BINARIES))
TESTBINARIES:=$(filter test-%,$(BINARIES))

IGNOREBINARIES:=
IBINARIES:=$(addprefix $(BINDIR)/,$(BINARIES))
IGLBINARIES:=$(addprefix $(BINDIR)/,$(GLBINARIES))
IINCLUDES:=$(addprefix $(INCLUDEDIR)/topologic/,$(notdir $(wildcard include/topologic/*.h)))
IMANPAGES:=$(addprefix $(MANDIR)/man1/,$(notdir $(wildcard src/*.1)))

# don't delete intermediary files
.SECONDARY:

# meta rules
all: $(DATABASES) $(BINARIES)
clean:
	rm -f $(DATABASES) $(BINARIES); true
scrub: clean

install: $(IBINARIES) $(IINCLUDES) $(IMANPAGES)
uninstall:
	rm -f $(IBINARIES) $(IINCLUDES) $(IMANPAGES)

archive: ../$(NAME)-$(VERSION).tar.gz

../$(NAME)-$(VERSION).tar.gz:
	git archive --format=tar --prefix=$(NAME)-$(VERSION)/ HEAD | gzip -9 >$@

# meta rules for documentation
documentation.xml: doxyfile include/*/* xslt/doxy*
	doxygen $^
	cd documentation/xml && $(XSLTPROC) combine.xslt index.xml | $(XSLTPROC) ../../xslt/doxygen-post-process.xslt - > ../../$@

# meta rules for javascript
js: $(JSBINARIES)

# meta rules for OpenGL
gl: $(GLBINARIES)
install-glut: $(IGLBINARIES)
uninstall-glut:
	rm -f $(IGLBINARIES)

# run unit tests
test: $(TESTBINARIES)
	for i in $^; do echo TEST: $$i; ./$$i; done

# pattern rules to install things
$(BINDIR)/%: %
	$(INSTALL) -D $< $@

$(INCLUDEDIR)/%: include/%
	$(INSTALL) -D $< $@

$(MANDIR)/man1/%.1: src/%.1
	$(INSTALL) -D $< $@

# pattern rules for databases
%.sqlite3: src/%.sql
	rm -f $@ && $(SQLITE3) $@ < $<

# pattern rules for C++ code
%: src/%.cpp include/*/*.h
	$(CXX) -std=c++0x -Iinclude/ $(CXXFLAGS) $(PCCFLAGS) $< $(LDFLAGS) $(PCLDFLAGS) -o $@ && ($(DEBUG) || strip -x $@)

%-glut: src/%-glut.cpp include/*/*.h
	$(CXX) -std=c++0x -Iinclude/ $(CXXFLAGS) $(PCCFLAGS) $(GLCFLAGS) $< $(LDFLAGS) $(PCLDFLAGS) $(GLLDFLAGS) -o $@ && ($(DEBUG) || strip -x $@)

%.js: src/%.cpp include/*/*.h
	$(EMXX) -std=c++0x -Iinclude/ -D NOLIBRARIES $(EMXXFLAGS) -s EXPORTED_FUNCTIONS="$(JSFUNCTIONS)" $< $(LDFLAGS) -o $@

%.html: src/%.cpp include/*/*.h
	$(EMXX) -std=c++0x -Iinclude/ -D NOLIBRARIES $(EMXXFLAGS) $< $(LDFLAGS) -o $@
