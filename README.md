Ubuntu: [![Ubuntu Build Status](https://travis-ci.org/capnproto/capnproto.svg?branch=master)](https://travis-ci.org/capnproto/capnproto)
# simpleHttpWebServer
This is a simple http web server project with  thread pool mechanism,supporting simple request like HEAD,GET;

## Principle:
Process has a main thread and a thread pool,user can specify the number of threads in thread pool through command input.


* main thread duty:

      wait interested events happen,and once any event happened,the main thread get activated.
      if it is a connection request,the main thread will accept it,adding it to interested events
      for monitor;
      if it is a event epollin reading avaiable,the main thread will read all the arrived input data,
      and then push a task to queue for thread-pool taking over handling analyze the receive data and 
      reply back to request.
* thread-pool duty:

       dealing with the queue task,i.e. analyze the received data and reply back to client request, 
       this project using a buffer of unorder_map<int,string> to store all the current connection buffers,
       this buffer shared by all the threads,mutex lock  is used to prevent unexpected asynchrous modification.


## Build project :
```
make all
```
## Run
run example(p:port number,t:thread number):
```
./all -p 8888 -t 4
```

pressure test tool[WebBench](https://github.com/linyacool/WebBench):<br/>
c:client number t:time (seconds) 2:http1.1 k:Keep-Live
```
./bin/webbench -t 60 -c 1000 -2 -k --get  http://127.0.0.1:8888/hello
```

## Test 1
  *Test scenario*: with client number fixed to 1000,and all clients keep alive up to 60 seconds,change the number of threads in server thread-pool,to compare the performance when use different number of threads in handling request. <br/>

* __case 1:__ <br/>
server(2 threads in thread-pool):
```
./all -p 8888 -t 2
```
client(1000 clients Keep-live 60 secs):
```
./bin/webbench -t 60 -c 1000 -2 -k --get  http://127.0.0.1:8888/hello
```
![alt text](https://github.com/anson0/simpleHttpWebServer/blob/master/testPictures/test_2_1k.png)
![alt text](https://github.com/anson0/simpleHttpWebServer/blob/master/testPictures/test_2_1k_result.png)

***
* __case 2:__<br/>
server(4 threads in thread-pool):
```
./all -p 8888 -t 4
```
client(1000 clients Keep-live 60 secs):
```
./bin/webbench -t 60 -c 1000 -2 -k --get  http://127.0.0.1:8888/hello
```
![alt text](https://github.com/anson0/simpleHttpWebServer/blob/master/testPictures/test_4_1k.png)
![alt text](https://github.com/anson0/simpleHttpWebServer/blob/master/testPictures/test_4_1k_result.png)

***
* __case 3:__<br/>
server(6 threads in thread-pool):
```
./all -p 8888 -t 6
```
client(1000 clients Keep-live 60 secs):
```
./bin/webbench -t 60 -c 1000 -2 -k --get  http://127.0.0.1:8888/hello
```
![alt text](https://github.com/anson0/simpleHttpWebServer/blob/master/testPictures/test_6_1k.png)
![alt text](https://github.com/anson0/simpleHttpWebServer/blob/master/testPictures/test_6_1k_result.png)

***
* __case 4:__<br/>
server(8 threads in thread-pool):
```
./all -p 8888 -t 8
```
client(1000 clients Keep-live 60 secs):
```
./bin/webbench -t 60 -c 1000 -2 -k --get  http://127.0.0.1:8888/hello
```
![alt text](https://github.com/anson0/simpleHttpWebServer/blob/master/testPictures/test_8_1k.png)
![alt text](https://github.com/anson0/simpleHttpWebServer/blob/master/testPictures/test_8_1k_result.png)




## Test 2
  *Test scenario*: with client number fixed to 10000,and all clients keep alive up to 60 seconds,change the number of threads in server thread-pool,to compare the performance when use different number of threads in handling request. <br/>

* __case 5:__ <br/>
server(2 threads in thread-pool):
```
./all -p 8888 -t 2
```
client(10000 clients Keep-live 60 secs):
```
./bin/webbench -t 60 -c 10000 -2 -k --get  http://127.0.0.1:8888/hello
```
![alt text](https://github.com/anson0/simpleHttpWebServer/blob/master/testPictures/test_2_10k.png)
![alt text](https://github.com/anson0/simpleHttpWebServer/blob/master/testPictures/test_2_10k_result.png)

***
* __case 6:__<br/>
server(4 threads in thread-pool):
```
./all -p 8888 -t 4
```
client(10000 clients Keep-live 60 secs):
```
./bin/webbench -t 60 -c 10000 -2 -k --get  http://127.0.0.1:8888/hello
```
![alt text](https://github.com/anson0/simpleHttpWebServer/blob/master/testPictures/test_4_10k.png)
![alt text](https://github.com/anson0/simpleHttpWebServer/blob/master/testPictures/test_4_10k_result.png)

***
* __case 7:__<br/>
server(6 threads in thread-pool):
```
./all -p 8888 -t 6
```
client(10000 clients Keep-live 60 secs):
```
./bin/webbench -t 60 -c 10000 -2 -k --get  http://127.0.0.1:8888/hello
```
![alt text](https://github.com/anson0/simpleHttpWebServer/blob/master/testPictures/test_6_10k.png)
![alt text](https://github.com/anson0/simpleHttpWebServer/blob/master/testPictures/test_6_10k_result.png)

***
* __case 8:__<br/>
server(8 threads in thread-pool):
```
./all -p 8888 -t 8
```
client(10000 clients Keep-live 60 secs):
```
./bin/webbench -t 60 -c 10000 -2 -k --get  http://127.0.0.1:8888/hello
```
![alt text](https://github.com/anson0/simpleHttpWebServer/blob/master/testPictures/test_8_10K.png)
![alt text](https://github.com/anson0/simpleHttpWebServer/blob/master/testPictures/test_8_10k_result.png)


## Test Result
Test case 1-4 are 1000 clients keep live and sending data for 1 min,the result shows thread number 4 is optimal among<br/> 2,4,6,8 threads scenerio in thread-pool;<br/>
Test case 5-8 are 10000 clients keep live and sending data for 1 min,the result shows thread number 2 is optimal among<br/> 2,4,6,8 threads scenerio in thread-pool;<br/>
Through test case 5-8,the result obviously show increase the number of threads downgrade the performance greatly when there is huge number of clients long connection and keeping sending request.



### Main techniques involved in this project:
* C++11 thread,mutex,lock,smart pointers,lambda 
* stl 
* thread pool
* asynchronous
* IO multiplex epoll level trigger



 


