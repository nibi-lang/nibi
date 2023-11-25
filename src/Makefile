
GIT_HASH := $(shell git rev-parse HEAD | cut -c 1-8)

COMPILER := g++
OPTS := -std=c++20 -DBUILD_HASH="\"$(GIT_HASH)\""
BIN := nibi

FRONT_SOURCES := \
	front/atoms.cpp \
	front/atomizer.cpp \
	front/verify_builtin_form.cpp

RUNTIME_SOURCES := \
	runtime/object.cpp \
	runtime/env.cpp \
	runtime/core.cpp

SOURCES := \
	$(FRONT_SOURCES) \
	$(RUNTIME_SOURCES) \
	nibi.cpp

INCLUDES := \
	-I ./ \

COMPILE := $(COMPILER) $(OPTS) $(SOURCES) ${INCLUDES} -o $(BIN) -lfmt

all:
	$(COMPILE) -O3
debug:
	$(COMPILE) -g -DNIBI_DEBUG=1

clean:
	rm nibi
	rm -rf nibi.dSYM/

