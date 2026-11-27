#include <stdio.h>
#include <sys/socket.h>
#include <memory.h>
#include <netdb.h>
#include <err.h>

#define BSIZE 1024
#define PORT "8080"

#define TRUE 1

#define bailout(s) { err(1, s); }


int main() {
    struct addrinfo hints;
    struct addrinfo* bindAddr;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(0, PORT, &hints, &bindAddr) != 0)
        bailout("Getting local address");

    int sock = socket(bindAddr->ai_family, bindAddr->ai_socktype, bindAddr->ai_protocol);
    if (sock == -1)
        bailout("Opening stream socket");

    if (bind(sock, bindAddr->ai_addr, bindAddr->ai_addrlen) == -1)
        bailout("Binding stream socket");

    freeaddrinfo(bindAddr);

    uint8_t buf[BSIZE];
    struct sockaddr_in clientAddr;
    socklen_t len = sizeof(clientAddr);
    uint8_t expectedSeq = 0;

    uint8_t reply[14] = {0};

    do {
        ssize_t n = recvfrom(sock, buf, BSIZE, 0, (struct sockaddr*) &clientAddr, &len);
        if (n == -1)
            bailout("Read data");

        uint8_t seq = buf[0];
        if (seq != expectedSeq) {
            if (reply[5] != 0)
                sendto(sock, reply, sizeof(reply), 0, (struct sockaddr*) &clientAddr, len);

            continue;
        }

        uint32_t timestamp;
        memcpy(&timestamp, buf + 1, sizeof(timestamp));
        timestamp = ntohl(timestamp);
        printf("Received. Timestamp: %u, content: %s\n", timestamp, (char*) (buf + 5));

        reply[0] = seq;
        memcpy(reply + 1, buf + 1, sizeof(timestamp));
        memcpy(reply + 5, "Received", 9);

        sendto(sock, reply, sizeof(reply), 0, (struct sockaddr*) &clientAddr, len);

        expectedSeq ^= 1;

    } while (TRUE);

    return 0;
}