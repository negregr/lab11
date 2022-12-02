#include "table_template_frame.h"
#include "database.h"
#include <libpq-fe.h>
#include <string.h>

typedef struct {
	int r_c_count[2];
	GtkListStore *store;
} callback_data;

static char query[2048];

static GtkListStore *gl_store = NULL;
static int gl_column_count = 0;
static GtkTreeModel *gl_model = NULL;
static GtkTreeIter gl_iter;
static char is_clicked = 0;

static void destroy_table_template(GtkWidget *widget, gpointer data)
{
	gtk_widget_destroy(widget);
	gchar *table_name = (gchar *) data;
	g_print("%s закрыта\n", table_name);
}

static void cell_edit_callback(GtkWidget *widget, gchar *path_string, gchar *new_text, gpointer user_data)
{
	GtkCellRenderer *cell = GTK_CELL_RENDERER(widget);
	guint column_number = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(cell), "my_column_num"));

	GtkTreeModel *model = user_data;
	GtkTreeIter iter;
	gtk_tree_model_get_iter_from_string(model, &iter, path_string);

	GtkListStore *model_raw = GTK_LIST_STORE(model);

	gtk_list_store_set(model_raw, &iter, column_number, new_text, -1);
}

static void cell_clicked(GtkTreeView *self, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data)
{
	gtk_tree_model_get_iter(gl_model, &gl_iter, path);
	is_clicked = 1;
}

static void delete_row(GtkWidget *widget, gpointer user_data)
{
	GtkTreeIter iter;
	if (is_clicked) {
		gtk_list_store_remove(GTK_LIST_STORE(gl_model), &gl_iter);
	}
}

static void add_new_row(GtkWidget *widget, gpointer user_data)
{
	callback_data *data = (callback_data *) user_data;
	GtkTreeIter iter;
	
	GValue g_value[1000];
	GType g_type;
	gint columns[1000];
	GValue tmp;

	GtkCellRenderer *renderer;
	
	g_type = G_TYPE_STRING;

	for (unsigned int i = 0; i < gl_column_count; i++) {
		columns[i] = i;
		memset(&tmp, 0, sizeof(GValue));
		g_value_init(&tmp, g_type);
		g_value_set_string(&tmp, "");
		memcpy(&g_value[i], &tmp, sizeof(GValue));
	}

	gtk_list_store_insert_with_valuesv(gl_store, &iter, -1, columns,
		g_value, gl_column_count);
}

static void send_new_data(GtkWidget *widget, gpointer user_data)
{

}

GtkWidget *create_new_template(const GString *template_name, void *data)
{
	memset(&gl_iter, 0, sizeof(gl_iter));
	GtkBuilder *builder = gtk_builder_new();
	GError *error = NULL;
	
	if (gtk_builder_add_from_file(builder, UI_DIR "table_template.ui", &error) == 0) {
		g_printerr("Error loading table_template.ui file: %s\n", error->message);
		g_clear_error(&error);
		return NULL;
	}

	GObject *window = gtk_builder_get_object(builder, "table_template");
	GObject *label = gtk_builder_get_object(builder, "table_name");
	GObject *table_view = gtk_builder_get_object(builder, "table_view");
	GObject *button = gtk_builder_get_object(builder, "button_add");
	
	GString *result_str = g_string_new("Таблица \"");
	g_string_append(result_str, template_name->str);
	g_string_append(result_str, "\"");
	gtk_label_set_label(GTK_LABEL(label), result_str->str);

	g_signal_connect(window, "destroy", G_CALLBACK(destroy_table_template), result_str->str);

	sprintf(query, "SELECT * FROM %s;", template_name->str);

	PGresult *query_result = send_query_to_server(query);
	
	int row_count = PQntuples(query_result);
	int column_count = PQnfields(query_result);
	int r_c_count[2] = {row_count, column_count};

	GType g_type[100];
	GValue g_value[1000];
	gint columns[100];

	GtkCellRenderer *renderer;

	for (unsigned int i = 0; i < (unsigned int) column_count; i++) {
		g_type[i] = G_TYPE_STRING;
		columns[i] = i;
	}

	GtkListStore *store = gtk_list_store_newv(column_count, g_type);
	GValue tmp;
	GtkTreeIter iter;

	for (unsigned int i = 0; i < (unsigned int) row_count; i++) {
		for (unsigned int j = 0; j < (unsigned int) column_count; j++) {
			memset(&tmp, 0, sizeof(GValue));
			g_value_init(&tmp, G_TYPE_STRING);
			g_value_set_string(&tmp, PQgetvalue(query_result, i, j));
			memcpy(&g_value[j], &tmp, sizeof(GValue));
		}

		gtk_list_store_insert_with_valuesv(store, &iter, -1, columns, g_value, column_count);
	}
	
	GtkWidget *tree_view = gtk_tree_view_new();

	g_signal_connect(button, "clicked", G_CALLBACK(add_new_row), NULL);
	
	gtk_widget_set_focus_on_click(tree_view, 1);

	for (unsigned int i = 0; i < (unsigned int) column_count; i++) {
		renderer = gtk_cell_renderer_text_new();
		g_object_set(renderer, "editable", TRUE, NULL);
		g_object_set_data(G_OBJECT(renderer), "my_column_num", GUINT_TO_POINTER(i));
		g_signal_connect(renderer, "edited", G_CALLBACK(cell_edit_callback), GTK_TREE_MODEL(store));
		g_signal_connect(GTK_TREE_VIEW(tree_view), "row-activated", G_CALLBACK(cell_clicked), NULL);
		gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view),
			-1,
		   	PQfname(query_result, i), renderer,
			"text", i,
			NULL
		);
	}

	gtk_tree_view_set_model(GTK_TREE_VIEW(tree_view), GTK_TREE_MODEL(store));
	gtk_tree_view_set_grid_lines(GTK_TREE_VIEW(tree_view), 3);
	gtk_tree_view_set_activate_on_single_click(GTK_TREE_VIEW(tree_view), 1);
	gtk_container_add(GTK_CONTAINER(table_view), GTK_WIDGET(tree_view));
	gl_model = GTK_TREE_MODEL(store);
	
	gl_store = store;
	gl_column_count = column_count;

	button = gtk_builder_get_object(builder, "button_delete");
	g_signal_connect(button, "clicked", G_CALLBACK(delete_row), NULL);
	button = gtk_builder_get_object(builder, "button_change");
	g_signal_connect(button, "clicked", G_CALLBACK(send_new_data), NULL);

	return GTK_WIDGET(window);
}

