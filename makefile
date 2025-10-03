CXX := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Iinclude -pthread

# sources
LIB_SRC := src/logger.cpp
LIB_OBJ := src/logger.o
LIB := libtslog.a

SERVER_SRC := src/server.cpp src/client_handler.cpp src/server_main.cpp
CLIENT_SRC := src/client.cpp
SIM_SRC := src/sim_client.cpp

SERVER_OBJ := $(SERVER_SRC:.cpp=.o)
CLIENT_OBJ := $(CLIENT_SRC:.cpp=.o)
SIM_OBJ := $(SIM_SRC:.cpp=.o)

BIN_SERVER := server
BIN_CLIENT := client
BIN_SIM := sim_client

.PHONY: all clean

all: $(LIB) $(BIN_SERVER) $(BIN_CLIENT) $(BIN_SIM)

# lib
$(LIB): $(LIB_OBJ)
	ar rcs $@ $^

src/logger.o: src/logger.cpp include/logger.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

# server
$(BIN_SERVER): $(SERVER_OBJ) $(LIB)
	$(CXX) $(CXXFLAGS) -o $@ $(SERVER_OBJ) -L. -ltslog

src/server.o: src/server.cpp include/server.h include/client_handler.h include/logger.h
	$(CXX) $(CXXFLAGS) -c src/server.cpp -o src/server.o

src/client_handler.o: src/client_handler.cpp include/client_handler.h include/server.h include/logger.h
	$(CXX) $(CXXFLAGS) -c src/client_handler.cpp -o src/client_handler.o

src/server_main.o: src/server_main.cpp include/server.h include/logger.h
	$(CXX) $(CXXFLAGS) -c src/server_main.cpp -o src/server_main.o

# client
$(BIN_CLIENT): src/client.o $(LIB)
	$(CXX) $(CXXFLAGS) -o $@ src/client.o -L. -ltslog

src/client.o: src/client.cpp include/logger.h
	$(CXX) $(CXXFLAGS) -c src/client.cpp -o src/client.o

# sim_client
$(BIN_SIM): src/sim_client.o $(LIB)
	$(CXX) $(CXXFLAGS) -o $@ src/sim_client.o -L. -ltslog

src/sim_client.o: src/sim_client.cpp include/logger.h
	$(CXX) $(CXXFLAGS) -c src/sim_client.cpp -o src/sim_client.o

clean:
	rm -f src/*.o $(LIB) $(BIN_SERVER) $(BIN_CLIENT) $(BIN_SIM) *.log
