CC:=clang
CXX:=clang++
PKGCONFIG:=pkg-config

LIBRARIES:=libxml-2.0

DEBUG:=false

PCCFLAGS:=$(shell $(PKGCONFIG) --cflags $(LIBRARIES))
PCLDFLAGS:=$(shell $(PKGCONFIG) --libs $(LIBRARIES))
CFLAGS:=-O2 $(shell if $(DEBUG); then echo '-g'; fi)
CXXFLAGS:=$(CFLAGS)
LDFLAGS:=

DATABASE:=
BINARIES:=$(basename $(notdir $(wildcard src/*.cpp)))

# don't delete intermediary files
.SECONDARY:

# meta rules
all: $(DATABASES) $(BINARIES)
clean:
	rm -f $(DATABASES) $(BINARIES); true
scrub: clean

# pattern rules for databases
%.sqlite3: src/%.sql
	rm -f $@ && $(SQLITE3) $@ < $<

# pattern rules for C++ code
%: src/%.cpp include/*/*.h
	$(CXX) -std=c++0x -Iinclude/ $(CXXFLAGS) $(PCCFLAGS) $< $(LDFLAGS) $(PCLDFLAGS) -o $@ && ($(DEBUG) || strip -x $@)

test: cf
	./cf
