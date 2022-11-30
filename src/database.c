#include "database.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <libpq-fe.h>

int connect_to_psql_server(const char *login, const char *passwd)
{
	PGconn *pg_conn = PQsetdbLogin(NULL, NULL, NULL, NULL, 
		"lab7", login, passwd);
	ConnStatusType status = PQstatus(pg_conn);
	char *db = PQdb(pg_conn);
	char *user = PQuser(pg_conn);
	char *pass = PQpass(pg_conn);

	
	if (status == CONNECTION_OK) {
		return 0;
	}

	return 1;
}

