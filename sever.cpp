#include <iostream>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "threadpool.h"
#include "head.h"

const int port = 8888;



void severinit() {
	struct sockaddr_in addr;
	int opt = 1;
	int lfd = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(lfd, (sockaddr*)&addr, sizeof(addr));
	listen(lfd, 2000);

	int epfd = epoll_create(2000);
	if (epfd == -1) {
		std::cerr << "create has errno" << std::endl;
		exit(1);
	}

	struct epoll_event op, ev[2000];
	op.data.fd = lfd;
	op.events = EPOLLIN;
	epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &op);

	threadpool<int>::getInstance();

	while (1) {
		int num = epoll_wait(epfd, ev, 2000, 1);
		if(num < 0) continue;
		if (num > 0) {
			for (int i = 0; i < num; ++i) {
				if (ev[i].data.fd == lfd) {
					int nfd = accept(lfd, NULL, NULL);
					struct epoll_event next;
					next.data.fd = nfd;
					next.events = EPOLLIN;
					epoll_ctl(epfd, EPOLL_CTL_ADD, nfd, &next);
				} else {
					int nfd = ev[i].data.fd;
					threadpool<int>::thpool->append(nfd);
				}
			}
		}
	}
}

int main() {
	severinit();
	return 0;
}
