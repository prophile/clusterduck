#include "Commands.h"
#include "Database.h"
#include <stdio.h>

static void command_addhost ( const cduck_command* command, int argc, const char** argv )
{
	int i;
	cduck_db_open();
	for (i = 0; i < argc; i++)
	{
		cduck_db_add_host(argv[i]);
	}
	cduck_db_close();
}

static void command_delhost ( const cduck_command* command, int argc, const char** argv )
{
	int i;
	cduck_db_open();
	for (i = 0; i < argc; i++)
	{
		cduck_db_remove_host(argv[i]);
	}
	cduck_db_close();
}

static void command_join ( const cduck_command* command, int argc, const char** argv )
{
	int i;
	const char* group;
	if (argc == 0)
	{
		printf("You must specify a group.\n");
		return;
	}
	if (argc == 1)
	{
		printf("You must specify at least one host.\n");
		return;
	}
	group = argv[0];
	cduck_db_open();
	for (i = 1; i < argc; i++)
	{
		cduck_db_join_group(group, argv[i]);
	}
	cduck_db_close();
}

static void command_part ( const cduck_command* command, int argc, const char** argv )
{
	int i;
	const char* group;
	if (argc == 0)
	{
		printf("You must specify a group.\n");
		return;
	}
	if (argc == 1)
	{
		printf("You must specify at least one host.\n");
		return;
	}
	group = argv[0];
	cduck_db_open();
	for (i = 1; i < argc; i++)
	{
		cduck_db_part_group(group, argv[i]);
	}
	cduck_db_close();
}

CDUCK_REGISTER_COMMAND_STATIC(addhost, 0xF000, command_addhost, NULL, "adds one or more hosts to the DB");
CDUCK_REGISTER_COMMAND_STATIC(delhost, 0xF001, command_delhost, NULL, "removes one or more hosts from the DB");
CDUCK_REGISTER_COMMAND_STATIC(join,    0xF002, command_join,    NULL, "makes one or more hosts part of a workgroup");
CDUCK_REGISTER_COMMAND_STATIC(part,    0xF003, command_part,    NULL, "makes one or more hosts leave a workgroup");
