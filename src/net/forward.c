#include "forward.h"

// UDP socket to forward data to and address
int sock;
struct addrinfo hints;
struct addrinfo *result, *rp;

int open_udp_socket(char *hostname, char *port){

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
	hints.ai_flags = 0;
	hints.ai_protocol = 0;


	//Create the socket
	int error = getaddrinfo(hostname, port, &hints, &result);
	if(error != 0){
		perror("Failed to create socket");
		return -1;
	}

	for(rp = result; rp != NULL; rp = rp->ai_next){
		sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if(sock == -1)
			continue;

		if(connect(sock, rp->ai_addr, rp->ai_addrlen) == 0)
			break;                  /* Success */

		close(sock);

	}

	if(rp == NULL){
		perror("Bind failed");
		return -1;
	}

	freeaddrinfo(result);

	return 0;

}

int close_udp_socket(){

	close(sock);
	return 0;

}

int send_data(int len, char* data){

	//send the message to server
	int bytesSent = write(sock, data, len);
	if(bytesSent != len){
		perror("Mismatch in number of bytes sent");
		fprintf(stderr, "Send %d bytes\n", bytesSent);
		return -1;
	}

	return 0;

}

int main(int argc, char **argv){

	if(argc < 3){
		printf("Usage: %s hostname port\n", argv[0]);
		return -1;
	}

	char buffer[1000];

	bool running = true;

	// Hostname, Port
	if(open_udp_socket(argv[1], argv[2]) == -1){
		perror("Error creating socket");
		return -1;
	}

	while(running){

		char len1, len2;
		len1 = getc(stdin);
		len2 = getc(stdin);

		if(feof(stdin)){
			printf("stdin closed\n");
			break;
		}

		unsigned short len;

		int count;

		len = (((unsigned char)len1)<<8) + ((unsigned char)len2);

		if(len > 1000){
			fprintf(stderr, "Buffer to small for packet!\n");
			break;
		}

		fread(buffer, sizeof(char), len, stdin);
		/*
		int i;
		for(i = 0; i < len; i++){
			int byte = getc(stdin);
			buffer[i] = (unsigned char)byte;
		}*/

		// Standard log
		fprintf(stdout, "Sending Packet Length: %d\n", ((int)len));

		if(send_data(len, buffer) == -1){
			fprintf(stderr, "Failed to send data to client via udp\n");
			break;
		}

	}

	close_udp_socket();

	return 0;

}
