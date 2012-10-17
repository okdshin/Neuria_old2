CXX = g++ -std=gnu++0x
CXXFLAGS = -Wall -g -D MULTIPLETIMER_UNIT_TEST
INCLUDES = 
LIBS = -lcrypto -lboost_serialization -lboost_thread -ldl -lpthread -lboost_system
OBJS = MultipleTimer.o
PROGRAM = MultipleTimer.out

all:$(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ $(INCLUDES) $(LIBS) -o $(PROGRAM)

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LIBS) -c $<

.PHONY: clean
clean:
	rm -f *o $(PROGRAM)
