#ifdef _WIN32
#define _WIN32_WINNT 0x0600
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#ifndef IPV6_V6ONLY
#define IPV6_V6ONLY 27
#endif
#define VALIDSOCKET(S) ((S) != INVALID_SOCKET)
#define CLOSESOCKET(S) closesocket(S)
#define SOCKERR() (WSAGetLastError())

#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#define VALIDSOCKET(S) ((S) >= 0)
#define CLOSESOCKET(S) close(S)
#define SOCKERR() (errno)
#define SOCKET int
#endif
