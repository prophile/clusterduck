#ifndef __included_database_h
#define __included_database_h

#include <time.h>
#include <inttypes.h>

void cduck_db_open ();
void cduck_db_close ();

void cduck_db_add_host ( const char* ip );
void cduck_db_remove_host ( const char* ip );

void cduck_db_join_group ( const char* group, const char* ip );
void cduck_db_part_group ( const char* group, const char* ip );

typedef struct _cduck_db_host_stats
{
	uint64_t cpu_count;
	float cpu_usage;
	float cpu_power;
	uint64_t available_ram;
	uint64_t used_ram;
} cduck_db_host_stats;

void cduck_db_update_statistics ( const char* ip, time_t update_time, const cduck_db_host_stats* statistics );
int cduck_db_fetch_statistics ( const char* ip, time_t minimum, cduck_db_host_stats* statistics );

const char** cduck_db_get_group ( const char* group );
const char** cduck_db_select_random ( const char* group, int n );
const char** cduck_db_select_horsepower ( const char* group, int n );
const char** cduck_db_select_memory ( const char* group, int n );
const char** cduck_db_select_generic ( const char* group, int n );

#endif