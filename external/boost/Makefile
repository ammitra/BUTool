package=boost
include package.gmk
include objdir.gmk

archive=boost_1_65_1.tar.bz2
sdir=boost_1_65_1

clean_source:
	rm -rf ${OBJDIR}/$(sdir) ${OBJDIR}/.extracted.mk

clean: clean_source

${OBJDIR}/.extracted.mk:
	@mkdir -p ${OBJDIR}
	tar xf $(archive) -C ${OBJDIR}
	touch $@
	touch Makefile

${OBJDIR}/$(sdir): ${OBJDIR}/.extracted.mk

extract: ${OBJDIR}/$(sdir)
ifneq ($(or $(if $(MAKECMDGOALS),,true),$(filter extract configure build install,$(MAKECMDGOALS))),)
include ${OBJDIR}/.extracted.mk
endif

CPPFLAGS+=-I${OBJDIR}/$(sdir)

.PHONY: extract configure build install clean_source

include generic.paths.gmk

name=boost
lib=boost
include:=${OBJDIR}/$(sdir)/boost
INCSRCDIR:=${OBJDIR}/$(sdir)
include generic.lib.gmk

include:=
name=boost_regex
lib=boost_regex
sources_cxx:=$(wildcard ${OBJDIR}/$(sdir)/libs/regex/src/*.cpp)
include generic.lib.gmk

pkg_name=boost
