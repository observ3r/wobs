#include <gdk/gdk.h>
#include <glib.h>
#include <gtk/gtk.h>

typedef struct gui_list {
    GtkListStore *model;
    GtkTreeIter iter;
    GtkWidget *tree_view;
    GtkCellRenderer *cell;
    GtkTreeViewColumn *column;
    GtkWidget *scrolled_window;
}gui_list;

gui_list devices;
gui_list info;

void lstn();
void del_dev(gchar *mac);

static void info_popup(GtkTreeView        *view,
                        GtkTreePath        *ipath,
                        GtkTreeViewColumn  *col,
                        gpointer            user_data);

gui_list create_list(char *title,gui_list liststruct);

static void update_settings(GtkWidget *widget,
                 gpointer data);

