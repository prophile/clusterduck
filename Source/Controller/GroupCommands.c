#include "Commands.h"
#include "Database.h"

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

CDUCK_REGISTER_COMMAND_STATIC(addhost, 0xF000, command_addhost, NULL, "adds one or more hosts to the DB");
CDUCK_REGISTER_COMMAND_STATIC(delhost, 0xF001, command_delhost, NULL, "removes one or more hosts from the DB");
