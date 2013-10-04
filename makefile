DESTDIR:=
PREFIX:=/usr/local
BINDIR:=$(DESTDIR)$(PREFIX)/bin
INCLUDEDIR:=$(DESTDIR)$(PREFIX)/include
MANDIR:=$(DESTDIR)$(PREFIX)/share/man

NAME:=topologic
VERSION:=6

CC:=clang
CXX:=clang++
EMXX:=em++
PKGCONFIG:=pkg-config
INSTALL:=install

UNAME:=$(shell uname)
LIBRARIES:=libxml-2.0
LIBRARIES_GL:=
FRAMEWORKS:=
FRAMEWORKS_GL:=GLUT OpenGL Cocoa

DEBUG:=false

ifneq ($(UNAME),Darwin)
PCCFLAGS:=$(shell $(PKGCONFIG) --cflags $(LIBRARIES) 2>/dev/null)
PCLDFLAGS:=$(shell $(PKGCONFIG) --libs $(LIBRARIES) 2>/dev/null)
GLCFLAGS:=$(shell $(PKGCONFIG) --cflags $(LIBRARIES_GL) 2>/dev/null)
GLLDFLAGS:=$(shell $(PKGCONFIG) --libs $(LIBRARIES_GL) 2>/dev/null)
else
PCCFLAGS:=-I/usr/include/libxml2
PCLDFLAGS:=-lxml2 $(addprefix -framework ,$(FRAMEWORKS))
GLCFLAGS:=
GLLDFLAGS:=$(addprefix -framework ,$(FRAMEWORKS_GL))
endif
CFLAGS:=-O2 $(shell if $(DEBUG); then echo '-g'; fi)
CXXFLAGS:=$(CFLAGS)
EMCFLAGS:=-O2 --llvm-lto 3 -s TOTAL_MEMORY=33554432
EMXXFLAGS:=$(EMCFLAGS)
LDFLAGS:=

JSFUNCTIONS:=['_main','_setRadius','_setPrecision','_updateModel','_updateProjection','_getProjection','_mouseDrag','_mouseScroll','cwrap']

DATABASE:=
BINARIES:=$(filter-out %-gl %-json,$(basename $(notdir $(wildcard src/*.cpp))))
GLBINARIES:=$(filter %-gl,$(basename $(notdir $(wildcard src/*.cpp))))
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

# meta rules for javascript
js: $(JSBINARIES)

# meta rules for OpenGL
gl: $(GLBINARIES)
install-gl: $(IGLBINARIES)
uninstall-gl:
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

%-gl: src/%-gl.cpp include/*/*.h
	$(CXX) -std=c++0x -Iinclude/ $(CXXFLAGS) $(PCCFLAGS) $(GLCFLAGS) $< $(LDFLAGS) $(PCLDFLAGS) $(GLLDFLAGS) -o $@ && ($(DEBUG) || strip -x $@)

%.js: src/%.cpp include/*/*.h
	$(EMXX) -std=c++0x -Iinclude/ -D NOLIBRARIES $(EMXXFLAGS) -s EXPORTED_FUNCTIONS="$(JSFUNCTIONS)" $< $(LDFLAGS) -o $@
