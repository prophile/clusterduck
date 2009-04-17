#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <poll.h>
#include <pthread.h>
#include <netinet/in.h>
#include <string.h>
#include <setjmp.h>
#include "Shared/Encrypt.h"

static void make_daemon ()
{
	// ignore HUPs so that we don't get reaped by the parent
	pid_t rc;
	signal(SIGHUP, SIG_IGN);
	// fork
	rc = fork();
	if (rc == 0)
	{
		// child process, change to pwd /
		chdir("/");
		// close stdin, stdout and stderr
		close(0);
		close(1);
		close(2);
		// make us leader of a new process group (world of our own!)
		setsid();
	}
	else
	{
		// parent, just exit
		exit(0);
	}
}

struct _socket_handle_data
{
	int fd;
	struct sockaddr_in remote_address;
	socklen_t remote_address_len;
};

static void blocking_read ( int fd, jmp_buf* jumpTarget, void* data, size_t len )
{
	unsigned char* bytes = (unsigned char*)data;
	size_t remaining = len;
	size_t offset = 0;
	ssize_t rc;
	while (remaining > 0)
	{
		rc = read(fd, bytes + offset, remaining);
		if (rc < 0)
		{
			longjmp(*jumpTarget, 1);
		}
		else if (rc == 0)
		{
			usleep(1000 * 5);
		}
		else
		{
			remaining -= rc;
			offset += rc;
		}
	}
}

static void* socket_handle ( void* udata )
{
	jmp_buf storageBuf;
	void* rawData = NULL;
	void* unencryptedData = NULL;
	unsigned int packetLen;
	struct _socket_handle_data* hdata = (struct _socket_handle_data*)udata;
	if (!setjmp(storageBuf))
	{
		blocking_read(hdata->fd, &storageBuf, &packetLen, sizeof(packetLen));
		packetLen = ntohl(packetLen);
		rawData = malloc(packetLen);
		blocking_read(hdata->fd, &storageBuf, rawData, packetLen);
		unencryptedData = cduck_decrypt(rawData, packetLen, &packetLen);
		if (!unencryptedData)
			longjmp(storageBuf, 1);
		free(rawData);
		rawData = NULL;
		// handle the data here
	}
	if (rawData)
		free(rawData);
	if (unencryptedData)
		free(unencryptedData);
	close(hdata->fd);
	free(udata);
	return NULL;
}

int main ( int argc, char** argv )
{
	int sock, rc, one = 1;
	struct pollfd pfd;
	struct _socket_handle_data* handle;
	pthread_t ptt;
	struct sockaddr_in local_addr;
	// make us a daemon process
	make_daemon();
	// set up listener socket
	sock = socket(AF_INET, SOCK_STREAM, 0);
	// bind
	local_addr.sin_len = sizeof(local_addr);
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(33940);
	local_addr.sin_addr.s_addr = INADDR_ANY;
	bind(sock, (struct sockaddr*)&local_addr, sizeof(local_addr));
	// make it listen
	listen(sock, 5);
	// prepare to poll
	while (1)
	{
		pfd.fd = sock;
		pfd.events = POLLIN;
		pfd.revents = 0;
		// poll for input
		rc = poll(&pfd, 1, -1);
		// we got some!
		if (rc > 0)
		{
			// create the new handle, fill it with data
			handle = malloc(sizeof(struct _socket_handle_data));
			handle->fd = accept(sock, (struct sockaddr*)&(handle->remote_address), &(handle->remote_address_len));
			// set SO_LINGER
			setsockopt(handle->fd, 6, SO_LINGER, &one, sizeof(one));
			// dispatch in another thread
			pthread_create(&ptt, NULL, socket_handle, handle);
			pthread_detach(ptt);
		}
	}
	return 0;
}
