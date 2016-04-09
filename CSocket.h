#ifndef __SOCKET_H
#define __SOCKET_H

#include <string>
#include <netinet/in.h>

class CSocket
{
public:
    CSocket();
    virtual ~CSocket();
	std::string  getLocalAddr();
	std::string getCliIp();
	int getSockfd() { return sockfd;}
	bool Close();
	bool Connect();
	bool Send(char* sendbuf, int len);
	bool Recv(char* recvbuf, int len, int timeout);

	bool setSoLinger(bool dolinger, int seconds);
	bool setIntOptions(int option, int value);
	bool setTimeout(int option, int milliseconds);
	bool setNonBlock(bool isnonsocket);
	bool setNoDelay(bool nodelay);

	bool setUp(int sockfd, struct sockaddr_in* addr);
	bool setAddr(const char* addr, int port);
protected:
	int sockfd;
	char ip[20];
	struct sockaddr_in sockAddr;	
	CSocket(const CSocket&);
	CSocket& operator=(const CSocket&);
};


//tcp server
class CServSocket : public CSocket
{
public:
    CServSocket();
    virtual ~CServSocket();

	CSocket* Accept();
    bool Listen(int port);

private:
	bool Close();
};


//tcp client
class CCliSocket
{
public:
    CCliSocket();
    virtual ~CCliSocket();
	bool Connect(int port);
	bool Send(const char* sendbuf, int len);
	bool Recv(char* recvbuf, int len);
	bool GetHostIp(const char* addr);
	char* getIp() { return servip;}
	int getCliSockfd() { return clisockfd;}
	bool Close();
private:
    char servip[32];
	int clisockfd;	
};

#endif //__SOCKET_H

