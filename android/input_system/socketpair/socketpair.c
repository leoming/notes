#include <pthread.h>

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

#define SOCKET_BUFFER_SIZE (32768U)

void* func_thread1(void* arg) {
    int fd = *(int*)arg;
    char buf[500];
    int len;
    int cnt = 0;
    memset(buf, 0, 500);
    while (1) {
        len = sprintf(buf, "hello main thread, cnt = %d\n", cnt++);
        write(fd, buf, len);
        len = read(fd, buf, 500);
        printf("%s\n", buf);
        sleep(5);
    }
    return NULL;
}

int main()
{
    int sockets[2];
    socketpair(AF_UNIX, SOCK_SEQPACKET, 0, sockets);
    int bufferSize = SOCKET_BUFFER_SIZE;
    setsockopt(sockets[0], SOL_SOCKET, SO_SNDBUF, &bufferSize, sizeof(bufferSize));
    setsockopt(sockets[0], SOL_SOCKET, SO_RCVBUF, &bufferSize, sizeof(bufferSize));
    setsockopt(sockets[1], SOL_SOCKET, SO_SNDBUF, &bufferSize, sizeof(bufferSize));
    setsockopt(sockets[1], SOL_SOCKET, SO_RCVBUF, &bufferSize, sizeof(bufferSize));

    pthread_t threadID;
    pthread_create(&threadID, NULL, func_thread1, &sockets[1]);

    char buf[500];
    int len;
    int cnt = 0;
    int fd = sockets[0];
    while (1) {
        /* read thread 1 data */
        len = read(fd, buf, 500);
        printf("%s\n", buf);

        /* send data to thread 1 */
        len = sprintf(buf, "hello thread 1, count = %d\n", cnt++);
        write(fd, buf, len);
    }
    return 0;
}
