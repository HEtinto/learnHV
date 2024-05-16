#include "hv/hloop.h"
#include "hv/htime.h"

void on_timer(htimer_t* timer) {
    char str[DATETIME_FMT_BUFLEN] = {0};
    datetime_t dt = datetime_now();
    datetime_fmt(&dt, str);

    printf("> %s\n", str);
    // 获取userdata
    hio_t* io = (hio_t*)hevent_userdata(timer);
    // 发送当前时间字符串
    hio_write(io, str, strlen(str));
}

typedef struct mytest {
    int age;
    char name[10];
} __attribute__((packed)) mytest_t;
// 使用__attribute__((packed)) 预编译宏来取消内存对齐

// 在on_timer1中向服务端发送自定义的结构体数据
// 该函数在定时器到期时被调用
void on_timer1(htimer_t* timer) {
    static int count = 0;
    mytest_t test1 = {0};
    test1.age = count;
    snprintf(test1.name, sizeof(test1.name), "count:%d", count++);
    printf("sizeof : %d\n", sizeof(mytest_t));
    // 获取userdata
    hio_t *io = (hio_t*)hevent_userdata(timer);
    // 发送当前字符串
    hio_write(io, &test1, sizeof(mytest_t));
}

void on_close(hio_t* io) {
}

void on_recv(hio_t* io, void* buf, int readbytes) {
    mytest_t* mytest = (mytest_t*)buf;
    printf("test age:%d, test name:%s\n", mytest->age, mytest->name);
    // printf("< %.*s\n", readbytes, (char*)buf);
}

void on_connect(hio_t* io) {
    // 设置read回调
    hio_setcb_read(io, on_recv);
    // 开始读
    hio_read(io);

	// 添加一个定时器
    htimer_t* timer = htimer_add(hevent_loop(io), on_timer1, 1000, INFINITE);
    // 设置userdata
    hevent_set_userdata(timer, io);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: cmd port\n");
        return -10;
    }
    int port = atoi(argv[1]);

	// 创建事件循环
    hloop_t* loop = hloop_new(0);
    // 创建TCP客户端
    hio_t* io = hloop_create_tcp_client(loop, "127.0.0.1", port, on_connect, on_close);
    if (io == NULL) {
        return -20;
    }
    // 运行事件循环
    hloop_run(loop);
    // 释放事件循环
    hloop_free(&loop);
    return 0;
}
