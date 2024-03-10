#ifndef HTTPCONNECTION_H
#define HTTPCONNECTION_H
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdarg.h>
#include <errno.h>
#include "locker.h"
#include <sys/uio.h>

class http_conn
{

public:
    static int m_epollfd; // 用于保存全局的epoll描述符
    static int m_user_count;
    void init(int sockfd, const sockaddr_in &addr); // 初始化新接受的连接
    void close_conn();
    int read();
    void process();

private:
    int m_sockfd;
    sockaddr_in m_address;
};

#endif