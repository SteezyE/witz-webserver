#include "sock.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>

#define MAX(A,B) ((A) > (B) ? (A) : (B))
#define IP "127.0.0.1"
#define PORT "55555"
#define WTZFSIZE 220000
#define WTZCNT 1000
#define DEBUG
#undef DEBUG // remove to see debug messages

int main()
{

	FILE *fp;
	fp = fopen("../witze", "r");
	char wtzfile[WTZFSIZE];
	int wtzptr[WTZCNT+1] = {0}, wsize = 0, j = 0, c;
	while((c = fgetc(fp)) != EOF)
	{
		if(c == '%') 
		{
			wtzfile[j++] = 0;
			wtzptr[++wsize] = j;
			if(fgetc(fp) == EOF) break;
		}
		else wtzfile[j++] = c;
	}
#ifdef _WIN32		
	WSADATA d;
	if(WSAStartup(MAKEWORD(2, 2), &d)) return EXIT_FAILURE;
#endif 
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	struct addrinfo *abind;
	getaddrinfo(IP, PORT, &hints, &abind);

	SOCKET sock = socket(abind->ai_family, abind->ai_socktype, abind->ai_protocol);
	if(!VALIDSOCKET(sock)) return SOCKERR();

	if(bind(sock, abind->ai_addr, abind->ai_addrlen)) return SOCKERR();
	freeaddrinfo(abind);

	if(listen(sock, 100) < 0) return SOCKERR();
#ifdef DEBUG
	printf("witze server is live at %s:%s\n", IP, PORT);
#endif
	time_t timer;
	srand(time(NULL));

	fd_set master;
	FD_ZERO(&master);
	FD_SET(sock, &master);
	SOCKET maxsock = sock;

	const char *header = 
			"HTTP/1.1 200 OK\r\n"
			"Connection: close\r\n"
			"Content-Type: text/plain; charset=utf-8\r\n\r\n";
	while(1)
	{
		fd_set reads = master;
		if(select(maxsock+1, &reads, 0, 0, 0) < 0) return SOCKERR();
		for(SOCKET i=1; i<=maxsock; i++)
		{
			if(FD_ISSET(i, &reads))
			{
				if(i == sock)
				{
					struct sockaddr_storage aclient;
					socklen_t csize = sizeof(aclient);
					SOCKET csock = accept(sock, (struct sockaddr*) &aclient, &csize);
					if(!VALIDSOCKET(csock)) return SOCKERR();
					FD_SET(csock, &master);
					maxsock = MAX(maxsock, csock);
#ifdef DEBUG
					char abuf[100];
					getnameinfo((struct sockaddr*) &aclient, csize, abuf, sizeof(abuf), 0, 0, NI_NUMERICHOST);
					time(&timer);
					struct tm *ptm = localtime(&timer);
					printf("%02d:%02d:%02d %s just connected\n", ptm->tm_hour, ptm->tm_min, ptm->tm_sec, abuf);
#endif
				}
				else
				{
					char read[1024];
					int bsize = recv(i, read, sizeof(read), 0);
					if(bsize < 1) goto die;
					int rnd = rand();
					send(i, header, strlen(header), 0);
					send(i, wtzfile + wtzptr[rnd % wsize], wtzptr[rnd % wsize + 1] - wtzptr[rnd % wsize] - 1, 0);
die:
					FD_CLR(i, &master);
					CLOSESOCKET(i);
				}
			}	
		}
	}
	CLOSESOCKET(sock);
#ifdef _WIN32
	WSACleanup();
#endif
	return EXIT_SUCCESS;
}
