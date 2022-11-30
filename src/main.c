#include <gtk/gtk.h>
#include <stdio.h>
#include <libpq-fe.h>
#include "main_frame.h"
#include "login_frame.h"
#include "database.h"

static void activate(GtkApplication *app, gpointer data)
{
	GtkWidget *main_window = create_main_window(app);
	gtk_application_add_window(app, GTK_WINDOW(main_window));
	gtk_window_present(GTK_WINDOW(main_window));

	GtkWidget *login_window = create_login_frame(GTK_WINDOW(main_window));
	gtk_window_set_transient_for(GTK_WINDOW(login_window), GTK_WINDOW(main_window));

	gtk_window_present(GTK_WINDOW(login_window));
}

int main(int argc, char **argv)
{
	GtkApplication *app = gtk_application_new("lab11.desktop", G_APPLICATION_FLAGS_NONE);
	g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
	g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);

	return 0;
}
