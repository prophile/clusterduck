#include "Commands.h"
#include "Message.h"
#include "Database.h"
#include <stdio.h>
#include <string.h>

static void command_package_operation ( const cduck_command* command, int argc, const char** argv )
{
	const char* package;
	const char** ips;
	int package_data_len;
	int i, j;
	if (argc == 0)
	{
		printf("You must specify a package.\n");
		return;
	}
	if (argc == 1)
	{
		printf("You must specify at least one group.\n");
		return;
	}
	package = argv[0];
	package_data_len = strlen(package);
	cduck_db_open();
	for (i = 1; i < argc; i++)
	{
		ips = cduck_db_get_group(argv[i]);
		j = 0;
		while (ips[j][0] != 0)
		{
			cduck_message(ips[j], command->id, package, package_data_len, NULL, NULL, 0);
			j++;
		}
	}
	cduck_db_close();
}

CDUCK_REGISTER_COMMAND_STATIC(pkginstall, 0x6000, command_package_operation, NULL, "install a package");
CDUCK_REGISTER_COMMAND_STATIC(pkgremove,  0x6001, command_package_operation, NULL, "remove a package");
