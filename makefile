CXX := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Iinclude -pthread

SRC := src/logger.cpp src/logger_test.cpp
OBJ := $(SRC:.cpp=.o)

LIB := libtslog.a
TEST := tslog_test

.PHONY: all clean

all: $(LIB) $(TEST)

$(LIB): src/logger.o
	@ar rcs $@ $^

src/logger.o: src/logger.cpp include/logger.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TEST): src/logger_test.o $(LIB)
	$(CXX) $(CXXFLAGS) $< -L. -ltslog -o $@

src/logger_test.o: src/logger_test.cpp include/logger.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(LIB) $(TEST) tslog_output.log
