#include "login_frame.h"
#include "database.h"
#include <gtk/gtk.h>

typedef struct {
	GtkWindow *parent;
	GObject *login_frame;
} callback_data;

static GtkButton *login_button = NULL;
static GtkEntry *login_entry = NULL;
static GtkEntry *passwd_entry = NULL;

static callback_data cb_data;

static uint8_t is_connected = 0U;

static void destroy_login_frame(GtkWidget *widget, gpointer user_data)
{
	gtk_widget_destroy(widget);

	if (!is_connected) {
		gtk_widget_destroy(GTK_WIDGET(cb_data.parent));
	}
	gtk_widget_set_sensitive(GTK_WIDGET(cb_data.parent), 1);
}

static void login_button_clicked(GtkButton *button, gpointer user_data)
{
	const gchar *login_text = gtk_entry_get_text(login_entry);
	const gchar *passwd_text = gtk_entry_get_text(passwd_entry);

	g_print("Подключение к базе данных lab7...\n");

	if (connect_to_psql_server(login_text, passwd_text) == 0) {
		is_connected = 1U;
		g_print("Подключение прошло успешно!\n");

		gtk_widget_destroy(GTK_WIDGET(((callback_data *) user_data)->login_frame));
		gtk_widget_set_sensitive(GTK_WIDGET(((callback_data *) user_data)->parent), 1);
	} else {
		g_print("Подключение не удалось...\n");
		gtk_entry_set_text(login_entry, "");
		gtk_entry_set_text(passwd_entry, "");
	}
}

GtkWidget *create_login_frame(GtkWindow *parent)
{
	GtkBuilder *builder = gtk_builder_new();
	GError *error = NULL;

	if (gtk_builder_add_from_file(builder, UI_DIR "login_window.ui", &error) == 0) {
		g_printerr("Error loading login_window.ui file: %s\n", error->message);
		g_clear_error(&error);
		return NULL;
	}

	gtk_widget_set_sensitive(GTK_WIDGET(parent), 0);

	GObject *window = gtk_builder_get_object(builder, "login_window");
	GObject *button = gtk_builder_get_object(builder, "login_button");

	GObject *login  = gtk_builder_get_object(builder, "login_enter");
	GObject *passwd = gtk_builder_get_object(builder, "password_enter");

	cb_data.parent = parent;
	cb_data.login_frame = window;

	g_signal_connect(GTK_BUTTON(button), "clicked", G_CALLBACK(login_button_clicked), &cb_data);
	g_signal_connect(GTK_WIDGET(window), "destroy", G_CALLBACK(destroy_login_frame), &cb_data.parent);

	login_button = GTK_BUTTON(button);
	login_entry  = GTK_ENTRY(login);
	passwd_entry = GTK_ENTRY(passwd);

	return GTK_WIDGET(window);
}

