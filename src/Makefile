
COMPILER := g++
OPTS := -std=c++20
BIN := nibi

SOURCES := \
	atomiser.cpp \
	main.cpp

COMPILE := $(COMPILER) $(OPTS) $(SOURCES) -o $(BIN) -lfmt

all:
	$(COMPILE) -O3
debug:
	$(COMPILE) -g

clean:
	rm $(BIN)

