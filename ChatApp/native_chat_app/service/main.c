#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> // 添加这个头文件以包含Inet_ntop和ntohs
#include <string.h>
#include <unistd.h>
#include <time.h>

#define MAXLINE 4096
#define LISTENQ 1024

struct chat_socket {
    int sockfd[1024];
    int sock_max;
};

pthread_t create_worker(void *worker_func(void *), int sockfd){
    pthread_t tid;
    printf("socket %d\n", sockfd);
    if (0 != pthread_create(&tid, NULL, worker_func, sockfd)) {
        perror("pthread_create error");
        return -1;
    }
    printf("tid %d\n", tid);
    return tid;
}

void *worker_func(void *sock){
    int sockfd = (int)sock;
    printf("socket %d\n", sockfd);
    char buff[MAXLINE] = {0x0};
    int n = 0;
    while (1) {
        while ( (n = read(sockfd, buff, MAXLINE)) > 0){
        buff[n] = 0;
        if (fputs(buff, stdout) == EOF)
            perror("EOF read.");
        }
        if (n < 0) {
            perror("read error");
        }
    }
}

int main()
{
    int listenfd, connfd;
    socklen_t len;
    struct sockaddr_in servaddr, cliaddr;
    char buff[MAXLINE];
    time_t ticks;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(6666);

    bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    listen(listenfd, LISTENQ);

    for (;;) {
        len = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &len);
        for (;;) {
            printf("connection from %s, port %d\n",
                inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof(buff)),
                ntohs(cliaddr.sin_port));
            ticks = time(NULL);
            snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
            write(connfd, buff, strlen(buff));
            sleep(1);
        }
        close(connfd);
    }
    return 0;
}