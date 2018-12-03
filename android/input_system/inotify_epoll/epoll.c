//用来监测多个文件，
//有无数据供读出
//有无空间供写入
//
//epoll_create 创建fd
//epoll_ctl(... EPOLL_CTL_ADD/EPOLL_CTL_DLE ...)
//epoll_wait 等待某个文件可用
//
/* usage: epoll file1... */

#include <sys/epoll.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

int add_to_epoll(int fd, int epollFd) {
    struct epoll_event eventItem;
    memset(&eventItem, 0, sizeof(eventItem));
    eventItem.events = EPOLLIN;
    eventItem.data.fd = fd;
    int result = epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &eventItem);
    return result;
}

void rm_from_epoll(int fd, int epollFd) {
    epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, NULL);
}

int main(int argc, char** argv)
{
    static const int EPOLL_MAX_EVENTS = 16;
    struct epoll_event mPendingEventItems[EPOLL_MAX_EVENTS];

    if (argc < 2) {
        printf("usage: %s file1 ...\n", argv[0]);
        return -1;
    }

    int mEpollFd = epoll_create(8);
    int i;

    for (i = 1; i < argc; i++) {
        int tmpFd = open(argv[i], O_RDWR);
        add_to_epoll(tmpFd, mEpollFd);
    }
    
    char buf[500];

    while (1) {
        int pollResult = epoll_wait(mEpollFd, mPendingEventItems, EPOLL_MAX_EVENTS, -1);
        for (i = 0; i < pollResult; i++) {
            printf("Reason: 0x%x\n", mPendingEventItems[i].events);
            int len = read(mPendingEventItems[i].data.fd, buf, 500);
            buf[len] = '\0';
            printf("buf: %s\n", buf);
        }
    }
    return 0;
}
