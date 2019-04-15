CFLAGS=-g3 -std=c++11
INCLUDE= -I./include
CXX=g++
LINKER=-lpthread
SRCDIR:= ./src
OBJ:= main.o $(SRCDIR)/epoll.o $(SRCDIR)/Util.o $(SRCDIR)/httpData.o $(SRCDIR)/server.o

all:$(OBJ)
	$(CXX) $(CFLAGS) $(INCLUDE)  -o $@ $^ $(LINKER)
%.o:%.cpp 
	$(CXX) $(CFLAGS) $(INCLUDE) -MD -c -o $@ $<
clean:
	rm *.o
	rm src/*.o
	rm *.d
	rm src/*.d
-include $(OBJ:.o=.d)
