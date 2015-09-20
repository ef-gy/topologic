-include ef.gy/base.mk include/ef.gy/base.mk

NAME:=topologic
VERSION:=8

.third-party/libefgy/include/ef.gy/base.mk:
	mkdir .third-party || true
	cd .third-party && git clone git://github.com/ef-gy/libefgy.git

include/ef.gy/base.mk: .third-party/libefgy/include/ef.gy/base.mk
	ln -sfn ../.third-party/libefgy/include/ef.gy include/ef.gy

LIBRARIES:=libxml-2.0
FRAMEWORKS:=

JQUERY:=https://code.jquery.com/jquery-2.1.1.min.js
JQUERYMOBILE:=https://code.jquery.com/mobile/1.4.4/jquery.mobile-1.4.4.min.js
JQUERYMOBILECSS:=https://code.jquery.com/mobile/1.4.4/jquery.mobile-1.4.4.min.css

ifeq ($(UNAME),Darwin)
PCCFLAGS:=-I/usr/include/libxml2
PCLDFLAGS:=-lxml2 $(addprefix -framework ,$(FRAMEWORKS))
endif
CXXFLAGS:=$(CFLAGS) -fno-exceptions
EMCFLAGS:=-O3 --llvm-lto 3 -fno-exceptions -s TOTAL_MEMORY=67108864 -s OUTLINING_LIMIT=20480 -DNOVAO -DWEBGL -s ASSERTIONS=1 --memory-init-file 0
EMXXFLAGS:=$(EMCFLAGS)

JSFUNCTIONS:=['_main','_interpretDrag','_setActiveDimension','_forceRedraw','_setFlameColouring','_resetColourMap','_setViewportSize','_getJSON','_getSVG','_getArgs','_parseJSON','_parseArgs','_getModels','_initialiseGL']

BINARIES:=$(filter-out %-sdl,$(basename $(notdir $(wildcard src/*.cpp)))) $(addprefix test-case-,$(notdir $(wildcard src/test-case/*.cpp)))

%.html: src/%.cpp include/*/*.h
	$(EMXX) -std=c++0x -Iinclude/ -D NOLIBRARIES $(EMXXFLAGS) $< $(LDFLAGS) -o $@

$(DOWNLOADS)/jquery.js: $(DOWNLOADS)/.volatile
	$(CURL) '$(JQUERY)' -o $@

$(DOWNLOADS)/jquery.mobile.js: $(DOWNLOADS)/.volatile
	$(CURL) '$(JQUERYMOBILE)' -o $@

$(DOWNLOADS)/jquery.mobile.css: $(DOWNLOADS)/.volatile
	$(CURL) '$(JQUERYMOBILECSS)' -o $@

topologic-web.js: src/web/setup.js topologic-sdl.js src/web/glue.js
	cat $^ > $@

topologic-web.min.js: topologic-web.js
	uglifyjs $^ -o $@ -c -m

topologic-web.js.xml: topologic-web.js
	echo "<script type='text/javascript' xmlns='http://www.w3.org/1999/xhtml'><![CDATA[" > $@
	cat $^ >> $@
	echo "]]></script>" >> $@

topologic-web.css: $(DOWNLOADS)/jquery.mobile.css src/web/topologic.css
	cat $^ | $(CSSMIN) > $@

topologic-web.css.xml: topologic-web.css
	echo "<style xmlns='http://www.w3.org/1999/xhtml'><![CDATA[" > $@
	cat $^ >> $@
	echo "]]></style>" >> $@

topologic-web.html: src/web/topologic.xhtml xslt/web-prepare.xslt topologic-web.js.xml topologic-web.css.xml
	$(XSLTPROC) --stringparam root "$$(pwd)" -o "$@" xslt/web-prepare.xslt $<

src/chrome/jquery-2.1.1.min.js:
	curl -q -s https://code.jquery.com/jquery-2.1.1.min.js -o $@

src/chrome/jquery.mobile-1.4.4.min.js:
	curl -q -s https://code.jquery.com/mobile/1.4.4/jquery.mobile-1.4.4.min.js -o $@

src/chrome/topologic.js: topologic-web.js src/chrome/jquery-2.1.1.min.js src/chrome/jquery.mobile-1.4.4.min.js
	cp $< $@

src/chrome/topologic.html: src/web/topologic.xhtml xslt/chrome-prepare.xslt src/chrome/topologic.js topologic-web.css.xml
	$(XSLTPROC) --stringparam root "$$(pwd)" -o "$@" xslt/chrome-prepare.xslt $<

src/chrome/topologic-128.png: $(ICON)
	rsvg -w 128 -a $^ $@ || true

src/chrome/topologic-16.png: $(ICON)
	rsvg -w 16 -a $^ $@ || true

chrome-app.zip: src/chrome/manifest.json src/chrome/topologic.html src/chrome/topologic.js src/chrome/jquery-2.1.1.min.js src/chrome/jquery.mobile-1.4.4.min.js src/chrome/background.js src/chrome/topologic-128.png src/chrome/topologic-16.png
	zip -9jo $@ $^

