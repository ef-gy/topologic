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

