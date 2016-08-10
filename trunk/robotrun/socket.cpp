#include "socket.h"
#include "logger.h"

#ifdef WIN32
#pragma comment(lib, "ws2_32.lib")
#endif


Socket::Socket()
{
	sock = -1;
}

Socket::~Socket()
{

}

bool	Socket::init()
{
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1)
	{
		Logger::getInstance()->log("Could not create socket");
		return false;
	}
	// puts("Socket created");

	memset((void *)&server, 0x00, sizeof(server));
	server.sin_addr.s_addr = inet_addr(SERVER_ADD);
	server.sin_family = AF_INET;
	server.sin_port = htons(SERVER_PORT);

	//set_nonblock
/*
#ifdef __linux__
	int flags = fcntl(sock, F_GETFL, 0);
	//assert(flags != -1);
	if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) < 0) return false;
#else
	unsigned long arg = 1;
	if( ioctlsocket(sock, FIONBIO, &arg) != 0) return false;
#endif

*/

	return true;
}

void	Socket::uninit()
{
	closesocket();
}

bool	Socket::connect()
{
	//Connect to server
	int err = ::connect(sock, (struct sockaddr *)&server, sizeof(server));
	if ( err < 0)
	{
		Logger::getInstance()->log("connect failed. Error!");
		return false;
	}
	return true;
}

void	Socket::closesocket()
{
	if (sock == -1) return;
#if WIN32
	::closesocket(sock);
#else
	close(sock);
#endif
	sock = -1;
}

// select socket
bool	Socket::update()
{
	fd_set read_flags, write_flags;
	struct timeval waitd;          // the max wait time for an event
	int sel;

	waitd.tv_sec = 10;
	waitd.tv_usec = 0;
	FD_ZERO(&read_flags);
	FD_ZERO(&write_flags);
	FD_SET(sock, &read_flags);

	sel = select(sock + 1, &read_flags, &write_flags, (fd_set*)0, &waitd);
	if (sel < 0) return true;	// 아무것도 없다!

	// 읽을것이 있으면 read
	if (FD_ISSET(sock, &read_flags)) 
	{
		FD_CLR(sock, &read_flags);

		char in[SOCKET_BUFFER];
		memset(&in, 0, sizeof(in));

		int recvsize = ::recv(sock, in, sizeof(in), 0);
		if (recvsize <= 0)
		{
			Logger::getInstance()->log("Socket recv. Error!");
			closesocket();
			return false;
		}
		else
		{
			// 수신! (완료라 볼 수 없다!)
			if (recvbuffer.totalsize > 0 && recvbuffer.totalsize > recvbuffer.currentsize)
			{
				// 이전에 받던것이 있다. 이어서 받는다
				memcpy(recvbuffer.buffer + recvbuffer.currentsize, in, recvsize - sizeof(int));
				recvbuffer.currentsize += recvsize;

				// 수신 완료
				if (recvbuffer.totalsize == recvbuffer.currentsize)
					recvdone();
			}
			else
			{
				// 처음 받음
				int psize = (int)in;
				recvbuffer.totalsize = psize;
				recvbuffer.currentsize = recvsize - sizeof(int);
				memcpy(recvbuffer.buffer, in + sizeof(int), recvsize - sizeof(int));

				// 수신 완료
				if (recvbuffer.totalsize == recvbuffer.currentsize)
					recvdone();
			}
		}
	}

	// 보낼것이 있으면 보낸다로 설정
	if (sendbuffer.totalsize > 0)
		FD_SET(sock, &write_flags);

	// 보냄
	if (FD_ISSET(sock, &write_flags))
	{
		FD_CLR(sock, &write_flags);
		int sendsize = ::send(sock, sendbuffer.buffer+sendbuffer.currentsize, sendbuffer.totalsize - sendbuffer.currentsize, 0);
		if (sendbuffer.totalsize == sendbuffer.currentsize+sendsize)
		{
			senddone();
		}
		else
		{
			sendbuffer.currentsize += sendsize;
		}
	}

	return true;
}


void	Socket::recvdone()
{
	recvbufferlist.push_back(recvbuffer);

	recvbuffer.totalsize = -1;
	recvbuffer.currentsize = 0;
	memset(recvbuffer.buffer, 0, SOCKET_BUFFER);
}

void	Socket::senddone()
{
	if (!sendbufferlist.empty())
	{
		sendbuffer.totalsize = sendbufferlist[0].totalsize;
		sendbuffer.currentsize = 0;
		memcpy(sendbuffer.buffer, sendbufferlist[0].buffer, sendbufferlist[0].totalsize);
		sendbufferlist.pop_front();
	}
	else
	{
		sendbuffer.totalsize = -1;
		sendbuffer.currentsize = 0;
		memset(sendbuffer.buffer, 0, SOCKET_BUFFER);
	}
}

bool	Socket::sendpacket(int packetsize, char *packet)
{
	if (sendbuffer.totalsize > 0)
	{
		// 전송 중이라 쌓아 놓는다
		SocketBuffer buf;
		buf.totalsize = packetsize;
		memcpy(buf.buffer, packet, packetsize);
		sendbufferlist.push_back(buf);
	}
	else
	{
		// 바로 전송할거로 이동
		sendbuffer.totalsize = packetsize;
		sendbuffer.currentsize = 0;
		memcpy(sendbuffer.buffer, packet, packetsize);
	}
	return true;
}

// Read packet for parse
bool	Socket::recvpacket(SocketBuffer *buffer)
{
	if( !recvbufferlist.empty() )
	{
		buffer->totalsize = recvbufferlist[0].totalsize;
		buffer->currentsize = recvbufferlist[0].currentsize;
		memcpy(buffer->buffer, recvbufferlist[0].buffer, SOCKET_BUFFER);
		recvbufferlist.pop_front();
		return true;
	}

	return false;
}

