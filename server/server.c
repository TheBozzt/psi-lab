#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <err.h>

#include "md5.h"

#define TRUE 1

#define PORT "8080"
#define LISTEN_QS 5
#define BSIZE 1024

#define bailout(s) { err(1, s); }

void get_hash(char* hash, uint8_t* p) {
    for (int i = 0; i < 16; ++i){
        sprintf(hash + i * 2, "%02x", p[i]);
    }
    hash[32] = '\n';
}

void handleConnection(int sock) {
    char buf[BSIZE], result[33];
    memset(buf, 0, BSIZE);

    ssize_t val;

    MD5Context ctx;
    md5Init(&ctx);

    while ((val = read(sock, buf, BSIZE)) != 0) {
        if (val == -1)
            bailout("Reading stream message");

        if (buf[val - 1] != '\n') {
            md5Update(&ctx, buf, val);
            memset(buf, 0, BSIZE);
            continue;
        }
        
        md5Update(&ctx, buf, val - 1);
        md5Finalize(&ctx);

        get_hash(result, ctx.digest);
        write(sock, result, 33);

        memset(buf, 0, BSIZE);
        md5Init(&ctx);
    }

    return;
}

void endChild(int) {
    wait(NULL);
}

int main(int argc, char** argv) {
    struct addrinfo hints;
    struct addrinfo* bindAddr;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(0, PORT, &hints, &bindAddr) != 0)
        bailout("Getting local address");

    int sock = socket(bindAddr->ai_family, bindAddr->ai_socktype, bindAddr->ai_protocol);
    if (sock == -1)
        bailout("Opening stream socket");

    if (bind(sock, bindAddr->ai_addr, bindAddr->ai_addrlen) == -1)
        bailout("Binding stream socket");

    freeaddrinfo(bindAddr);

    listen(sock, LISTEN_QS);
    int msgSock;
    pid_t pid;

    signal(SIGCHLD, endChild);

    do {
        msgSock = accept(sock, (struct sockaddr*) 0, (socklen_t *) 0);
        if (msgSock == -1)
            bailout("Accept");

        pid = fork();
        if (pid == -1)
            bailout("Fork");

        if (pid == 0) {
            close(sock);
            handleConnection(msgSock);
        } else {
            close(msgSock);
        }

    } while (pid != 0);

    return 0;
}