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

# ======= buildenv
define buildenv
export PLATFORM="$(PLATFORM)"
export PREFIX="$(PREFIX)"
export MAKEFLAGS="-I $(PWD)/mk"
export PROJECT_ROOT="$(PROJECT_ROOT)"
endef

ifneq ($(LIBDIR),)
define buildenv +=

export LIBDIR="$(LIBDIR)"
endef
endif

ifneq ($(BINDIR),)
define buildenv +=

export BINDIR="$(BINDIR)"
endef
endif

# =======
export buildenv

# ======= runenv
define runenv
export PATH="$$PATH:$(PREFIX)/bin"
endef
# =======
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
