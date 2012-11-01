CXX = clang++ -std=c++11
CXXFLAGS = -Wall -g -D SOCKETSESSIONHEADER_UNIT_TEST
INCLUDES = 
LIBS =
OBJS = SocketSessionHeader.o
PROGRAM = SocketSessionHeader.out

all:$(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ $(INCLUDES) $(LIBS) -o $(PROGRAM)

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LIBS) -c $<

.PHONY: clean
clean:
	rm -f *o $(PROGRAM)
