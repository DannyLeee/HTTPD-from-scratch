<p style="text-align:right;">
姓名:李韋宗<br>
學號:B10615024<br>
日期:Apr. 8, 2020<br>
</p>

<h1 style="text-align:center;"> Netweork Security - HW1

## 建置環境
* WSL(Ubuntu 16.04 LST)
* VS code
* google Chrome

---

## 使⽤說明
* [README](https://github.com/DannyLeee/HTTPD-from-scratch/master/README.md)
* step1: 在目錄中執行 `make`
* step2: 在目錄中執行 `./Host`
* step3: 在瀏覽器中進入 127.0.0.1:8787 或 localhost:8787 ，即可進入首頁
* 使用時 terminal 將輸出每筆 request
* **如果使用時亦常中斷或連線緩慢，重開 ./Host 即可** (我也不知道為啥會這樣)

### 使用 view.cgi 瀏覽 DB.txt
* 點擊 view 連結
* 或在網址列加上 `/view.cgi`

### 使用 insert.cgi 寫入 DB.txt
* 在輸入欄位中鍵入文字後
* 依照需求點擊 request method
* 或在網址列加上 `/insert.cgi?data=` 並鍵入文字

---

## 程式碼設計架構與功能說明
### Host.cpp
#### socket 及變數初始化
* 當中的3個 `if` 皆為 error handling
* 當無法開啟 socket 、無法 bind address 、連線數大於10，拋出 error
    ```cpp
    int cgiOutput[2];
    int status;
    char* inputData;
    pid_t cpid;
    char c;

    /*socket initial*/
    // the address of the socket(internet format)
    struct sockaddr_in server_addr, client_addr;
    // length of cnt socket	// for the portability
    socklen_t sin_len = sizeof(client_addr);
    int fd_server , fd_client;
    char bfr[2048];	// storing the the content sent by the browser to server
    int on = 1;

    /* open a socket (AF_INET: Internet address. 
    SOCK_STREAM: Provides sequenced, reliable, bidirectional, 
    connection-mode byte streams, and may provide a transmission mechanism 
    for out-of-band data.) */
    fd_server = socket(AF_INET, SOCK_STREAM,0);
    if (fd_server < 0) {
        perror("socket");
        exit(1);
    }

    /*  set the socket property
    SOL_SOCKET: set options at the socket level
    SO_REUSEADDR: supplied to bind() should allow reuse of local addresses */
    setsockopt(fd_server, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));
    server_addr.sin_family = AF_INET;	// Address family
    server_addr.sin_addr.s_addr = INADDR_ANY;	// Internet address
    server_addr.sin_port = htons(8787);	// port num

    /* assigns an address to an unnamed socket
    2nd the address bind to the socket */
    if (bind(fd_server, (struct sockaddr *) &server_addr, 
    sizeof(server_addr)) == -1) {
        perror("bind");
        close(fd_server);
        exit(1);
    }

    // listen for socket connections and limit the queue of incoming connections
    if (listen(fd_server,10) == -1) {
        perror("Listen");
        close(fd_server);
        exit(1);
    }
    ```
#### 接收連線的無窮迴圈
* 先建立與 cgi 子程序的 pipe 負責溝通
* 接著使用 `accept` 建立連線
* `fork` 出子程序接手接下來的工作
    ```cpp
    while(1) {
        /* Use pipe to create a data channel betweeen two process
        'cgiOutput' handle data from 'CGI' to 'host'*/
        if(pipe(cgiOutput)<0) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }

        /* accept a new connection on a socket 
        做一個跟1st一樣的socket */
        fd_client = accept(fd_server, (struct sockaddr *) &client_addr,
        &sin_len);

        if (fd_client == -1) {
            perror("Connection failed\n");
            continue;
        }

        printf("--------- Got client connection ---------\n");

        /* Creates a new process to execute cgi program */
        if((cpid = fork()) < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
    ```
##### 子程序
* 先將 browser 傳送的 request 讀入 `bfr` 並在 terminal 中顯示
* 子程序將用不到的 pipe 口關閉
* 並將 STDOUT 轉入 pipe 讓 cgi 回傳至 host
* 解析 `bfr` 中的 request 並呼叫對應的 cgi 且傳入相應的值
* 其中，insert.cgi 值的傳遞方法是透過 `execlp` 的參數當 argv 傳入
    ```cpp
        /*child process*/
        if(cpid == 0) {
            memset(bfr,0,2048);	// reset buf
            read(fd_client, bfr, 2047);	// read 2047 bytes from fd to buf
            printf("%s\n", bfr);	// print request to terminal

            //close unused fd
            close(cgiOutput[0]);
            close(fd_server);	// child doesn't need
            // the STDOUT of CGI program is redirect to cgiOutput
            //redirect the output from stdout to cgiOutput
            dup2(cgiOutput[1],STDOUT_FILENO);
            //after redirect we don't need the old fd
            close(cgiOutput[1]);
            close(fd_client);

            if (strncmp(bfr,"GET /program.cgi",16) == 0) {
                // execute cgi program
                execlp("./program.cgi","./program.cgi",NULL);
            }
            else if (strncmp(bfr,"GET /view.cgi",13) == 0) {
            // execute cgi view
            execlp("./view.cgi","./view.cgi",NULL);
            }
            else if (strncmp(bfr, "GET /insert.cgi?data=", 21) == 0) {
                inputData = strtok(bfr, " ");       // GET
                inputData = strtok(NULL, " ");      // /insert.cgi?data=...
                inputData += 17;
                execlp("./insert.cgi", "./insert.cgi", inputData, NULL);
            }
            else if (strncmp(bfr, "POST /insert.cgi", 16) == 0) {
                std::string s = bfr;
                std::string::size_type n, m;
                n = s.find("data=");
                s = s.substr(n+5);
                execlp("./insert.cgi", "./insert.cgi", s.c_str(), NULL);
            }
            else {
                write(STDOUT_FILENO, webPage, sizeof (webPage) - 1);
            }
            exit(0);
        }
    ```
##### 父程序
* 關閉用不到的 pipe 口
* 讀出 cgi 在 STDOUT 中回傳的資料並輸出至 socket
* 關閉 pipe 及 socket 等待下次連線
    ```cpp
        /*parent process*/
        else {
            //close unused fd
            close(cgiOutput[1]);

            // receive the message from the  CGI program
            while (read(cgiOutput[0], &c, 1) > 0) {
                // output the message to terminal
                write(fd_client, &c, 1);
            }

            // connection finish
            close(cgiOutput[0]);
            close(fd_client);
        }
    }
    ```
    ---

### view.c
* 用唯獨模式開啟本地端檔案 DB.txt
* 輸出 respond 標頭
* 用 text/text格式輸出 DB.txt完整內容
    ```cpp
    #include <stdio.h>
    #include <error.h>
    #include <stdlib.h>

    int main(void){
        // open file
        FILE *fp = fopen("./DB.txt", "r");
        if (!fp) {
            perror("File opening failed");
            return EXIT_FAILURE;
        }

        // output to stdout
        printf("HTTP/1.1 200 OK\r\n");
        printf("Content-Type: text/text; charset=UTF-8\r\n\r\n");
        
        // FILE pointer
        int c; // note: int, not char, required to handle EOF
        while ((c = fgetc(fp)) != EOF) { // standard C I/O file reading loop
        putchar(c);
        }
        fclose(fp);
    }
    ```
    ---

### insert.c
* 以寫入模式開啟本地端檔案 DB.txt
* 讀出 argv 的內容並寫入檔案
* 輸出新增了哪些字串
    ```cpp
    #include <stdio.h>
    #include <error.h>
    #include <stdlib.h>

    int main(int argc, char *argv[]){
        // open file
        FILE *fp = fopen("./DB.txt", "a");
        if (!fp) {
            perror("File opening failed");
            return EXIT_FAILURE;
        }

        // write to file
        fputs(argv[1], fp);
        fputs("\n", fp);

        // output to stdout
        printf("HTTP/1.1 200 OK\r\n");
        printf("Content-Type: text/html; charset=UTF-8\r\n\r\n");
        printf("<!DOCTYPE html>\r\n");
        printf("<HTML><HEAD><meta http-equiv=\"content-type\" 
        content=\"text/html;charset=utf-8\">\n");
        printf("<TITLE>Insert complete!!</TITLE>\n");
        printf("<BODY><center><b>Inserted</b> %s", argv[1]);
        printf("<center></BODY></HTML>\n");  
    }
    ```

---

## 成果截圖
* 每當載入一次頁面時， /favicon.ico 都會被請求一次
    * 這是網頁左上角的圖示請求

> 排版不適請移駕至 [github](https://github.com/DannyLeee/HTTPD-from-scratch/master/report.md)

### 先連進 127.0.0.1:8787
* browser 端
![](https://i.imgur.com/CxDZ5uy.png)
* terminal 端
![](https://i.imgur.com/9SnjIzk.png)

### 使用 view.cgi 查看 DB.txt 的內容
* browser 端
![](https://i.imgur.com/gU8UPvj.png)
* terminal 端
![](https://i.imgur.com/Pp3l71a.png)


### 使用 GET 呼叫 insert.cgi 新增 DB.txt 的內容
* browser 端
![](https://i.imgur.com/h96KPDI.png)
* terminal 端
![](https://i.imgur.com/iQl3B1C.png)

### 使用 POST 呼叫 insert.cgi 新增 DB.txt 的內容
* browser 端 (post 的資訊在最下方)
![](https://i.imgur.com/R2S5pqy.png)
![](https://i.imgur.com/Fiqq4u4.png)
* terminal 端
![](https://i.imgur.com/lV44cNi.png)

### 再使用 view.cgi 確認有成功插入 DB.txt
* browser 端
![](https://i.imgur.com/5lwMlFA.png)
* terminal 端
![](https://i.imgur.com/HFaYLPT.png)

---

## 困難與⼼得
　　一開始對功課毫無頭緒，後來助教把範例程式丟上來外加同學給我的簡單範例後，才逐漸了解概念，但單純 trace code 就花了我不少時間，因為除了作業系統稍微修改 kernel 外，沒實際寫過多執行緒的程式，也沒寫過 socket 相關的程式，在查完包含 `pipe`, `dup2`, `socket`, `accept` 等函數的功能與用法後，才正式將2者結合。完成最初結合的隔天就出現了可以連線但瀏覽器沒有輸出的嚴重問題，又隔天才發現 pipe 的宣告放在迴圈外面，導致最初的 respond 有輸出到 socket 而後續的卻只在 terminal。接著遇到的困難是讀檔失敗的問題，有試過包含 file descriptor 的 `open`, `read` 或 `fscanf`, `printf`，一次輸出一個字元或一次所有字串都無法成功，最後是用 FILE pointer 加 `fgetc` 與 `putchar`才成功。insert 的部分有前車之鑑所以用 FILE pointer ， GET 的字串解析及寫檔很順利，但 POST 的因為沒注意到範例裡面的表單有 `enctype=‘multipart/form-data’` 而浪費了一些時間處理字串，刪除加密後就變得非常簡單，也讓我得到意外的收穫。

　　在整個寫完跟同學討論的時候，意外發現 host.cpp 應該要拆成有負責處理連線的一個程式，與一個負責處理 cgi 的程式，而不是像我的作法合在一起處理，導致我沒用到助教的第2個 pipe，但時間過於緊迫實際上的功能也做得差不多了，我想就先這樣吧。

---

## 參考資料與致謝
> cdoe註解中 socket  function 的參數與功能
> > https://pubs.opengroup.org/onlinepubs/007908799/xns <br>
> > http://beej-zhtw.netdpi.net/03-ip-address-structure-translation/3-3-structure <br>
> > http://man7.org/linux/man-pages/man2 <br>

> execlp argv的用法: http://burweisnote.blogspot.com/2017/08/execlpexecvp.html

> 同學的範例
> > https://youtu.be/Q1bHO4VbUck<br>
> > https://github.com/zpaylvlz/CCU_network_program/blob/master/webs.c

<script type="text/javascript" src="http://cdn.mathjax.org/mathjax/latest/MathJax.js?config=TeX-AMS-MML_HTMLorMML"></script>
<script type="text/x-mathjax-config">
    MathJax.Hub.Config({ tex2jax: {inlineMath: [['$', '$']]}, messageStyle: "none" });
</script>
