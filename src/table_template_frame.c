#include "table_template_frame.h"

static void destroy_table_template(GtkWidget *widget, gpointer data)
{
	gtk_widget_destroy(widget);
	gchar *table_name = (gchar *) data;
	g_print("%s закрыта\n", table_name);
}

GtkWidget *create_new_template(const GString *template_name, void *data)
{
	GtkBuilder *builder = gtk_builder_new();
	GError *error = NULL;
	
	if (gtk_builder_add_from_file(builder, UI_DIR "table_template.ui", &error) == 0) {
		g_printerr("Error loading table_template.ui file: %s\n", error->message);
		g_clear_error(&error);
		return NULL;
	}

	GObject *window = gtk_builder_get_object(builder, "table_template");
	GObject *label = gtk_builder_get_object(builder, "table_name");
	
	GString *result_str = g_string_new("Таблица \"");
	g_string_append(result_str, template_name->str);
	g_string_append(result_str, "\"");
	gtk_label_set_label(GTK_LABEL(label), result_str->str);

	g_signal_connect(window, "destroy", G_CALLBACK(destroy_table_template), result_str->str);

	return GTK_WIDGET(window);
}

