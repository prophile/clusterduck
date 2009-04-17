#include "Database.h"

int main ()
{
	cduck_db_open();
	cduck_db_add_host("192.168.0.2");
	cduck_db_join_group("webservers", "192.168.0.2");
	cduck_db_close();
	return 0;
}
