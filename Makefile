CXX:=$(shell sh -c 'type $(CXX) >/dev/null 2>/dev/null && echo $(CXX) || echo g++')
#CXX = clang++-3.8

INSTALL = install

INCLUDES =

OPTIMIZATION = -O0

WARNINGS =

DEBUG = -g

STDC = -std=c++1y

LDFLAGS =

CFLAGS=-Wall

CXXFLAGS = $(OPTIMIZATION) -fPIC -fstack-protector -pthread $(CFLAGS) $(WARNINGS) $(DEBUG) $(STDC) -DUSE_CXX0X

STDLIB =
ifeq ($(shell uname -s), FreeBSD)
STDLIB += -stdlib=libstdc++
endif
CXXFLAGS +=  $(STDLIB)

CLIENT_SOURCES = client.cpp \
		   Utils/Files.cpp \
		   Utils/Net.cpp \

SERVER_SOURCES = server.cpp \
		   Utils/Files.cpp \
		   Utils/Net.cpp \
		   Connection.cpp \
		   Acceptor.cpp \
		   AEvent/AEventAbstract.cpp

CLIENT_EXEC = client

SERVER_EXEC = server

all: $(CLIENT_EXEC) $(SERVER_EXEC)

$(CLIENT_EXEC): $(CLIENT_SOURCES)
	$(CXX) $(INCLUDES) $(CXXFLAGS)-o $@ $^ $(LDFLAGS)

$(SERVER_EXEC): $(SERVER_SOURCES)
	$(CXX) $(INCLUDES) $(CXXFLAGS)-o $@ $^ $(LDFLAGS)

clean: 
	rm -f $(CLIENT_EXEC)

