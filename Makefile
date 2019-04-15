CFLAGS=-g3 -std=c++11
INCLUDE= -I./include
SOURCES=epoll.cpp Util.cpp httpData.cpp server.cpp
CXX=g++
LINKER=-lpthread
SRCDIR:= ./src
OBJ:= main.o $(SRCDIR)/epoll.o $(SRCDIR)/Util.o $(SRCDIR)/httpData.o $(SRCDIR)/server.o

TEST=$(OBJ:.o=.d)
all:$(OBJ)
	$(CXX) $(CFLAGS) $(INCLUDE)  -o $@ $^ $(LINKER)
#$(SRCDIR)/%.o:$(SRCDIR)/%.cpp 
#	$(CXX) $(CFLAGS) $(INCLUDE) -c -o $@ $<
%.o:%.cpp 
	$(CXX) $(CFLAGS) $(INCLUDE) -MD -c -o $@ $<
#main.o:main.cpp 
#	$(CXX) $(CFLAGS) $(INCLUDE)  -c -o $@ $<
OBJ1:
	echo $(OBJ)
TEST1:
	echo $(TEST)
clean:
	rm *.o
	rm src/*.o
	rm *.d
	rm src/*.d
-include $(OBJ:.o=.d)
