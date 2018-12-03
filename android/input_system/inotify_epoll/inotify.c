
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/inotify.h>
#include <errno.h>

/* inotify <dir> */

int read_process_inotify_fd(int fd) 
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
    //printf("got %d bytes of event information\n", res);

    
    while(res >= (int)sizeof(*event)) {
        event = (struct inotify_event *)(event_buf + event_pos);
        //printf("%d: %08x \"%s\"\n", event->wd, event->mask, event->len ? event->name : "");
        if(event->len) {
            if(event->mask & IN_CREATE) {
                printf("create file %s\n", event->name);
            } else {
                printf("remove file %s\n", event->name);
            }
        }
        event_size = sizeof(*event) + event->len;
        res -= event_size;
        event_pos += event_size;
    }
    return 0;
}

int main(int argc, char **argv)
{
	int mInotifyFd = inotify_init();
	int result = inotify_add_watch(mInotifyFd, argv[1], IN_DELETE | IN_CREATE);

	if (argc != 2) {
		printf("Usages: %s <dir> \n", argv[0]);
		return -1;
	}

	
	while (1) {
		read_process_inotify_fd(mInotifyFd);
	}
	
    return 0;
}

