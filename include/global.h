#include <glib.h>
#include <gtk/gtk.h>

typedef struct device {
    gchar *mac;
    char probes[80];
    gchar ven[9];
    gchar *time;
    int ap;
    GtkTreeIter iter;
}device;

struct node {
    device data;
    struct node *next;
};

GtkWidget *g_interface_input;
struct node* g_head;

device *add_dev(gchar *input_mac);
