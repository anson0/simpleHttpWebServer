OBJ=test.o test2.o main.o
all:$(OBJ)
	g++ -o $@ $^
%.o:%.cpp
	g++ -MD -o $@ -c $<
OBJ1:$(OBJ)

clean:
	rm *.o
	rm *.d
-include test.d test2.d
