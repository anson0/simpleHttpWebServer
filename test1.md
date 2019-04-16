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
![alt text](https://github.com/anson0/simpleHttpWebServer/blob/master/testPictures/test_8_1k_esult.png)



## Test 2
  *Test scenario*: with client number fixed to 10000,and all clients keep alive up to 60 seconds,change the number of threads in server thread-pool,to compare the performance when use different number of threads in handling request. <br/>

* __case 1:__ <br/>
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
* __case 2:__<br/>
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
* __case 3:__<br/>
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
* __case 4:__<br/>
server(8 threads in thread-pool):
```
./all -p 8888 -t 8
```
client(10000 clients Keep-live 60 secs):
```
./bin/webbench -t 60 -c 10000 -2 -k --get  http://127.0.0.1:8888/hello
```
![alt text](https://github.com/anson0/simpleHttpWebServer/blob/master/testPictures/test_8_10k.png)
![alt text](https://github.com/anson0/simpleHttpWebServer/blob/master/testPictures/test_8_10k_result.png)
