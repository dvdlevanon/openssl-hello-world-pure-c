#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define SECURE_PORT 33333
#define NON_SECURE_PORT 44444
#define KEY_FILE_NAME "key.pem"
#define CERTIFICATE_FILE_NAME "cert.pem"

int createSocket(int port)
{
	struct sockaddr_in addr;
	
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	int sfd = socket(AF_INET, SOCK_STREAM, 0);
	
	if (sfd < 0) {
		perror("Unable to create socket\n");
		return -1;
	}
	
	int socketReuse = 1;
	
	if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, 
		&socketReuse, sizeof(socketReuse)) != 0) {
		perror("Error to set socket reuse\n");
		return -1;
	}
	
	if (bind(sfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
		perror("Unable to bind\n");
		return -1;
	}

	if (listen(sfd, 1) < 0) {
		perror("Unable to listen\n");
		return -1;
	}

	return sfd;
}

SSL_CTX* create_context() {
	const SSL_METHOD *method = SSLv23_server_method();
	
	SSL_CTX* ctx = SSL_CTX_new(method);
	
	if (!ctx) {
		perror("Unable to create SSL context");
		ERR_print_errors_fp(stderr);
		return NULL;
	}
	
	SSL_CTX_set_ecdh_auto(ctx, 1);
	
	if (SSL_CTX_use_certificate_file(ctx, CERTIFICATE_FILE_NAME, SSL_FILETYPE_PEM) <= 0) {
		ERR_print_errors_fp(stderr);
		return NULL;
	}
	
	if (SSL_CTX_use_PrivateKey_file(ctx, KEY_FILE_NAME, SSL_FILETYPE_PEM) <= 0 ) {
		ERR_print_errors_fp(stderr);
		return NULL;
	}
	
	return ctx;
}

void* secureServer(void* arg) {
	int sfd = createSocket(SECURE_PORT);
	
	if (sfd < 0) {
		return NULL;
	}
	
	SSL_load_error_strings();	
	OpenSSL_add_ssl_algorithms();
	SSL_CTX *ctx = create_context();
	
	if (ctx == NULL) {
		printf("Error creating ssl context\n");
		return NULL;
	}
	
	while (1) {
		struct sockaddr_in addr;
		uint len = sizeof(addr);
		int client = accept(sfd, (struct sockaddr*)&addr, &len);
		
		if (client < 0) {
			perror("Unable to accept\n");
			return NULL;
		}
		
		SSL *ssl = SSL_new(ctx);
		SSL_set_fd(ssl, client);
		
		if (SSL_accept(ssl) <= 0) {
			ERR_print_errors_fp(stderr);
			return NULL;
		}
		
		char buffer[4096] = { 0 };
		
		int recvCount = SSL_read(ssl, buffer, sizeof(buffer));
		
		if (recvCount > 0) {
			buffer[recvCount] = 0;
		}
		
		printf("Secure client accepted: %s\n", buffer);
		
		const char reply[] = "Secure server\n";
		
		SSL_write(ssl, reply, strlen(reply));
		
		SSL_shutdown(ssl);
		SSL_free(ssl);
		close(client);
	}
	
	return NULL; 
}

void* nonSecureServer(void* arg) {
	int sfd = createSocket(NON_SECURE_PORT);
	
	if (sfd < 0) {
		return NULL;
	}
	
	while (1) {
		struct sockaddr_in addr;
		uint len = sizeof(addr);
		int client = accept(sfd, (struct sockaddr*)&addr, &len);
		
		if (client < 0) {
			perror("Unable to accept\n");
			return NULL;
		}
		
		char buffer[4096] = { 0 };
		int recvCount = recv(client, buffer, sizeof(buffer), 0);
		
		if (recvCount > 0) {
			buffer[recvCount] = 0;
		}
		
		printf("Non secure client accepted: %s\n", buffer);
		
		const char reply[] = "Non secure server\n";
		
		send(client, reply, strlen(reply), 0);
		close(client);
	}
	
	return NULL; 
}

int main(int argc, char *argv[]) {
	pthread_t secureThread;
	
	if (pthread_create(&secureThread, NULL, secureServer, NULL) != 0) {
		return 1;
	}
	
	pthread_t noSecureThread;
	
	if (pthread_create(&noSecureThread, NULL, nonSecureServer, NULL) != 0) {
		return 1;
	}
	
	pthread_join(secureThread, NULL);
	pthread_join(noSecureThread, NULL);
	
	return 0;
}
