#include "wobs.h"
#include "global.h"

#include <stdlib.h>

device *add_dev(gchar *input_mac) {
	/* Print time to test how long it takes */
	#if 0
	time_s=localtime(&btime);
	printf("Loop through llist... Start: %s",asctime(time_s));
	#endif

	device *dev;
	int known = 0;
	struct node *current = g_head;
	while (current != NULL) {
		if (strncmp(input_mac,current->data.mac,17)==0) {
			known = 1;
			dev = &current->data;
			break;
		}
		current = current->next;
	}

	/* print time to test how long it takes */
	#if 0
	time_s=localtime(&btime);
	printf(" End: %s Done.\n",asctime(time_s));
	#endif

	if (!known) {
		struct node* nDevice = malloc(sizeof(struct node));
		nDevice->data.mac = g_strdup_printf("%s",input_mac);
		dev = &nDevice->data;
		nDevice->next = g_head;
		g_head = nDevice;
		mac_trans(dev->mac,dev->ven);

		GError *gerror;
		GdkPixbuf *pixbuf;
		GtkTextIter iter;
	
		/* For inserting Image
		 * GtkTextIter txt_iter;
		 * GtkTextBuffer *txt;
		 * gtk_text_buffer_get_start_iter(txt,&txt_iter);
		 * pixbuf = gdk_pixbuf_new_from_file("icon.ico",&gerror);
		 * gtk_text_buffer_insert(txt,&txt_iter,
		 *			g_strdup_printf("MAC: %s",dev.mac),-1);
		 * gtk_text_buffer_insert_pixbuf(txt,&txt_iter,pixbuf);
		 */
	
		gchar *txt;
		txt = g_strdup_printf("%s:	 %s device",dev->mac,dev->ven);
		gtk_list_store_prepend(GTK_LIST_STORE(devices.model),&dev->iter);
		gtk_list_store_set(GTK_LIST_STORE(devices.model),
						&dev->iter,0,txt,-1);
	}
	return dev;
}
