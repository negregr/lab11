#ifndef DATABASE_H
#define DATABASE_H

#include <libpq-fe.h>

int connect_to_psql_server(const char *login, const char *passwd);

#endif

