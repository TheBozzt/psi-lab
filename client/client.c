#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    const char *host = (argc > 1) ? argv[1] : "127.0.0.1";

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Błąd tworzenia gniazda");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // konwersja ip
    if (inet_pton(AF_INET, host, &serv_addr.sin_addr) <= 0) {
        perror("Nieprawidłowy adres IP");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Błąd połączenia z serwerem");
        return -1;
    }

    printf("Połączono z serwerem\n");

    char input[BUFFER_SIZE];
    char reply[BUFFER_SIZE];

    if (argc > 2) {
        for (int i = 2; i < argc; i++) {
            if (send(sock, argv[i], strlen(argv[i]), 0) < 0) {
                perror("Błąd wysyłania danych");
                break;
            }

            // żeby serwer wiedział, że to już koniec
            if (send(sock, "\n", 1, 0) < 0) {
                perror("Błąd wysyłania znaku nowej linii");
                break;
            }

            // odpowiedz
            ssize_t bytes_read = recv(sock, reply, sizeof(reply) - 1, 0);
            if (bytes_read > 0) {
                reply[bytes_read] = '\0';
                printf("Serwer: %s", reply);
            } else if (bytes_read == 0) {
                printf("Serwer zamknął połączenie.\n");
                break;
            } else {
                perror("Błąd odbierania danych");
                break;
            }
        }

        close(sock);
        return 0;
    }

    while (1) {
        printf("> ");
        fflush(stdout);

        // sizeof(input) -1, by zostało miejsce na odpisanie \n
        if (fgets(input, sizeof(input) - 1, stdin) == NULL) {
            break; // EOF
        }

        // czyszczenie konca lini
        input[strcspn(input, "\r\n")] = '\0';

        if (strcmp(input, "exit") == 0) {
            printf("Połączenie zamknięte\n");
            break;
        }

        if (strlen(input) == 0) {
            continue;
        }

        strcat(input, "\n");

        if (send(sock, input, strlen(input), 0) < 0) {
            perror("Błąd wysyłania danych");
            break;
        }

        // odpowiedz
        ssize_t bytes_read = recv(sock, reply, sizeof(reply) - 1, 0);
        if (bytes_read > 0) {
            reply[bytes_read] = '\0';
            printf("Serwer: %s", reply);
        } else if (bytes_read == 0) {
            printf("Serwer zamknął połączenie.\n");
            break;
        } else {
            perror("Błąd odbierania danych");
            break;
        }
    }

    close(sock);

    return 0;
}
