#include "main_frame.h"
#include <gtk/gtk.h>

GtkWidget *create_main_window()
{
	GtkBuilder *builder = gtk_builder_new();
	GError *error = NULL;
	
	if (gtk_builder_add_from_file(builder, UI_DIR "main_window.ui", &error) == 0) {
		g_printerr("Error loading file: %s\n", error->message);
		g_clear_error(&error);
		return NULL;
	}

	GObject *window = gtk_builder_get_object(builder, "main_window");
//	GObject *box;
//	GObject *label;
//	GObject *button;

	return GTK_WIDGET(window);
}

