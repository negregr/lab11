#ifndef DATABASE_H
#define DATABASE_H

#include <libpq-fe.h>

int connect_to_psql_server(const char *login, const char *passwd);
PGresult *send_query_to_server(const char *query);

#endif

