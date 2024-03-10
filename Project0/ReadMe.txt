这个目录下的代码是为加入业务代码的。
编译指令：g++ *.cpp -lphread
运行命令：./a.out 10002
之后可以再浏览器中是尽情的访问。


Note：在http_conn.cpp中的一下代码，addfd函数的第三个参数设置为true，只能输出一次。设置为false可以不停的读取，因为是水平触发模式。

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
