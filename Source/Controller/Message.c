#include "Message.h"
#include "Shared/Encrypt.h"
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <poll.h>
#include <arpa/inet.h>
#include <string.h>
#include <assert.h>

static void send_packet ( int socket, unsigned short msgid, const void* data, unsigned int dlen )
{
	void* newblock2;
	unsigned int newblock2len;
	unsigned char* newblock = malloc(dlen + 2);
	memcpy(newblock + 2, data, dlen);
	msgid = htons(msgid);
	memcpy(newblock, &msgid, 2);
	newblock2 = cduck_encrypt(newblock, dlen + 2, &newblock2len);
	write(socket, newblock2, newblock2len);
	free(newblock2);
	free(newblock);
}

struct _wait_poll_data
{
	cduck_message_callback callback;
	void* udata;
	int fd;
	_Bool shouldExit;
};

static void blocking_read ( int fd, _Bool shouldExit, void* data, size_t len )
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
			if (shouldExit)
				pthread_exit(NULL);
			else
			{
				memset(data, 0, len);
				return;
			}
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

static void* wait_poll ( void* udata )
{
	struct _wait_poll_data* wpd = udata;
	unsigned int length1, length2;
	void* tempData;
	void* finalData;
	blocking_read(wpd->fd, wpd->shouldExit, &length1, 4);
	length1 = ntohl(length1);
	tempData = malloc(length1);
	blocking_read(wpd->fd, wpd->shouldExit, tempData, length1);
	finalData = cduck_decrypt(tempData, length1, &length2);
	if (wpd->callback)
		wpd->callback(wpd->udata, finalData, length2);
	free(finalData);
	free(tempData);
	close(wpd->fd);
	free(udata);
}

void cduck_message ( const char* host, unsigned short msgid, const void* data, unsigned int dlen, cduck_message_callback callback, void* udata, _Bool synchronous )
{
	int fd, rc;
	struct _wait_poll_data* wpd;
	struct sockaddr_in addr;
	pthread_t thread;
	fd = socket(AF_INET, SOCK_STREAM, 0);
	addr.sin_len = sizeof(addr);
	addr.sin_family = AF_INET;
	addr.sin_port = 0;
	addr.sin_addr.s_addr = INADDR_ANY;
	rc = bind(fd, (struct sockaddr*)&addr, sizeof(addr));
	assert(rc == 0);
	addr.sin_port = htons(33940);
	addr.sin_addr.s_addr = inet_addr(host);
	rc = connect(fd, (struct sockaddr*)&addr, sizeof(addr));
	assert(rc == 0);
	send_packet(fd, msgid, data, dlen);
	wpd = malloc(sizeof(*wpd));
	wpd->fd = fd;
	wpd->udata = udata;
	wpd->callback = callback;
	if (synchronous)
	{
		wpd->shouldExit = 0;
		wait_poll(wpd);
	}
	else
	{
		wpd->shouldExit = 1;
		pthread_create(&thread, NULL, wait_poll, wpd);
		pthread_detach(thread);
	}
}

