-include platform/$(PLATFORM)/env.mk.inc

PROJECT_ROOT?=${CURDIR}

ifdef INSTALL_HERE
PREFIX?=.
PREFIX:=$(abspath $(PREFIX))
LIBDIR?=${CURDIR}/lib
BINDIR?=${CURDIR}/bin
endif

# ======= sourcecheck
define sourcecheck
if [[ $$_ == $$0 ]]; then  
  echo "$$0 is meant to be sourced:"
  echo "  source $$0"
  exit 0
fi

endef
# =======
export sourcecheck

# ======= buildenv base
define buildenv
export PLATFORM="$(PLATFORM)"
export PREFIX="$(PREFIX)"
export PROJECT_ROOT="$(PROJECT_ROOT)"
endef

# ======= buildenv append MAKEFLAGS
ifdef SET_MAKEFLAGS
define buildenv +=

export MAKEFLAGS="-I $(PWD)/mk"
endef
endif


# ======= buildenv append LIBDIR
ifneq ($(LIBDIR),)
define buildenv +=

export LIBDIR="$(LIBDIR)"
endef
endif

# ======= buildenv append BINDIR
ifneq ($(BINDIR),)
define buildenv +=

export BINDIR="$(BINDIR)"
endef
endif

# ======= buildenv (run)
export buildenv

# ======= runenv
define runenv
export PATH="$$PATH:$(PREFIX)/bin"
endef
# ======= runenv (run)
export runenv

.DEFAULT_GOAL=env.sh

envscripts: env.sh buildenv.sh

env.sh: Makefile
	@echo $@
	@echo "$$sourcecheck" > $@
	@echo "$$buildenv" >> $@
	@echo "$$runenv" >> $@
	@echo "$$useprefix_sh" >> $@
ifneq ($(realpath platform/${PLATFORM}/env.sh.inc),)
	@cat "platform/${PLATFORM}/env.sh.inc" >> $@
endif

buildenv.sh: Makefile
	@echo $@
	@echo "$$sourcecheck" > $@
	@echo "$$buildenv" >> $@
