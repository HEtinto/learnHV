#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> // 添加这个头文件以包含Inet_ntop和ntohs
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include "list.h"

#define MAXLINE 4096
#define LISTENQ 1024

typedef struct msg {
    int send_user_id;
    int recv_user_id;
    char data[];
}msg_t;

typedef struct msg_list {
    struct list_head list;
    msg_t *msg;
}msg_list_t;

typedef struct user {
    struct list_head recv_head;
    int sockfd;
}user_t;

typedef struct chat_mgt {
    user_t *user[1024];
    int user_num;
}chat_mgt_t;

struct list_head global_msg_list_head;
chat_mgt_t chat_mgt = {0x0};

user_t *make_user(int sockfd){
    user_t *user = malloc(sizeof(user_t));
    if (user == NULL){
        perror("malloc error");
        return NULL;
    }
    INIT_LIST_HEAD(&user->recv_head);
    user->sockfd = sockfd;
    return user;
}

pthread_t create_worker(void *worker_func(void *), user_t *user){
    pthread_t tid;
    if (0 != pthread_create(&tid, NULL, worker_func, user)) {
        perror("pthread_create error");
        return -1;
    }
    printf("tid %d\n", tid);
    return tid;
}

void *worker_func_recv(void *arg){
    user_t *user = (user_t *)arg;
    printf("socket %d\n", user->sockfd);
    char buff[MAXLINE] = {0x0};
    int n = 0;
    while (1) {
        while ( (n = read(user->sockfd, buff, MAXLINE)) > 0){
            buff[n] = 0;
            msg_t *msg = malloc(sizeof(msg_t) + n);
            if (msg == NULL){
                perror("calloc error, can not save msg data.");
                continue;
            }
            msg_list_t *msg_list = malloc(sizeof(msg_list_t));
            if (msg_list == NULL) {
                perror("malloc error");
                free(msg);
                continue;
            }
            INIT_LIST_HEAD(&msg_list->list);
            memcpy(msg, buff, n);
            msg_list->msg = msg;
            list_add_tail(&msg_list->list, &user->recv_head);
        }
        if (n < 0) {
            perror("read error");
        }
    }
}

void *worker_func_send(void *sock){
    int sockfd = (int)sock;
    printf("socket %d\n", sockfd);
    char buff[MAXLINE] = {0x0};
    int n = 0;
    while (1) {
        while ( (n = read(sockfd, buff, MAXLINE)) > 0){
            buff[n] = 0;
            msg_t *msg = malloc(sizeof(msg_t) + n);
            if (msg == NULL){
                perror("calloc error, can not save msg data.");
            }
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
        user_t *user = make_user(connfd);
        if (user == NULL) {
            perror("make user error");
            close(connfd);
        }
        create_worker(worker_func_recv, user);
    }
    return 0;
}