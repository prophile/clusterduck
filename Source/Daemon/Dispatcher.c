#include "Dispatcher.h"

static cduck_dispatcher dispatchers[65536];

void cduck_register_dispatcher ( unsigned short msgid, cduck_dispatcher dispatcher )
{
	dispatchers[msgid] = dispatcher;
}

// this is tail-call optimised
void* cduck_dispatch ( unsigned short msgid, const void* data, unsigned int len, unsigned int* outLen )
{
	cduck_dispatcher dispatcher = dispatchers[msgid];
	if (!dispatcher)
	{
		*outLen = 0;
		return 0;
	}
	return dispatcher(msgid, data, len, outLen);
}

