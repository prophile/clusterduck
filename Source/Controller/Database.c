#include "Database.h"
#include <sqlite3.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <fnmatch.h>
#include <stdlib.h>

static int lockFD = 0;
static sqlite3* db = NULL;

extern const char* cduck_db_schema;

#define DB_MAX_GROUP_COUNT 2048

static char* __db_groups[2048];

static void initgroups ()
{
	int i;
	for (i = 0; i < DB_MAX_GROUP_COUNT; i++)
	{
		__db_groups[i] = malloc(16);
	}
}

static void srcdie ()
{
	fprintf(stderr, "DB error: %s\n", db);
	exit(1);
}

static void csrc ( int rc )
{
	if (rc == SQLITE_DONE || rc == SQLITE_ROW) return;
	else srcdie();
}

static void initdb ()
{
	sqlite3_exec(db, cduck_db_schema, NULL, NULL, NULL);
}

void cduck_db_open ()
{
	FILE* fp = NULL;
	mkdir("/var/cduck", 0700);
	lockFD = open("/var/cduck/cduck.db.lock", O_RDWR | O_CREAT | O_EXLOCK);
	if (fp = fopen("/var/cduck/cduck.db", "r"))
	{
		fclose(fp);
	}
	sqlite3_open("/var/cduck/cduck.db", &db);
	assert(db);
	sqlite3_extended_result_codes(db, 1);
	if (fp == NULL) // this means we didn't open it earlier
	{
		initdb();
	}
	initgroups();
}

void cduck_db_close ()
{
	sqlite3_close(db);
	close(lockFD);
	lockFD = 0;
	db = NULL;
}

void cduck_db_add_host ( const char* ip )
{
	int rc;
	sqlite3_stmt* statement;
	sqlite3_prepare_v2(db, "INSERT INTO hosts(ip) VALUES (?1)", -1, &statement, NULL);
	sqlite3_bind_text(statement, 1, ip, strlen(ip), SQLITE_STATIC);
	rc = sqlite3_step(statement);
	csrc(rc);
	sqlite3_finalize(statement);
	sqlite3_prepare_v2(db, "INSERT INTO groups(name, ip) VALUES ('all', ?1)", -1, &statement, NULL);
	sqlite3_bind_text(statement, 1, ip, strlen(ip), SQLITE_STATIC);
	rc = sqlite3_step(statement);
	csrc(rc);
	sqlite3_finalize(statement);
}

void cduck_db_remove_host ( const char* ip )
{
	int rc;
	sqlite3_stmt* statement;
	sqlite3_prepare_v2(db, "DELETE FROM hosts WHERE ip = ?1", -1, &statement, NULL);
	sqlite3_bind_text(statement, 1, ip, strlen(ip), SQLITE_STATIC);
	rc = sqlite3_step(statement);
	csrc(rc);
	sqlite3_finalize(statement);
	sqlite3_prepare_v2(db, "DELETE FROM groups WHERE member = ?1", -1, &statement, NULL);
	sqlite3_bind_text(statement, 1, ip, strlen(ip), SQLITE_STATIC);
	rc = sqlite3_step(statement);
	csrc(rc);
	sqlite3_finalize(statement);
}

void cduck_db_join_group ( const char* group, const char* ip )
{
	int rc;
	sqlite3_stmt* statement;
	sqlite3_prepare_v2(db, "INSERT INTO groups(name, ip) VALUES (?1, ?2)", -1, &statement, NULL);
	sqlite3_bind_text(statement, 1, group, strlen(group), NULL);
	sqlite3_bind_text(statement, 2, ip, strlen(ip), SQLITE_STATIC);
	rc = sqlite3_step(statement);
	csrc(rc);
	sqlite3_finalize(statement);
}

void cduck_db_part_group ( const char* group, const char* ip )
{
	int rc;
	sqlite3_stmt* statement;
	sqlite3_prepare_v2(db, "DELETE FROM groups WHERE name = ?1 AND ip = ?1", -1, &statement, NULL);
	sqlite3_bind_text(statement, 1, group, strlen(group), SQLITE_STATIC);
	sqlite3_bind_text(statement, 2, ip, strlen(ip), SQLITE_STATIC);
	rc = sqlite3_step(statement);
	csrc(rc);
	sqlite3_finalize(statement);
}

void cduck_db_update_statistics ( const char* ip, time_t update_time, const cduck_db_host_stats* statistics )
{
	int rc;
	sqlite3_stmt* statement;
	sqlite3_prepare_v2(db, "UPDATE hosts SET stat_time = ?1, cpu_count = ?2, cpu_power = ?3, cpu_usage = ?4, available_ram = ?5, used_ram = ?6", -1, &statement, NULL);
	sqlite3_bind_int64(statement, 1, update_time);
	sqlite3_bind_int64(statement, 2, statistics->cpu_count);
	sqlite3_bind_double(statement, 3, statistics->cpu_power);
	sqlite3_bind_double(statement, 4, statistics->cpu_usage);
	sqlite3_bind_int64(statement, 5, statistics->available_ram);
	sqlite3_bind_int64(statement, 6, statistics->used_ram);
	rc = sqlite3_step(statement);
	csrc(rc);
	sqlite3_finalize(statement);
}

int cduck_db_fetch_statistics ( const char* ip, time_t minimum, cduck_db_host_stats* statistics )
{
	int rc;
	sqlite3_stmt* statement;
	sqlite3_prepare_v2(db, "SELECT cpu_count, cpu_power, cpu_usage, available_ram, used_ram FROM hosts WHERE ip = ?1 AND stat_time > ?2", -1, &statement, NULL);
	sqlite3_bind_text(statement, 1, ip, strlen(ip), SQLITE_STATIC);
	sqlite3_bind_int64(statement, 2, minimum);
	if (sqlite3_step(statement) == SQLITE_ROW)
	{
		rc = 0;
		statistics->cpu_count = sqlite3_column_int64(statement, 1);
		statistics->cpu_power = sqlite3_column_double(statement, 2);
		statistics->cpu_usage = sqlite3_column_double(statement, 3);
		statistics->available_ram = sqlite3_column_int64(statement, 4);
		statistics->used_ram = sqlite3_column_int64(statement, 5);
	}
	else
	{
		rc = 1;
	}
	sqlite3_finalize(statement);
	return rc;
}

const char** cduck_db_get_group ( const char* group )
{
	int i = 0;
	int n;
	sqlite3_stmt* statement;
	sqlite3_prepare_v2(db, "SELECT ip FROM groups WHERE name = ?1", -1, &statement, NULL);
	sqlite3_bind_text(statement, 1, group, strlen(group), SQLITE_STATIC);
	while (sqlite3_step(statement) == SQLITE_ROW)
	{
		strcpy(__db_groups[i], (const char*)sqlite3_column_text(statement, 1));
		i++;
	}
	if (i == 0)
	{
		strcpy(__db_groups[0], group);
		__db_groups[1][0] = 0;
	}
	else
	{
		__db_groups[i][0] = '\0';
	}
	sqlite3_finalize(statement);
	return (const char**)__db_groups;
}

const char** cduck_db_select_horsepower ( const char* group, int n )
{
	int i = 0;
	sqlite3_stmt* statement;
	sqlite3_prepare_v2(db, "SELECT hosts.ip FROM hosts, groups WHERE hosts.stat_time != 0 AND group.name == ?1 AND group.ip = hosts.ip ORDER BY hosts.cpu_power DESC", -1, &statement, NULL);
	sqlite3_bind_text(statement, 1, group, strlen(group), SQLITE_STATIC);
	while (i < n && sqlite3_step(statement) == SQLITE_ROW)
	{
		strcpy(__db_groups[i], (const char*)sqlite3_column_text(statement, 1));
		i++;
	}
	__db_groups[i][0] = '\0';
	sqlite3_finalize(statement);
	return (const char**)__db_groups;
}
