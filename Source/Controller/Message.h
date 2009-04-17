#ifndef __included_message_h
#define __included_message_h

#include <stdbool.h>

typedef void (*cduck_message_callback)(void* udata, const void* data, unsigned int dlen);
void cduck_message ( const char* host, unsigned short msgid, const void* data, unsigned int dlen, cduck_message_callback callback, void* udata, _Bool synchronous );

#endif
