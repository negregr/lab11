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
static int gl_row_count = 0;
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
	gchar *column_num;
	char tmp[5];
	if (gl_row_count > 0) {
		if (is_clicked) {
			column_num = gtk_tree_model_get_string_from_iter(gl_model, &gl_iter);
			gtk_list_store_remove(GTK_LIST_STORE(gl_model), &gl_iter);
			if (atoi(column_num) == gl_row_count - 1) {
				sprintf(tmp, "%d", atoi(column_num) - 1);
				if (gl_row_count != 1) {
					gtk_tree_model_get_iter_from_string(gl_model, &gl_iter, tmp);
				}
			}
			gl_row_count -= 1;
		}
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
	gl_row_count += 1;
}

static void send_new_data(GtkWidget *widget, gpointer user_data)
{
	GtkTreeIter iter;
	gchar *tmp;
	GValue *g_value;
	char buff[2];
	gchar *table_name = (gchar *) user_data;
	char tmp_value[512];

	GtkTreePath *path;

	sprintf(query, "INSERT INTO %s VALUES ", table_name);
	GString *query_values = g_string_new(query);

	for (int i = 0; i < gl_row_count; i++) {
		sprintf(buff, "%d", i);
		path = gtk_tree_path_new_from_string(buff);
		gtk_tree_model_get_iter(gl_model, &iter, path);
		for (int j = 0; j < gl_column_count; j++) {
			gtk_tree_model_get(gl_model, &iter, j, &tmp, -1);
			if (j == 0) {
				g_string_append(query_values, "(");
			}
			if (j == gl_column_count - 1) {
				if (i == gl_row_count - 1) {
					sprintf(tmp_value, "\'%s\');", tmp);
				} else {
					sprintf(tmp_value, "\'%s\'),", tmp);
				}
			} else {
				sprintf(tmp_value, "\'%s\', ", tmp);
			}
			g_string_append(query_values, tmp_value);
		}
	}
	sprintf(query, "DELETE FROM %s;", table_name);
	PGresult *query_result = send_query_to_server(query);
	char *error_message = PQresultErrorMessage(query_result);
	if (error_message != NULL && strlen(error_message) > 0) {
		g_print("%s\n", error_message);
		free(error_message);
	}

	query_result = send_query_to_server(query_values->str);
	error_message = PQresultErrorMessage(query_result);
	if (error_message != NULL && strlen(error_message) > 0) {
		g_print("%s\n", error_message);
		free(error_message);
	}
	g_string_free(query_values, 1);
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
	GObject *column_list = gtk_builder_get_object(builder, "column_list");

	GObject *button = gtk_builder_get_object(builder, "button_add");
	g_signal_connect(button, "clicked", G_CALLBACK(add_new_row), NULL);
	button = gtk_builder_get_object(builder, "button_delete");
	g_signal_connect(button, "clicked", G_CALLBACK(delete_row), NULL);
	button = gtk_builder_get_object(builder, "button_change");
	g_signal_connect(button, "clicked", G_CALLBACK(send_new_data), (gpointer) template_name->str);
	
	GString *result_str = g_string_new("Таблица \"");
	g_string_append(result_str, template_name->str);
	g_string_append(result_str, "\"");
	gtk_label_set_label(GTK_LABEL(label), result_str->str);

	g_signal_connect(window, "destroy", G_CALLBACK(destroy_table_template), result_str->str);

	sprintf(query, "SELECT * FROM %s;", template_name->str);

	PGresult *query_result = send_query_to_server(query);
	
	int row_count = PQntuples(query_result);
	gl_row_count = row_count;
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
		gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(column_list), NULL, PQfname(query_result, i));
	}
	gtk_combo_box_set_active(GTK_COMBO_BOX(column_list), 0);

	gtk_tree_view_set_model(GTK_TREE_VIEW(tree_view), GTK_TREE_MODEL(store));
	gtk_tree_view_set_grid_lines(GTK_TREE_VIEW(tree_view), 3);
	gtk_tree_view_set_activate_on_single_click(GTK_TREE_VIEW(tree_view), 1);
	gtk_container_add(GTK_CONTAINER(table_view), GTK_WIDGET(tree_view));
	gl_model = GTK_TREE_MODEL(store);
	
	gl_store = store;
	gl_column_count = column_count;

	return GTK_WIDGET(window);
}

