#include <gtk/gtk.h>
#include <stdio.h>
#include "main_frame.h"

void activate(GtkApplication *app, gpointer data)
{
	GtkWidget *window = create_main_window(app);
	gtk_application_add_window(app, GTK_WINDOW(window));
	gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char **argv)
{
	GtkApplication *app = gtk_application_new("lab11.desktop", G_APPLICATION_FLAGS_NONE);
	g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
	g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);

	return 0;
}
