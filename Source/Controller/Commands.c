#include "Commands.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static cduck_command* command_ring = NULL;

static unsigned int hash (const char* str, unsigned int len) __attribute__((pure, nothrow));

void cduck_register_command ( const cduck_command* command )
{
	// duplicate it
	cduck_command* dupe = malloc(sizeof(cduck_command));
	cduck_command* temp_next;
	dupe->udata = command->udata;
	dupe->name = strdup(command->name);
	dupe->tooltip = strdup(command->tooltip);
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
	unsigned int firstHash;
	if (!command_ring) return 1;
	nameHash = hash(name, strlen(name));
	firstHash = command_ring->hash;
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

void cduck_command_print_usage ()
{
	unsigned int firstHash;
	if (!command_ring) return;
	firstHash = command_ring->hash;
	do
	{
		printf("\t%s - %s\n", command_ring->name, command_ring->tooltip);
		command_ring = command_ring->next;
	} while (command_ring->hash != firstHash);
}

static void command_nop ( const cduck_command* command, int argc, const char** argv )
{
	// do nothing
}

CDUCK_REGISTER_COMMAND_STATIC(nop, 0x0000, command_nop, 0, "does nothing");

static unsigned int hash (const char* str, unsigned int len)
{
	unsigned int hash = 0;
	unsigned int i;

	for (i = 0; i < len; i++)
		hash = str[i] + (hash << 6) + (hash << 16) - hash;

	return hash;
}
