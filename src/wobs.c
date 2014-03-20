#include "wobs.h"
#include "wlan.h"
#include "global.h"

#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

int listener_state = 0;

void lstn() {
	if (listener_state==0){
		g_print("Starting listener...\n\n");

		/* Spawn thread(s) */
		if (pthread_create(&listener_thread, NULL, &wlan_snif, NULL)!=0) {
			g_print("ERROR: Failed to start 802.11 listener!\n");
			return;
		}
		
		/* Add additional sniffers for new protocols here */

		listener_state=1;
	}else{
		g_print("Stopping listener...\n\n");
		/* Send kill to thread */
		int i;
		if (i=pthread_cancel(listener_thread)!=0) {
			g_print("ERROR %d: Failed to stop listener! (maybe already stopped)\n",i);
		}
		listener_state=0;

		/************************************************
		 * Clean up WLAN interface... Policy is that   *
		 * this function should not have to do anything *
		 * with specific sniffers, so this is a bit of  *
		 * an exception.                                *
		 ************************************************/
		system("iw dev wob0 del");
	}
}

void del_dev(gchar *mac) {
	struct node* current = g_head;
	struct node* previous;
	while (current != NULL) {
		if (mac==current->data.mac) {
			gtk_list_store_remove(GTK_LIST_STORE(devices.model),
							&current->data.iter);
			previous->next = current->next;
			free(current);
			break;
		}
		previous = current;
		current = current->next;
	}
}

static void info_popup(GtkTreeView *view,
			GtkTreePath *ipath,
			GtkTreeViewColumn *col,
			gpointer user_data) {
	GtkTreePath *dpath;
	GtkTreeIter iter;
	GtkWidget *popup=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect(G_OBJECT(popup),"destroy",
		G_CALLBACK(gtk_widget_destroy),NULL);
	gtk_container_set_border_width(GTK_CONTAINER(popup),5);
	gtk_widget_set_size_request(GTK_WIDGET(popup),300,200);
	gtk_window_set_resizable(GTK_WINDOW(popup),FALSE);
	gtk_window_set_position(GTK_WINDOW(popup), GTK_WIN_POS_CENTER);

	GtkWidget *mp_cont=gtk_vbox_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(popup),mp_cont);

	/* Frame */
	GtkWidget *popup_frame = gtk_frame_new("");
	gtk_frame_set_shadow_type(GTK_FRAME(popup_frame),GTK_SHADOW_ETCHED_IN);
	gtk_box_pack_start(GTK_BOX(mp_cont),popup_frame,TRUE,TRUE,0);
	GtkWidget *p_cont=gtk_vbox_new(FALSE,9);
	gtk_container_add(GTK_CONTAINER(popup_frame),p_cont);

	/* Close Button */
	GtkWidget *close_button_cont=gtk_hbox_new(FALSE,0);
	gtk_box_pack_end(GTK_BOX(mp_cont),close_button_cont,FALSE,FALSE,3);
	GtkWidget *close_button=gtk_toggle_button_new_with_label("Close");
	gtk_widget_set_size_request(GTK_WIDGET(close_button),60,35);
	gtk_box_pack_start(GTK_BOX(close_button_cont),close_button,TRUE,FALSE,0);
	g_signal_connect_swapped (close_button, "clicked",
					G_CALLBACK (gtk_widget_destroy),popup);

	struct node* current = g_head;
	gtk_tree_model_get_iter((GtkTreeModel*)devices.model,&iter,ipath);
	while (current != NULL) {
		dpath = gtk_tree_model_get_path((GtkTreeModel*)devices.model,
			&current->data.iter);
		if (gtk_tree_path_compare(dpath,ipath)==0) {
			/* Title */
			gchar *mact=g_strdup_printf("Device %s\n",current->data.mac);
			gtk_window_set_title(GTK_WINDOW(popup),mact);

			/* MAC */
			GtkWidget *mac_box=gtk_hbox_new(FALSE,0);
			gtk_box_pack_start(GTK_BOX(p_cont),mac_box,FALSE,FALSE,0);
			GtkWidget *mac_label=gtk_label_new("MAC:");
			gtk_box_pack_start(GTK_BOX(mac_box),mac_label,FALSE,FALSE,2);
			gchar *mac_addr_label=g_strdup_printf("%s",current->data.mac);
			GtkWidget *mac_addr=gtk_label_new(mac_addr_label);
			gtk_box_pack_end(GTK_BOX(mac_box),mac_addr,FALSE,FALSE,2);

			/* Type */
			GtkWidget *type_box=gtk_hbox_new(FALSE,0);
			gtk_box_pack_start(GTK_BOX(p_cont),type_box,FALSE,FALSE,0);
			GtkWidget *type_label=gtk_label_new("Type:");
			gtk_box_pack_start(GTK_BOX(type_box),type_label,FALSE,FALSE,2);

			if (current->data.ap==1) {
				GtkWidget *type=gtk_label_new("Access Point");
				gtk_box_pack_end(GTK_BOX(type_box),type,FALSE,FALSE,2);

				/* SSID */
				GtkWidget *ssid_box=gtk_hbox_new(FALSE,0);
				gtk_box_pack_start(GTK_BOX(p_cont),ssid_box,FALSE,FALSE,0);
				GtkWidget *ssid_label=gtk_label_new("SSID:");
				gtk_box_pack_start(GTK_BOX(ssid_box),ssid_label,FALSE,FALSE,2);
				gchar *ssidt=g_strdup_printf("%s",current->data.probes);
				GtkWidget *ssid=gtk_label_new(ssidt);
				gtk_box_pack_end(GTK_BOX(ssid_box),ssid,FALSE,FALSE,2);
			} else {
				GtkWidget *type=gtk_label_new("Client");
				gtk_box_pack_end(GTK_BOX(type_box),type,FALSE,FALSE,2);

				/* Probes */
				GtkWidget *prob_box=gtk_hbox_new(FALSE,0);
				gtk_box_pack_start(GTK_BOX(p_cont),prob_box,FALSE,FALSE,0);
				GtkWidget *probes_label=gtk_label_new("Probes:");
				gtk_box_pack_start(GTK_BOX(prob_box),probes_label,FALSE,FALSE,2);
				gchar *probest=g_strdup_printf("%s",current->data.probes);
				GtkWidget *probes=gtk_label_new(probest);
				gtk_box_pack_end(GTK_BOX(prob_box),probes,FALSE,FALSE,2);
			}

			/* Vendor */
			GtkWidget *ven_box=gtk_hbox_new(FALSE,0);
			gtk_box_pack_start(GTK_BOX(p_cont),ven_box,FALSE,FALSE,0);
			GtkWidget *ven_label=gtk_label_new("Vendor:");
			gtk_box_pack_start(GTK_BOX(ven_box),ven_label,FALSE,FALSE,2);
			gchar *vent=g_strdup_printf("%s",current->data.ven);
			GtkWidget *ven=gtk_label_new(vent);
			gtk_box_pack_end(GTK_BOX(ven_box),ven,FALSE,FALSE,2);

			/* Time */
			GtkWidget *time_box=gtk_hbox_new(FALSE,0);
			gtk_box_pack_start(GTK_BOX(p_cont),time_box,FALSE,FALSE,0);
			GtkWidget *time_label=gtk_label_new("Last Observed:");
			gtk_box_pack_start(GTK_BOX(time_box),time_label,FALSE,FALSE,2);
			gchar *timet=g_strdup_printf("%s",current->data.time);
			GtkWidget *time=gtk_label_new(timet);
			gtk_box_pack_end(GTK_BOX(time_box),time,FALSE,FALSE,2);

			gtk_widget_show_all(popup);
			break;
		}
		current = current->next;
	}
}

gui_list create_list(char *title,gui_list liststruct) {
	liststruct.scrolled_window = gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy(
		GTK_SCROLLED_WINDOW(liststruct.scrolled_window),
		GTK_POLICY_AUTOMATIC,
		GTK_POLICY_AUTOMATIC);

	liststruct.model=gtk_list_store_new(1,G_TYPE_STRING);
	liststruct.tree_view=gtk_tree_view_new();
	gtk_scrolled_window_add_with_viewport(
		GTK_SCROLLED_WINDOW(liststruct.scrolled_window),
		liststruct.tree_view);
	gtk_tree_view_set_model(GTK_TREE_VIEW(liststruct.tree_view),
		GTK_TREE_MODEL(liststruct.model));
	gtk_widget_show(liststruct.tree_view);

	liststruct.cell = gtk_cell_renderer_text_new ();
	liststruct.column = gtk_tree_view_column_new_with_attributes(title,
		liststruct.cell,"text", 0,NULL);

	gtk_tree_view_append_column(GTK_TREE_VIEW(liststruct.tree_view),
		GTK_TREE_VIEW_COLUMN(liststruct.column));
	return liststruct;
}
static void update_settings(GtkWidget *widget,
				 gpointer data) {
	printf("Updating\n\n");
	int was_on;
	if (listener_state!=0){
		lstn();
		was_on=1;
	}
	gtk_list_store_remove(GTK_LIST_STORE(devices.model),
		&g_head->data.iter);
	struct node* del;
	if (g_head->next) {
		struct node* current = g_head->next;
		while (current != NULL) {
			gtk_list_store_remove(GTK_LIST_STORE(devices.model),
				&current->data.iter);
			del = current;
			current = current->next;
			free(del);
		}
	}
	memset(&g_head,0,sizeof(&g_head));
	if (was_on){
		lstn();
	}
	printf("Done Updating\n\n");
}

int main(int argc,char *argv[]) {
	GtkWidget *window;
	GtkWidget *main_view;
	GtkWidget *listen_button;
	GtkWidget *update_button;
	GtkWidget *cont;
	GtkWidget *top_bar;
	GtkWidget *bot_bar;

	GtkWidget *menu_bar;
	GtkWidget *file_menu;
	GtkWidget *file;
	GtkWidget *quit;
	GtkWidget *help;
	GtkWidget *edit;
	GtkWidget *pref;
	GtkWidget *save;
	GtkWidget *open;
	GtkWidget *about;
	GtkWidget *edit_menu;
	GtkWidget *help_menu;

	GtkWidget *promi;
	GtkWidget *moni;
	GtkWidget *interface_label;
	GtkWidget *interface_box;
	GtkWidget *settings_p;

	GtkWidget *ptext;
	GtkWidget *settings_frame;
	GtkWidget *icon;


	gtk_init(&argc,&argv);
	PangoFontDescription *monospace;
	PangoFontDescription *title_font;
	monospace = pango_font_description_from_string("Monospace");
	title_font = pango_font_description_from_string("Sans");
	pango_font_description_set_size(title_font,15*PANGO_SCALE);

	/* Setup the window */
	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window),"WOBS - Wireless Observer");
	g_signal_connect(G_OBJECT(window),"destroy",
		G_CALLBACK(gtk_main_quit),NULL);
	gtk_container_set_border_width(GTK_CONTAINER(window),10);
	gtk_widget_set_size_request(GTK_WIDGET(window),500,550);
	cont=gtk_vbox_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(window),cont);

	/* Menu Bar */
	menu_bar=gtk_menu_bar_new();
	file_menu=gtk_menu_new();
	edit_menu=gtk_menu_new();
	help_menu=gtk_menu_new();
	file=gtk_menu_item_new_with_label("File");
	help=gtk_menu_item_new_with_label("Help");
	edit=gtk_menu_item_new_with_label("Edit");
	pref=gtk_menu_item_new_with_label("Preferences");
	save=gtk_menu_item_new_with_label("Save List");
	open=gtk_menu_item_new_with_label("Open List");
	quit=gtk_menu_item_new_with_label("Quit");
	about=gtk_menu_item_new_with_label("About");

	gtk_menu_item_set_submenu(GTK_MENU_ITEM(file),file_menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(file_menu),open);
	gtk_menu_shell_append(GTK_MENU_SHELL(file_menu),save);
	gtk_menu_shell_append(GTK_MENU_SHELL(file_menu),quit);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar),file);

	gtk_menu_item_set_submenu(GTK_MENU_ITEM(edit),edit_menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu),pref);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar),edit);

	gtk_menu_item_set_submenu(GTK_MENU_ITEM(help),help_menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(help_menu),about);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar),help);

	gtk_box_pack_start(GTK_BOX(cont),menu_bar,FALSE,FALSE,0);

	g_signal_connect(G_OBJECT(quit),"activate",
		G_CALLBACK(gtk_main_quit),NULL);

	/* The top bar */
	top_bar=gtk_hbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(cont),top_bar,FALSE,FALSE,4);
	gtk_widget_show(top_bar);

		/* Add the Listen button */
		listen_button=gtk_toggle_button_new_with_label("Listen");
		gtk_widget_set_size_request(GTK_WIDGET(listen_button),100,40);
		gtk_box_pack_start(GTK_BOX(top_bar),listen_button,FALSE,FALSE,0);
		g_signal_connect(G_OBJECT(listen_button),"toggled",
			G_CALLBACK(lstn),NULL);
		gtk_widget_show(listen_button);

		/* Add the Icon */
		icon = gtk_image_new_from_file("wobs.png");
		gtk_box_pack_end(GTK_BOX(top_bar),icon,FALSE,FALSE,0);

	/* The Bottom Bar */
	bot_bar=gtk_hbox_new(FALSE,5);
	gtk_box_pack_start(GTK_BOX(cont),bot_bar,TRUE,TRUE,3);

		/* Add settings frame */
		settings_frame = gtk_frame_new("Settings");
		gtk_frame_set_shadow_type(GTK_FRAME(settings_frame),GTK_SHADOW_ETCHED_IN);
		gtk_box_pack_start(GTK_BOX(bot_bar),settings_frame,FALSE,FALSE,0);

		settings_p=gtk_vbox_new(FALSE,3);
		gtk_container_add(GTK_CONTAINER(settings_frame),settings_p);

			/* The Interface box */
			interface_box=gtk_vbox_new(FALSE,0);
			gtk_box_pack_start(GTK_BOX(settings_p),interface_box,FALSE,FALSE,0);

			interface_label=gtk_label_new("Interface: ");
			gtk_box_pack_start(GTK_BOX(interface_box),interface_label,FALSE,FALSE,0);

			g_interface_input=gtk_entry_new();
			gtk_entry_set_text((GtkEntry*)g_interface_input,"wlan0");
			gtk_box_pack_start(GTK_BOX(interface_box),g_interface_input,FALSE,FALSE,0);

			/* Modes */
			#if 0
			promi=gtk_check_button_new_with_label("Promiscuous mode");
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(promi), TRUE);
			gtk_box_pack_start(GTK_BOX(settings_p),promi,FALSE,FALSE,0);

			moni=gtk_check_button_new_with_label("Monitor mode");
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(moni), TRUE);
			gtk_box_pack_start(GTK_BOX(settings_p),moni,FALSE,FALSE,0);
			#endif

			/* Update Button */
			ptext=gtk_label_new(
				"NOTE: Clicking 'Update'\nwill clear the Devices list.");
			gtk_box_pack_start(GTK_BOX(settings_p),ptext,FALSE,FALSE,20);
			gtk_widget_show(ptext);

			update_button=gtk_button_new_with_label("Update");
			gtk_widget_set_size_request(GTK_WIDGET(update_button),100,30);
			gtk_box_pack_start(GTK_BOX(settings_p),update_button,FALSE,FALSE,0);
			g_signal_connect(G_OBJECT(update_button),"clicked",
				G_CALLBACK(update_settings),NULL);
			gtk_widget_show(update_button);

		/* Setup the Main View */
		main_view=gtk_hpaned_new ();
		gtk_box_pack_start(GTK_BOX(bot_bar),main_view,TRUE,TRUE,0);
		gtk_widget_show(main_view);

			/* Add the info pane */
			#if 0
			info=create_list("Info",info);
			gtk_paned_add1(GTK_PANED(main_view),info.scrolled_window);
			gtk_widget_show(info.scrolled_window);
			#endif
			
			/* Add the list of devices */
			devices=create_list("Devices",devices);
			gtk_widget_modify_font(devices.tree_view, monospace);
			gtk_paned_add2(GTK_PANED(main_view),devices.scrolled_window);
			gtk_widget_show(devices.scrolled_window);
			g_signal_connect(devices.tree_view,"row_activated",
				(GCallback)info_popup,NULL);

	/* Show */
	gtk_widget_show(cont);
	gtk_widget_show_all(window);

	gtk_main();
	return 0;
}
