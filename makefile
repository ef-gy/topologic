DESTDIR:=
PREFIX:=/usr/local
BINDIR:=$(DESTDIR)$(PREFIX)/bin
INCLUDEDIR:=$(DESTDIR)$(PREFIX)/include
MANDIR:=$(DESTDIR)$(PREFIX)/share/man

NAME:=topologic
VERSION:=10

CC:=clang
CXX:=clang++
EMXX:=em++
PKGCONFIG:=pkg-config
INSTALL:=install
XSLTPROC:=xsltproc
CURL:=curl -s
CSSMIN:=cssmin

UNAME:=$(shell uname)
LIBRARIES:=libxml-2.0
LIBRARIES_GL:=libxml-2.0 glew
FRAMEWORKS:=
FRAMEWORKS_GL:=GLUT OpenGL Cocoa

DEBUG:=false

DOWNLOADS:=.downloads
JQUERY:=https://code.jquery.com/jquery-1.11.1.min.js
JQUERYMOBILE:=https://code.jquery.com/mobile/1.4.3/jquery.mobile-1.4.3.min.js
JQUERYMOBILECSS:=https://code.jquery.com/mobile/1.4.3/jquery.mobile-1.4.3.min.css

ifneq ($(UNAME),Darwin)
PCCFLAGS:=$(shell $(PKGCONFIG) --cflags $(LIBRARIES) 2>/dev/null)
PCLDFLAGS:=$(shell $(PKGCONFIG) --libs $(LIBRARIES) 2>/dev/null)
GLCFLAGS:=$(shell $(PKGCONFIG) --cflags $(LIBRARIES_GL) 2>/dev/null)
GLLDFLAGS:=$(shell $(PKGCONFIG) --libs $(LIBRARIES_GL) 2>/dev/null) -lglut -lGL
else
PCCFLAGS:=-I/usr/include/libxml2
PCLDFLAGS:=-lxml2 $(addprefix -framework ,$(FRAMEWORKS))
GLCFLAGS:=
GLLDFLAGS:=$(addprefix -framework ,$(FRAMEWORKS_GL))
endif
CFLAGS:=-O2 $(shell if $(DEBUG); then echo '-g'; fi)
CXXFLAGS:=$(CFLAGS) -fno-exceptions
EMCFLAGS:=-O3 --llvm-lto 3 -fno-exceptions -s TOTAL_MEMORY=67108864 -s OUTLINING_LIMIT=20480 -DNOVAO -DWEBGL --closure 1
EMXXFLAGS:=$(EMCFLAGS)
LDFLAGS:=

JSFUNCTIONS:=['_main','_setRadius','_setConstant','_setPrecision','_updateModel','_updateProjection','_getProjection','_interpretDrag','_setActiveDimension','cwrap','_forceRedraw','_setIFSParameters','_setFlameColouring','_setColour','_setFlameParameters','_resetColourMap','_setViewportSize','_getJSON','_getSVG','_parseJSON']

DATABASE:=
BINARIES:=$(filter-out %-glut %-gles %-sdl,$(basename $(notdir $(wildcard src/*.cpp)))) $(addprefix test-case-,$(notdir $(wildcard src/test-case/*.cpp)))
GLBINARIES:=$(filter %-glut,$(basename $(notdir $(wildcard src/*.cpp))))
JSBINARIES:=$(addsuffix .js,$(BINARIES))
TESTBINARIES:=$(filter test-case-%,$(BINARIES))

IGNOREBINARIES:=
IBINARIES:=$(addprefix $(BINDIR)/,$(filter-out $(IGNOREBINARIES) test-case-%,$(BINARIES)))
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
documentation documentation/xml/combine.xslt documentation/xml/index.xml: doxyfile include/*/* xslt/doxy*
	doxygen $<

documentation.xml: documentation/xml/combine.xslt documentation/xml/index.xml xslt/doxygen*
	xsltproc documentation/xml/combine.xslt documentation/xml/index.xml | xsltproc xslt/doxygen-post-process.xslt - > $@

# meta rules for javascript
js: $(JSBINARIES)

# meta rules for OpenGL
gl: $(GLBINARIES)
install-gl: $(IGLBINARIES)
uninstall-gl:
	rm -f $(IGLBINARIES)

# run unit tests
test: $(addprefix run-,$(TESTBINARIES))

run-test-case-%: test-case-%
	@echo TEST BATCH: $*
	@./$^
	@echo PASSED

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

# downloads
$(DOWNLOADS)/.volatile:
	mkdir -p $(DOWNLOADS) || true
	touch $@

$(DOWNLOADS)/jquery.js: $(DOWNLOADS)/.volatile
	$(CURL) '$(JQUERY)' -o $@

$(DOWNLOADS)/jquery.mobile.js: $(DOWNLOADS)/.volatile
	$(CURL) '$(JQUERYMOBILE)' -o $@

$(DOWNLOADS)/jquery.mobile.css: $(DOWNLOADS)/.volatile
	$(CURL) '$(JQUERYMOBILECSS)' -o $@

# merge instructions
topologic-web.js: $(DOWNLOADS)/jquery.js $(DOWNLOADS)/jquery.mobile.js src/web/setup.js topologic-sdl.js src/web/glue.js
	cat $^ > $@

topologic-web.css: $(DOWNLOADS)/jquery.mobile.css src/web/topologic.css
	cat $^ | $(CSSMIN) > $@

topologic-web.css.xml: topologic-web.css
	echo "<style xmlns='http://www.w3.org/1999/xhtml'><![CDATA[" > $@
	cat $^ >> $@
	echo "]]></style>" >> $@

topologic-web.html: src/web/topologic.xhtml xslt/web-prepare.xslt topologic-web.css.xml
	$(XSLTPROC) --stringparam root "$$(pwd)" -o "$@" xslt/web-prepare.xslt $<
