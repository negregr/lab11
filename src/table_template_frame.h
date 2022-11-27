#ifndef TABLE_TEMPLATE_H

#include <gtk/gtk.h>

#define TABLE_NAMES_COUNT (6U)

static GString *table_names[TABLE_NAMES_COUNT];

GtkWidget *create_new_template(const GString *template_name, void *data);

#endif

