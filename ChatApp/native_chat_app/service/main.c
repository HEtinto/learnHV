#include "../include/msg.h"

typedef struct msg_list {
    struct list_head list;
    msg_t *msg;
}msg_list_t;

typedef struct chat_mgt {
    service_user_t *user[1024];
    int user_num;
}chat_mgt_t;

struct list_head global_msg_list_head;
chat_mgt_t chat_mgt = {.user = 0x0, .user_num = 1024};

void release_msg_list(msg_list_t *msg_list){
    free(msg_list->msg);
    list_del(&msg_list->list);
    free(msg_list);
}

service_user_t *make_user(int sockfd){
    service_user_t *user = malloc(sizeof(service_user_t));
    if (user == NULL){
        perror("malloc error");
        return NULL;
    }
    INIT_LIST_HEAD(&user->recv_head);
    user->sockfd = sockfd;
    return user;
}

pthread_t create_worker(void *worker_func(void *), int sockfd){
    pthread_t tid;
    if (0 != pthread_create(&tid, NULL, worker_func, &sockfd)) {
        perror("pthread_create error");
        return -1;
    }
    printf("tid %d\n", tid);
    return tid;
}

void *worker_func_recv(void *arg){
    int sockfd = *((int *)arg);
    printf("socket %d\n", sockfd);
    char buff[MAXLINE] = {0x0};
    int n = 0;
    service_user_t *user = NULL;
    while (1) {
        while ( (n = read(sockfd, buff, MAXLINE)) > 0){
            buff[n] = 0;
            msg_t *msg = buff;
            printf("data len: %d\n", msg->data_len);
        }
        if (n == 0) {
            if (user != NULL) {
                user->activate = false;
            }
            perror("Connection closed by perr.\n");
            break;
        }
        if (n < 0) {
            if (errno == EINTR) {
                continue;
            } else {
                if (user != NULL) {
                    user->activate = false;
                }
                perror("read error");
                break;
            }
        }
    }
    // 关闭文件描述符
    close(sockfd);
}

void *worker_func_send(void *arg){
    
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
        create_worker(worker_func_recv, connfd);
    }
    return 0;
}