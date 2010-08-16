#include "mapmake.h"

int show_dialog(param_type *p)
{
	int argc = 1;
	char **argv;
	int go = FALSE;

	/* widgets usados neste dialogo */
	GtkWidget *win, *chk, *ok, *cancel, *tmp;

	argv = g_new(gchar *, 1);
	argv[0] = "mapmake";

	/* inicializa o gtk */
	gtk_init(&argc, &argv);

	win = gtk_dialog_new();
	gtk_window_set_title(GTK_WINDOW(win), "Maping tools");
	gtk_signal_connect(GTK_OBJECT(win), "delete_event", GTK_SIGNAL_FUNC(gtk_main_quit), NULL);
	
	ok = gtk_button_new_with_label("Ok");
	gtk_signal_connect(GTK_OBJECT(ok), "clicked", GTK_SIGNAL_FUNC(okay), &go);

	cancel = gtk_button_new_with_label("Cancel");
	gtk_signal_connect(GTK_OBJECT(cancel), "clicked", GTK_SIGNAL_FUNC(gtk_main_quit), NULL);

	/* box com os botões */
	tmp = gtk_hbutton_box_new();

	gtk_box_pack_start(GTK_BOX(tmp), ok, FALSE, FALSE, 5);
	gtk_box_pack_end(GTK_BOX(tmp), cancel, FALSE, FALSE, 5);

	gtk_box_pack_end(GTK_BOX(GTK_DIALOG(win)->action_area), tmp, FALSE, FALSE, 5);

	chk = gtk_check_button_new_with_label("Use 8 pixels");

	if (*p == VIZ8)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(chk), TRUE);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(chk), FALSE);

	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(win)->vbox), chk, FALSE, FALSE, 5);

	gtk_widget_show_all(win);

	gtk_main();

	gtk_widget_hide_all(win);

	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(chk)))
		*p = VIZ8;
	else
		*p = VIZ4;

	return go;
}

void okay(GtkButton *b, gpointer o)
{
	if (o != NULL)
		*((int *) o) = TRUE;

	gtk_main_quit();
}
