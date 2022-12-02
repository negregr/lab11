#include "database.h"

#include <libpq-fe.h>

static PGconn *conn = NULL;
static PGresult *query_result = NULL;

int connect_to_psql_server(const char *login, const char *passwd)
{
	PGconn *pg_conn = PQsetdbLogin(NULL, NULL, NULL, NULL, 
		"lab7", login, passwd);
	ConnStatusType status = PQstatus(pg_conn);
	
	if (status == CONNECTION_OK) {
		conn = pg_conn;
		return 0;
	}

	return 1;
}

PGresult *send_query_to_server(const char *query)
{
	PGresult *result = PQexec(conn, query);
	ExecStatusType result_status = PQresultStatus(result);
	
	if (result_status == PGRES_TUPLES_OK) {
		query_result = result;
	}

	return result;
}

