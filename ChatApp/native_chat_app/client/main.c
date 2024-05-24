#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> // 添加这个头文件以包含Inet_ntop和ntohs
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>

#define MAXLINE 4096
#define LISTENQ 1024

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
    int sockfd;
    socklen_t len;
    struct sockaddr_in servaddr;
    char buff[MAXLINE] = {0x0};
    time_t ticks;
    size_t n;
    pthread_t tid;
    void *exit_status = NULL;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&servaddr, 0x0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(6666);

    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0){
        perror("connect failed.");
        return -1;
    }
    if ((tid = create_worker(worker_func, sockfd)) > 0) {
        printf("tid %d\n", tid);
        if (pthread_join(tid, &exit_status) != 0) {
            perror("pthread_join error");
            return -1;
        }
    }else {
        perror("create_worker failed.");
        return -1;
    }


    return 0;
}