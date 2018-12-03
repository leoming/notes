//用来监测多个文件，
//有无数据供读出
//有无空间供写入
//
//epoll_create 创建fd
//epoll_ctl(... EPOLL_CTL_ADD/EPOLL_CTL_DLE ...)
//epoll_wait 等待某个文件可用
//
/* usage: epoll file1... */

#include <sys/inotify.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

char* base_dir;
char* epoll_file[1000];


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

int get_fd_from_name(char* file_name) {
    int i; 
    char name[100];
    sprintf(name, "%s/%s", base_dir, file_name);
    for (i = 0; i < 1000; i++) {
        if (!epoll_file[i])
            continue;
        if (!strcmp(epoll_file[i], name))
            return i;
    }
    return -1;
}

int read_process_inotify_fd(int fd, int epollFd) 
{
	int res;
    char event_buf[512];
    int event_size;
    int event_pos = 0;
    struct inotify_event *event;

    res = read(fd, event_buf, sizeof(event_buf));
    if(res < (int)sizeof(*event)) {
        if(errno == EINTR)
            return 0;
        printf("could not get event, %s\n", strerror(errno));
        return -1;
    }

    
    while(res >= (int)sizeof(*event)) {
        event = (struct inotify_event *)(event_buf + event_pos);
        //printf("%d: %08x \"%s\"\n", event->wd, event->mask, event->len ? event->name : "");
        if(event->len) {
            if(event->mask & IN_CREATE) {
                printf("create file %s\n", event->name);
                char* name  = (char*)malloc(512);
                sprintf(name, "%s/%s", base_dir, event->name);
                int tmpFd = open(name, O_RDWR);
                printf("add to epoll: %s\n", name);
                add_to_epoll(tmpFd, epollFd);
                epoll_file[tmpFd] = name;
            } else {
                int tmpFd = get_fd_from_name(event->name);
                if (tmpFd >= 0) {
                    printf("remove file %s\n", event->name);
                    rm_from_epoll(tmpFd, epollFd);
                    free(epoll_file[tmpFd]);
                }
            }
        }
        event_size = sizeof(*event) + event->len;
        res -= event_size;
        event_pos += event_size;
    }
    return 0;
}

int main(int argc, char** argv)
{
    static const int EPOLL_MAX_EVENTS = 16;
    struct epoll_event mPendingEventItems[EPOLL_MAX_EVENTS];

    if (argc < 2) {
        printf("usage: %s file1 ...\n", argv[0]);
        return -1;
    }
	int mInotifyFd = inotify_init();

    int mEpollFd = epoll_create(8);
    int i;

    base_dir = argv[1];
	int result = inotify_add_watch(mInotifyFd, argv[1], IN_DELETE | IN_CREATE);
    add_to_epoll(mInotifyFd, mEpollFd);
    
    char buf[500];

    while (1) {
        int pollResult = epoll_wait(mEpollFd, mPendingEventItems, EPOLL_MAX_EVENTS, -1);
        for (i = 0; i < pollResult; i++) {
            if (mPendingEventItems[i].data.fd == mInotifyFd) {
		        read_process_inotify_fd(mInotifyFd, mEpollFd);
            } else {
                printf("Reason: 0x%x\n", mPendingEventItems[i].events);
                int len = read(mPendingEventItems[i].data.fd, buf, 500);
                buf[len] = '\0';
                printf("buf: %s\n", buf);
            }
        }
    }
    return 0;
}


