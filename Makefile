CXX = g++
CXXFLAGS = -Wall -Wextra -pedantic -std=c++17

client:
	$(CXX) $(CXXFLAGS) -o client ./clientFolder/client.h ./clientFolder/*.cc

server:
	$(CXX) $(CXXFLAGS) -o server ./serverFolder/server.h ./serverFolder/*.cc

all: client server

clean:
	rm -f client server