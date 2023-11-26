
GIT_HASH := $(shell git rev-parse HEAD | cut -c 1-8)

COMPILER := g++
OPTS := -std=c++2a -DBUILD_HASH="\"$(GIT_HASH)\""
BIN := nibi

FRONT_SOURCES := \
	src/front/atoms.cpp \
	src/front/atomizer.cpp \
	src/front/verify_builtin_form.cpp

RUNTIME_SOURCES := \
	src/runtime/object.cpp \
	src/runtime/env.cpp \
	src/runtime/core.cpp \
	src/runtime/builtins/builtins.cpp \
	src/runtime/builtins/fn.cpp

SOURCES := \
	$(FRONT_SOURCES) \
	$(RUNTIME_SOURCES) \
	src/nibi.cpp

COMPILE := $(COMPILER) $(OPTS) $(SOURCES) -I ./src -o $(BIN) -lfmt

all:
	$(COMPILE)

debug:
	$(COMPILE) -g -DNIBI_DEBUG=1

test:
	cd tests/test_scripts; python3 run.py ../../nibi

clean:
	rm nibi
	rm -rf nibi.dSYM/

