#include "Commands.h"
#include "Message.h"
#include "Database.h"
#include <stdio.h>

static void command_shell ( const cduck_command* command, int argc, const char** argv )
{
	char shell_command[2048];
	const char* group;
	const char** ips;
	const char* thisArg;
	char ch;
	int i, chidx = 0;
#define PUSHCHAR(x) shell_command[chidx++] = x
	if (argc == 0)
	{
		printf("You must specify a group.\n");
		return;
	}
	else if (argc == 1)
	{
		printf("You must specify a command.\n");
		return;
	}
	group = argv[0];
	for (i = 1; i < argc; i++)
	{
		thisArg = argv[i];
		PUSHCHAR('\'');
		while (ch = *thisArg++)
		{
			if (ch == '\'' || ch == '\\')
			{
				PUSHCHAR('\\');
			}
			PUSHCHAR(ch);
		}
		PUSHCHAR('\'');
	}
	shell_command[chidx] = 0;
	cduck_db_open();
	ips = cduck_db_get_group(group);
	i = 0;
	while (ips[i][0] != 0)
	{
		printf("Sending SHELL to client %s\n", ips[i]);
		cduck_message(ips[i], command->id, shell_command, chidx, NULL, NULL, 0);
		i++;
	}
	cduck_db_close();
}

CDUCK_REGISTER_COMMAND_STATIC(shell, 0x2000, command_shell, NULL, "sends a shell command");
