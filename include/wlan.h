#include <gdk/gdk.h>
#include <gtk/gtk.h>
#include <glib.h>

#define BUFSIZE 1024

void wlan_proc(u_char *buf);
void *wlan_snif(void* unused);
gchar *get_src_mac(u_char buf[]);
char get_type(char buf);

pthread_t listener_thread;
