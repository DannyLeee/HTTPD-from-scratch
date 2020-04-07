## Introduce
This repo will show you an example of how to build an http server with cgi from scratch. But it is a homework actually.

## Workflow
Host will get browser request and call the CGI program, then the CGI program will return the result to Host. And the Host will print the result to browser.

## Run
```
terminal:
    make
    ./Host
browser:
    127.0.0.1:8787 or localhost:8787
```


## Expected results
![](https://i.imgur.com/Dn6S4og.png)
```
$ ./Host
--------- Got client connection ---------
GET / HTTP/1.1
Host: 127.0.0.1:8787
Connection: keep-alive
Cache-Control: max-age=0
Upgrade-Insecure-Requests: 1
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/80.0.3987.149 Safari/537.36
Sec-Fetch-Dest: document
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9
Sec-Fetch-Site: cross-site
Sec-Fetch-Mode: navigate
Accept-Encoding: gzip, deflate, br
Accept-Language: zh-TW,zh;q=0.9,en-US;q=0.8,en;q=0.7,zh-CN;q=0.6
```

## How to use and what can do
* only can service particular GET and POST method.
* view the local file:
    * click `view`
* insert to local file:
    * type texts in the input box and select the method to server.

**If server shutdown without error message please restart ./Host**
(I really don't know why)
