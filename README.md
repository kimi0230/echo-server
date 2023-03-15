# Echo Server using C

## Build Code
```sh
make build
```

## Start server
```sh
./server
```


# Notes
### TCP socket select
[TCP socket select用法分析](https://blog.csdn.net/u014530704/article/details/72833186)

`int select( int nfds, fd_set FAR* readfds,　fd_set * writefds, fd_set * exceptfds,　const struct timeval * timeout)`
參數含義：
1. nfds：是一個整數值，是指集合中所有文件描述符的範圍，即所有文件描述符的最大值加1，不能錯！在Windows中這個參數的值無所謂，可以設置不正確。
2. readfds：（可選）指針，指向一組等待可讀性檢查的套接口。
3. writefds：（可選）指針，指向一組等待可寫性檢查的套接口。
4. exceptfds：（可選）指針，指向一組等待錯誤檢查的套接口。
5. timeout：select()最多等待時間，對阻塞操作則為NULL。

返回值：
select()調用返回處於就緒狀態並且已經包含在fd_set結構中的描述字總數；如果超時則返回0；否則的話，返回SOCKET_ERROR錯誤，應用程序可通過WSAGetLastError獲取相應錯誤代碼。
1. 當返回為-1時，所有描述符集清0。
2. 當返回為0時，表示超時。
3. 當返回為正數時，表示已經準備好的描述符數。
select（）返回後，在3個描述符集裡，依舊是1的位就是準備好的描述符。這也就是為什麼，每次用select後都要用FD_ISSET的原因。
select函數實現I/O多路復用，可以用來監視多個描述符，之後我們調用FD_ISSET函數確定具體是哪一個描述符準備好了。

### Backoff
wait_interval = base * multiplier^n
* base is the initial interval, ie, wait for the first retry
* n is the number of failures that have occurred
* multiplier is an arbitrary multiplier that can be replaced with any suitable
value

## htonl(), ntohl(), htons(), ntohs()
* htonl()--"Host to Network Long"
* ntohl()--"Network to Host Long"
* htons()--"Host to Network Short"
* ntohs()--"Network to Host Short"

# Reference
* https://www.geeksforgeeks.org/udp-client-server-using-connect-c-implementation/?ref=gcse
* [C++ : : NETWORKING : : THREADED ECHO SERVER](https://cppsecrets.com/users/2194110105107104105108981049711648504964103109971051084699111109/C00-Networking-Threaded-echo-server.php)
* [TCP socket select用法分析](https://blog.csdn.net/u014530704/article/details/72833186)
* [Better Retries with Exponential Backoff and Jitter](https://www.baeldung.com/resilience4j-backoff-jitter)
* [tornado常见socket报错](https://www.jianshu.com/p/a7762d8c9973)
* [htonl(), ntohl(), htons(), ntohs() 函数具体应用](https://blog.csdn.net/u010355144/article/details/44964181)