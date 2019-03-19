if [[ $_ == $0 ]]; then  
  echo "$0 is meant to be sourced:"
  echo "  source $0"
  exit 0
fi

export PATH="$PATH:$PWD/bin/tool"
export BUTOOL_PATH="$PWD"
