#include <gtk/gtk.h>
#include <stdio.h>
#include "main_frame.h"

int main(int argc, char **argv)
{
	gtk_init(&argc, &argv);
	GtkWidget *main_window = create_main_window();
	gtk_widget_show_all(main_window);
	gtk_main();

	return 0;
}
