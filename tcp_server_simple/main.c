#include "hv/hloop.h"

void on_close(hio_t* io) {
}

typedef struct mytest {
    int age;
    char name[10];
} __attribute__((packed)) mytest_t;

void on_recv(hio_t* io, void* buf, int readbytes) {
	// 打印数据
    mytest_t* test = (mytest_t*)buf;
    printf("recv: age=%d, name=%s\n", test->age, test->name);
    // 回显数据
    hio_write(io, buf, readbytes);
}

void on_accept(hio_t* io) {
	// 设置close回调
    hio_setcb_close(io, on_close);
    // 设置read回调
    hio_setcb_read(io, on_recv);
    // 开始读
    hio_read(io);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: cmd port\n");
        return -10;
    }
    int port = atoi(argv[1]);

    // 创建事件循环
    hloop_t* loop = hloop_new(0);
    // 创建TCP服务
    hio_t* listenio = hloop_create_tcp_server(loop, "0.0.0.0", port, on_accept);
    if (listenio == NULL) {
        return -20;
    }
    // 运行事件循环
    hloop_run(loop);
    // 释放事件循环
    hloop_free(&loop);
    return 0;
}
