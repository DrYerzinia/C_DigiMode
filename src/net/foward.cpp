#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

#include <iomanip>
#include <iostream>
#include <cstring>

#include "../util/Util.cpp"

// UDP socket to forward data to and address
int sock;
struct addrinfo hints;
struct addrinfo *result, *rp;

int open_udp_socket(std::string hostname, std::string port){

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
	hints.ai_flags = 0;
	hints.ai_protocol = 0;


	//Create the socket
	int error = getaddrinfo(hostname.c_str(), port.c_str(), &hints, &result);
	if(error != 0) {
      std::cerr << "Failed to create socket" << std::endl;
      return -1;
   }

	for(rp = result; rp != NULL; rp = rp->ai_next) {
		sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if(sock == -1)
			continue;

		if(connect(sock, rp->ai_addr, rp->ai_addrlen) == 0)
			break;                  /* Success */

		close(sock);

	}

   if(rp == NULL) {
	  std::cerr << "bind failed" << std::endl;
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
	if(bytesSent != len) {
	   std::cerr << "Mismatch in number of bytes sent" << std::endl;
	   std::cerr << "Error " << errno << std::endl;
	   std::cerr << "Sent " << bytesSent << " bytes" << std::endl;
	   return -1;
   }

	return 0;

}

int main(int argc, char **argv){

	std::string args;
	for(int i = 0; i < argc; i++) args += std::string(argv[i]) + std::string(" ");
	util::readInArguments(args);

	std::string hostname = util::getArgument("--hostname");
	std::string port = util::getArgument("--port");

	char buffer[1000];

	bool running = true;

	if(open_udp_socket(hostname, port) == -1) return -1;

	while(running){

		char len;
		if(!std::cin.get(len)) break;
		if(!std::cin.read(buffer, len)) break;

		std::cout << "Sending Packet Length: " << ((int)len) << std::endl;

		if(send_data(len, buffer) == -1) break;

	}

	close_udp_socket();

	return 0;

}
