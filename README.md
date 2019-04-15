# simpleHttpWebServer
this is a simple http web server demo with  thread pool mechanism;

principle:
Process has a main thread and a thread pool,user can specify the number of threads in thread pool through command input.
main thread duty:wait interested events happen,and once any event happened,the main thread get activated,if it is a connection
                 request,the main thread will accept it,adding it to interested events for monitor;if it is a event writing 
                 input buffer,the main thread will read all the arrived input data,and then push a task to queue for thread-pool
                 taking over handling analyze the receive data and reply back to request.
thread-pool duty:
                 dealing with the queue task,i.e. analyze the received data and reply back to client request,this project using
                 a buffer of unorder_map<int,string> to store all the current connection buffers,this buffer shared by all the 
                 threads,mutex lock  is used to prevent unexpected asynchrous modification.


build project:
make all

run example(p:port number,t:thread number):
./all -p 8888 -t 4

pressure test[PressureTestTool](https://github.com/linyacool/WebBench):
c:client number t:time (seconds) 2:http1.1 k:Keep-Live
./bin/webbench -t 60 -c 1000 -2 -k --get  http://127.0.0.1:8888/hello
running 1000 clients

Test1(1000 clients):
server(4 threads in thread-pool):./all -p 8888 -t 4
client(1000 clients Keep-live 60 secs):./bin/webbench -t 60 -c 1000 -2 -k --get  http://127.0.0.1:8888/hello
![alt text](https://github.com/anson0/simpleHttpWebServer/blob/master/cpuUseageMemory.png)
![alt text](https://github.com/anson0/simpleHttpWebServer/blob/master/toolResult.png)

Test1(10000 clients):
server(4 threads in thread-pool):./all -p 8888 -t 4
client(10000 clients Keep-live 60 secs):./bin/webbench -t 60 -c 10000 -2 -k --get  http://127.0.0.1:8888/hello
![alt text](https://github.com/anson0/simpleHttpWebServer/blob/master/cpuUsageMemory2.png)
![alt text](https://github.com/anson0/simpleHttpWebServer/blob/master/toolResult2.png)





 


