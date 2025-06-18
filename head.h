#include <iostream>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <mysql/mysql.h>
#include <pthread.h>
#include <poll.h>            // poll系统调用
#include <sys/epoll.h>       // epoll I/O多路复用(Linux特有)
#include <sys/select.h>      // select系统调用
#include <errno.h>           // 错误码(errno)
#include <string.h>          // 字符串操作(strerror等)
#include <stdlib.h>          // 通用工具函数
#include <stdio.h>           // 标准I/O
#include <sys/types.h>       // 基本系统数据类型
#include <sys/stat.h>        // 文件状态
#include <sys/time.h>        // 时间相关
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <vector>
