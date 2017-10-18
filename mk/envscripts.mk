ifeq ($(PREFIX),)
$(error Choose a PREFIX before creating source scripts)
endif

define sourcecheck :=
if [[ $$_ == $$0 ]]; then  
  echo "$$0 is meant to be sourced:"
  echo "  source $$0"
  exit 0
fi

endef
export sourcecheck

define env_sh :=
export PREFIX="$(PREFIX)"
export MAKEFLAGS="-I $(PWD)/mk"
endef
export env_sh

define useprefix_sh :=
# Adds PREFIX to executable path, library path, and compiler flags

if [[ -z $$PREFIX ]]; then
  echo "PREFIX must be set before sourcing this script"
  return 0
fi

export CPPFLAGS="-isystem $$PREFIX/include"
export LDFLAGS="-L$$PREFIX/lib"
export LD_LIBRARY_PATH="$$PREFIX/lib"
export PATH="$$PATH:$$PREFIX/bin"
endef
export useprefix_sh

envscripts: env.sh useprefix.sh

env.sh: Makefile
	@echo $@
	@echo "$$sourcecheck" > $@
	@echo "$$env_sh" >> $@

useprefix.sh: Makefile
	@echo $@
	@echo "$$sourcecheck" > $@
	@echo "$$env_sh" >> $@
	@echo "$$useprefix_sh" >> $@
