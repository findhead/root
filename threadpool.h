#include <vector>
#include <pthread.h>
#include "locker.h"
#include "cond.h"
#include "head.h"
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <string>

void* myptbread_read(void* args) {
    int* fd_ptr = static_cast<int*>(args);
    int lfd = *fd_ptr;
   // delete fd_ptr; // 释放动态分配的内存
    
    // 1. 读取请求（带错误处理和边界检查）
    char buf[4096];
    ssize_t bytes_read = read(lfd, buf, sizeof(buf) - 1);
    
    if (bytes_read <= 0) {
        if (bytes_read < 0) {
            perror("read error");
        }
        close(lfd);
        return nullptr;
    }
    buf[bytes_read] = '\0';
    
    // 2. 验证HTTP请求（简化版）
    if (strstr(buf, "HTTP/") == nullptr) {
        const char* bad_request = 
            "HTTP/1.1 400 Bad Request\r\n"
            "Content-Length: 0\r\n\r\n";
        write(lfd, bad_request, strlen(bad_request));
        close(lfd);
        return nullptr;
    }
    
    // 3. 构建精确长度的响应
    const std::string content = "<h1>hello world!</h1>";
    std::string response = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html; charset=utf-8\r\n"
        "Connection: close\r\n"
        "Content-Length: " + std::to_string(content.length()) + "\r\n\r\n" +
        content;
    
    // 4. 循环写入确保完整发送
    size_t total_sent = 0;
    const char* response_data = response.c_str();
    size_t response_len = response.length();
    
    while (total_sent < response_len) {
        ssize_t sent = write(lfd, response_data + total_sent, response_len - total_sent);
        
        if (sent < 0) {
            if (errno == EINTR) {
                continue; // 被信号中断，重试
            }
            if (errno == EPIPE) {
                break; // 客户端已关闭连接
            }
            perror("write error");
            break;
        } else if (sent == 0) {
            break; // 不应发生，但安全处理
        } else {
            total_sent += sent;
        }
    }
    
    // 5. 诊断输出
    if (total_sent == response_len) {
        std::cout << "Success: Sent " << total_sent << " bytes" << std::endl;
    } else {
        std::cout << "Partial send: " << total_sent << "/" << response_len 
                  << " bytes (" << (100.0 * total_sent / response_len) << "%)" << std::endl;
    }
    
    // 6. 优雅关闭连接
    shutdown(lfd, SHUT_RDWR); // 完全关闭读写
    close(lfd);
    
    return nullptr;
}
template <typename T>
class threadpool {
public:
    static threadpool<T>* thpool;  // 静态成员声明
    
    static threadpool<T>* getInstance() {
        if (thpool == nullptr) {
            thpool = new threadpool<T>();
        }
        return thpool;
    }
    
    void append(T num);
    
private:
    threadpool();
    ~threadpool();
    static void* worker(void* arg);
    
    int m_size;
    std::vector<pthread_t> pool;
    std::vector<T> tasks;
    locker m_lock;
    cond m_cond;
};

// 静态成员定义
template <typename T>
threadpool<T>* threadpool<T>::thpool = nullptr;

template <typename T>
threadpool<T>::threadpool() : m_size(8) {
    pool.resize(m_size);
    for (int t = 0; t < m_size; ++t) {
        if (pthread_create(&pool[t], NULL, worker, this) != 0) {
            std::cout << "pthread error" << std::endl;
            exit(1);
        }
        pthread_detach(pool[t]);
    }
}

template <typename T>
void* threadpool<T>::worker(void* arg) {
    threadpool* pool = threadpool<T>::getInstance();;
    while (true) {
        pool->m_lock.lock();
        while (pool->tasks.empty()) {
            pool->m_cond.wait(pool->m_lock);
        }
        T task = pool->tasks.front();
        pool->tasks.erase(pool->tasks.begin());
        pool->m_lock.unlock();
        
        // 处理任务
        myptbread_read(&task);
    }
    return NULL;
}

template <typename T>
void threadpool<T>::append(T num) {
    m_lock.lock();
    tasks.push_back(num);
    m_cond.signal();
    m_lock.unlock();
}
