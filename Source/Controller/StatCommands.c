#include "Commands.h"
#include "Message.h"
#include "Database.h"
#include <string.h>
#include <alloca.h>
#include <time.h>
#include <stdio.h>

static void stat_update ( void* host, const void* data, unsigned int dlen )
{
	char* bytes;
	cduck_db_host_stats stats;
	bytes = alloca(dlen + 1);
	memcpy(bytes, data, dlen);
	bytes[dlen] = 0;
	sscanf(bytes, "{%llu, %f, %f, %lld, %lld}", &stats.cpu_count, &stats.cpu_usage, &stats.cpu_power, &stats.available_ram, &stats.used_ram);
	cduck_db_update_statistics((const char*)host, time(NULL), &stats);
}

static void command_stat ( const cduck_command* command, int argc, const char** argv )
{
	const char** ips;
	int i, j;
	cduck_db_open();
	for (i = 0; i < argc; i++)
	{
		j = 0;
		ips = cduck_db_get_group(argv[i]);
		while (ips[j][0] != 0)
		{
			printf("Sending STAT to client %s\n", ips[i]);
			cduck_message(ips[j], command->id, NULL, 0, stat_update, (void*)ips[j], 1);
			j++;
		}
	}
	cduck_db_close();
}

CDUCK_REGISTER_COMMAND_STATIC(stat, 0x1000, command_stat, NULL, "updates statistics");
