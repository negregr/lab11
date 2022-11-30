#include "main_frame.h"
#include "login_frame.h"
#include "table_template_frame.h"
#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

static GString *table_names[TABLE_NAMES_COUNT];
static uint8_t is_first_time = 1;
static GtkWidget *main_window;

static void init_table_names()
{
	table_names[0] = g_string_new("Кафедра");
	table_names[1] = g_string_new("Преподаватель");
	table_names[2] = g_string_new("Занятие");
	table_names[3] = g_string_new("Предмет");
	table_names[4] = g_string_new("Аудитория");
	table_names[5] = g_string_new("Группа");
}

static void table_button_create(GtkWidget *widget, gpointer user_data)
{
	GString *table_name = (GString *) user_data;
	GtkWidget *window = create_new_template(table_name, NULL);
	gtk_window_set_transient_for(GTK_WINDOW(window), GTK_WINDOW(main_window));
	g_print("Открыта таблица \"%s\"\n", table_name->str);
	gtk_widget_show_all(window);
}

static void change_user_button_clicked(GtkWidget *widget, gpointer user_data)
{
	GtkWidget* login_frame = create_login_frame(GTK_WINDOW(main_window));
	gtk_window_set_transient_for(GTK_WINDOW(login_frame), GTK_WINDOW(main_window));
	gtk_window_present(GTK_WINDOW(login_frame));
}

static void destroy_app(GtkWidget *widget, gpointer user_data)
{
	g_application_quit(G_APPLICATION(user_data));
}

GtkWidget *create_main_window(GtkApplication *app)
{
	if (is_first_time) {
		init_table_names();
	}

	GtkBuilder *builder = gtk_builder_new();
	GError *error = NULL;
	
	if (gtk_builder_add_from_file(builder, UI_DIR "main_window.ui", &error) == 0) {
		g_printerr("Error loading file: %s\n", error->message);
		g_clear_error(&error);
		return NULL;
	}

	GObject *window = gtk_builder_get_object(builder, "main_window");
	main_window = GTK_WIDGET(window);

	GString *name;
	GString *button_id;

	GObject *button;
	char num[2];

	for (uint8_t i = 0; i < TABLE_NAMES_COUNT; i++) {
		button_id = g_string_new("button");
		sprintf(num, "%d", i + 1);
		button_id = g_string_append(button_id, num);
		button = gtk_builder_get_object(builder, button_id->str);
		name = table_names[i];
		g_signal_connect(button, "clicked", G_CALLBACK(table_button_create), (gpointer) name);
		g_string_free(button_id, TRUE);
	}

	button = gtk_builder_get_object(builder, "button8");
	g_signal_connect(button, "clicked", G_CALLBACK(destroy_app), (gpointer) app);
	button = gtk_builder_get_object(builder, "button7");
	g_signal_connect(button, "clicked", G_CALLBACK(change_user_button_clicked), NULL);
	g_signal_connect(main_window, "destroy", G_CALLBACK(destroy_app), (gpointer) app);

	return GTK_WIDGET(window);
}

