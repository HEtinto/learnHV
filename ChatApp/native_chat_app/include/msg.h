#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> // 添加这个头文件以包含Inet_ntop和ntohs
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include "list.h"

#define MAXLINE 4096
#define LISTENQ 1024
#define MAX_DATA_SIZE 1024

#define USER_NAME_LEN 16
#define PASSWORD_LEN 8

typedef struct client_user {
    int user_id;
    char user_name[USER_NAME_LEN];
    char password[PASSWORD_LEN];
} client_user_t;

typedef struct service_user {
    struct list_head recv_head;
    int sockfd;
    bool activate;
} service_user_t;

typedef struct msg {
    int msg_type;
    int data_len;
    char data[MAX_DATA_SIZE];
} __attribute__((packed)) msg_t;

// send queue member
typedef struct send_list {
    struct list_head list;
    msg_t *msg;
}send_list_t;


