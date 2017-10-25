ifeq ($(PREFIX),)
$(error Choose a PREFIX before creating source scripts)
endif

define sourcecheck
if [[ $$_ == $$0 ]]; then  
  echo "$$0 is meant to be sourced:"
  echo "  source $$0"
  exit 0
fi

endef
export sourcecheck

# =======
define buildenv
export PREFIX="$(PREFIX)"
export MAKEFLAGS="-I $(PWD)/mk"
endef
# =======
export buildenv

# =======
define runenv
# Adds PREFIX to executable path, library path, and compiler flags

if [[ -z $$PREFIX ]]; then
  echo "PREFIX must be set before sourcing this script"
  return 0
fi

export CPPFLAGS="-isystem $$PREFIX/include"
export LDFLAGS="-L$$PREFIX/lib"
export PATH="$$PATH:$$PREFIX/bin"
endef
# =======
export runenv

envscripts: env.sh buildenv.sh

env.sh: Makefile
	@echo $@
	@echo "$$sourcecheck" > $@
	@echo "$$buildenv" >> $@
	@echo "$$runenv" >> $@
	@echo "$$useprefix_sh" >> $@

buildenv.sh: Makefile
	@echo $@
	@echo "$$sourcecheck" > $@
	@echo "$$buildenv" >> $@
