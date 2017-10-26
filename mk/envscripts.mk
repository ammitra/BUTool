PREFIX?=./build
PREFIX:=$(abspath $(PREFIX))

# =======
define sourcecheck
if [[ $$_ == $$0 ]]; then  
  echo "$$0 is meant to be sourced:"
  echo "  source $$0"
  exit 0
fi

endef
# =======
export sourcecheck

# =======
define buildenv
export PREFIX="$(PREFIX)"
export MAKEFLAGS="-I $(PWD)/mk"
export CPPFLAGS="-isystem $(PREFIX)/include"
export LDFLAGS="-L$(PREFIX)/lib"
endef
# =======
export buildenv

# =======
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

buildenv.sh: Makefile
	@echo $@
	@echo "$$sourcecheck" > $@
	@echo "$$buildenv" >> $@
