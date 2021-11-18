
include MakefileSTD/MakefileSTD

AR = ar
CXX = g++
CXXFLAGS = -flto -pipe -std=c++20 -pedantic -Wall -IuSockets/src
LIBS=
LIBFILE=libicon4.a

ifeq ($(DEBUG),1)
	CXXFLAGS += -ggdb -g -pg
else
	CXXFLAGS += -Ofast -s
endif

ifeq ($(platform),win)
	LIBS += -lzlib
	LIBS += -lgcc_s_seh-1 -lmfc100
	LIBS += -lmfc100u -lmsvcp100 -lmsvcr100 -lmsvcr100_clr0400
else
	LIBS += -L/usr/lib -ldl -lz
	CXXFLAGS += -fPIC
	CXXFLAGS += -I/usr/include
endif
LIBS += -lpthread -lssl -lcrypto

INCLUDE = -I./src/
CXXFLAGS += $(INCLUDE)
OBJECTS =bin/asio.o bin/socket.o bin/acceptor.o
OBJECTS+=bin/tcp_socket_impl.o bin/tcp_acceptor_impl.o
OBJECTS+=bin/ssl_socket_impl.o bin/ssl_acceptor_impl.o

all: $(LIBFILE)

$(LIBFILE): $(OBJECTS) uSockets/uSockets.a
	$(AR) rvs $@ $^

uSockets/uSockets.a:
	make uSockets/Makefile

bin/%.o: src/%.cpp src/%.hpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

.PHONY: clean
clean:
	(cd uSockets ; make clean)
	$(RM) $(OBJECTS) $(LIBFILE)

