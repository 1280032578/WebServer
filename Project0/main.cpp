
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include "locker.h"
#include "threadpool.h"
#include "http_conn.h"
using namespace std;

#define MAX_FD 65536		   // 最大的文件描述符个数
#define MAX_EVENT_NUMBER 10000 // 监听的最大的事件数量

// 添加文件描述符
extern void addfd(int epollfd, int fd, bool one_shot);
extern void removefd(int epollfd, int fd);

void addsig(int sig, void (*handler)(int))
{
	printf("addsig begin\n");
	struct sigaction sa;
	memset(&sa, '\0', sizeof(sa));
	sa.sa_flags = 0;
	sa.sa_handler = handler;
	sigfillset(&sa.sa_mask);
	sigaction(sig, &sa, NULL);
	printf("addsig over\n");
}
int main(int argc, char *argv[])
{
	// 0.check参数
	if (argc <= 1)
	{
		printf("参数个数太少！");
		return 0;
	}
	int port = atoi(argv[1]);
	// 1.捕获信息
	addsig(SIGPIPE, SIG_IGN);
	// 2.创建线程池
	threadpool<http_conn> *pool = new threadpool<http_conn>;
	// 3.TCP网络通信模板
	int listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd == -1)
	{
		printf("创建监听描述符错误!\n");
		return 0;
	}
	// IP+Port
	struct sockaddr_in address;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	// 设置端口复用

	int reuse = 1;
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

	int ret = bind(listenfd, (struct sockaddr *)&address, sizeof(address));
	ret = listen(listenfd, 5);
	// epoll

	// 1.创建epoll实例
	int epollfd = epoll_create(10);
	// 2.将当前listenfd描述符加入到epoll事件表中
	addfd(epollfd, listenfd, false);
	// 3.创建事件数组，用于从内核中返回发生事件的描述符
	epoll_event events[MAX_EVENT_NUMBER];

	// 业务逻辑
	// 创建数组保存所有客户端的信息
	http_conn *user = new http_conn[MAX_FD];
	http_conn::m_epollfd = epollfd;

	while (1)
	{
		int nums = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, -1);
		for (int i = 0; i < nums; i++)
		{
			int socketfd = events[i].data.fd;
			if (socketfd == listenfd)
			{
				// 有新的客户端进来
				struct sockaddr_in client_address;
				socklen_t client_addrlength = sizeof(client_address);
				// 尝试创建fd与其连接
				int connfd = accept(listenfd, (struct sockaddr *)&client_address, &client_addrlength);
				user[connfd].init(connfd, client_address);
			}
			else if (events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR))
			{
				// 出现问题了，需要与其断开连接
				user[socketfd].close_conn();
			}
			else if (events[i].events & EPOLLIN)
			{
				if (user[socketfd].read())
				{
					pool->append(user + socketfd);
				}
				else
				{
					user[socketfd].close_conn();
				}
			}
			else if (events[i].events & EPOLLOUT)
			{
			}
		}
	}
	close(epollfd);
	close(listenfd);
	delete[] user;
	delete pool;
	return 0;
}