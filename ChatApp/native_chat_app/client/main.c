#include "../include/msg.h"


// SEND QUEUE
struct list_head global_send_queue_head;

pthread_t create_worker(void *worker_func(void *), int sockfd){
    pthread_t tid;
    printf("socket %d\n", sockfd);
    if (0 != pthread_create(&tid, NULL, worker_func, &sockfd)) {
        perror("pthread_create error");
        return -1;
    }
    printf("tid %d\n", tid);
    return tid;
}

void *client_worker_func_send(void *sock){
    int sockfd = *((int *)sock);
    send_list_t *psend_list = NULL;
    send_list_t *psend_list_tmp = NULL;
    while (1) {
        list_for_each_entry_safe(psend_list, psend_list_tmp, &global_send_queue_head, list){
            printf("Data insert queue.\n");
            write(sockfd, psend_list->msg, psend_list->msg->data_len);
            list_del_init(&psend_list->list);
            free(psend_list->msg);
            free(psend_list);
        }
    }
}

void *client_worker_func_recv(void *sock){
    int sockfd = *((int *)sock);
    printf("socket %d\n", sockfd);
    char buff[MAXLINE] = {0x0};
    int n = 0;
    msg_t *msg = NULL;
    while (1) {
        while ( (n = read(sockfd, buff, MAXLINE)) > 0){

        }
        if (n == 0){
            perror("Connection closed.\n");
            break;
        }
        if (n < 0){
            if (errno == EINTR){
                continue;
            }
            break;
        }
    }
}

void test_send_msg()
{
    msg_t *msg = calloc(sizeof(msg_t), 1);
    send_list_t *node = calloc(sizeof(send_list_t), 1);
    node->msg = msg;
    printf("INPUT:\n");
    fgets(msg->data, MAX_DATA_SIZE, stdin);
    list_add_tail(&node->list, node);
}

client_user_t user = {0x0};

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

    // init send queue
    INIT_LIST_HEAD(&global_send_queue_head);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&servaddr, 0x0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(6666);

    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0){
        perror("connect failed.");
        return -1;
    }

    if ((tid = create_worker(client_worker_func_send, sockfd)) > 0) {
        printf("tid %d\n", tid);
        // test send data
        test_send_msg();
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