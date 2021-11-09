
include ../MakefileSTD/MakefileSTD


CXX = g++
CXXFLAGS = -ggdb3 -m64 -pipe -std=c++20 -pedantic -Wall
LIBS=

ifeq ($(platform),win)
	LIBS += -lzlib
	LIBS += -lgcc_s_seh-1 -lmfc100
	LIBS += -lmfc100u -lmsvcp100 -lmsvcr100 -lmsvcr100_clr0400
else
	LIBS += -L/usr/lib -ldl -lz
	CXXFLAGS += -fPIC
	CXXFLAGS += -I/usr/include
endif
LIBS += -lpthread -lboost_system -lssl -lcrypto

INCLUDE = -I../src/
CXXFLAGS += $(INCLUDE)
OBJECTS=bin/socket.o bin/asio.o bin/tcp.o

EXE = tcp_socket.exe

all: $(EXE) $(OBJECTS)

%.exe: bin/%.o $(OBJECTS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

bin/%.o: ../src/%.cpp
	$(CXX) -c -o $@ $^ $(CXXFLAGS)

bin/%.o: %.cpp
	$(CXX) -c -o $@ $^ $(CXXFLAGS)

.PHONY: clean
clean:
	$(RM) *.exe *.o bin/*.o

