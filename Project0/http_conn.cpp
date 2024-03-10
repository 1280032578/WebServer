#include "http_conn.h"
// 所有的客户数
int http_conn::m_user_count = 0;
// 所有socket上的事件都被注册到同一个epoll内核事件中，所以设置成静态的
int http_conn::m_epollfd = -1;
int setnonblocking(int fd)
{
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return fd;
}
// 添加文件描述符
void addfd(int epollfd, int fd, bool one_shot)
{
    struct epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLRDHUP;
    if (one_shot)
    {
        event.events |= EPOLLONESHOT;
    }
    int ret = epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    setnonblocking(fd);
}
// 从epoll中移除监听的文件描述符
void removefd(int epollfd, int fd)
{
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, 0);
    close(fd);
}

void http_conn::init(int sockfd, const sockaddr_in &addr)
{
    m_sockfd = sockfd;
    m_address = addr;
    // 端口复用
    int reuse = 1;
    setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    addfd(m_epollfd, sockfd, true);
    http_conn::m_user_count++;
}

void http_conn::close_conn()
{
    if (m_sockfd != -1)
    {
        removefd(m_epollfd, m_sockfd);
        m_sockfd = -1;
        http_conn::m_user_count--;
    }
    return;
}
int http_conn::read()
{
    printf("读数据啦\n");
    // return 1;
}
// 由线程池中的工作线程调用，这是处理HTTP请求的入口函数
void http_conn::process()
{
    printf("解析请求\n");
    return;
}