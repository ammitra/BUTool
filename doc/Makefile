#SEARCH_PATHS = $(find ../ -path "../*src" -o -path "../*include")
SEARCH_PATHS = "INPUT="$(wildcard  ../*src  ../*include ../*/*/src ../*/*/include)
.PHONY: doc clean

all: doc

clean:
	@echo "Cleaning up"
	@rm -rf ./html > /dev/null 2>&1

doc :
	@echo "Generating doxygen"
#	@echo ${SEARCH_PATHS}
	@(cat ./BUTool.cfg ; echo ${SEARCH_PATHS}) | doxygen -


