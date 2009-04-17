#include "Commands.h"
#include <string.h>
#include <stdlib.h>

static cduck_command* command_ring = NULL;

static unsigned int hash (const char* str, unsigned int len) __attribute__((pure, nothrow));

void cduck_register_command ( const cduck_command* command )
{
	// duplicate it
	cduck_command* dupe = malloc(sizeof(cduck_command));
	cduck_command* temp_next;
	dupe->udata = command->udata;
	dupe->name = strdup(command->name);
	dupe->hash = hash(dupe->name, strlen(dupe->name));
	dupe->id = command->id;
	dupe->handler = command->handler;
	if (command_ring)
	{
		temp_next = command_ring->next;
		command_ring->next = dupe;
		dupe->next = temp_next;
	}
	else
	{
		command_ring = dupe;
		command_ring->next = dupe;
	}
}

int cduck_dispatch_command ( const char* name, int argc, const char** argv )
{
	unsigned int nameHash;
	unsigned int firstHash = command_ring->hash;
	nameHash = hash(name, strlen(name));
	if (!command_ring) return 1;
	do
	{
		if (nameHash == command_ring->hash)
		{
			command_ring->handler(command_ring, argc, argv);
			return 0;
		}
		command_ring = command_ring->next;
	} while(command_ring->hash != firstHash);
	return 1;
}

static void __command_nop ( const cduck_command* command, int argc, const char** argv )
{
	// do nothing
}

CDUCK_REGISTER_COMMAND_STATIC(nop, 0x0000, __command_nop, 0);

static unsigned int hash (const char* str, unsigned int len)
{
	unsigned int hash = 0;
	unsigned int i;

	for (i = 0; i < len; i++)
		hash = str[i] + (hash << 6) + (hash << 16) - hash;

	return hash;
}
