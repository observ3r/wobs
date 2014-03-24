#include "wlan.h"
#include "global.h"

#include <string.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

void *wlan_snif(void *unused) {
	int sok;
	int sockopt;
	u_char buf[BUFSIZE];
	struct ifreq ifopts;

	if ((sok = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) == -1) {
		perror("sniffer: socket");
		exit(-1);
	}
	const char *if_name=gtk_entry_get_text((GtkEntry*)g_interface_input);	
	char iw_command[50];
	char wob_if[] = "wob0";
	
	/* Not very robust.. the new interface name */
	if (sizeof(if_name)>10) {
		sprintf(iw_command,"iw dev wlan0 interface add wob0 type monitor");
	} else {
		sprintf(iw_command,"iw dev %s interface add wob0 type monitor",if_name);
	}
	
	system(iw_command);
	system("ifconfig wob0 up");
	
	strncpy(ifopts.ifr_name,wob_if,4);
	ioctl(sok, SIOCGIFFLAGS, &ifopts);
	ifopts.ifr_flags |= IFF_PROMISC;
	ioctl(sok, SIOCSIFFLAGS, &ifopts);
	
	setsockopt(sok,SOL_SOCKET,SO_BINDTODEVICE,wob_if,4);
	
	while(1){
		recvfrom(sok,buf,BUFSIZE,0,NULL,NULL);
		wlan_proc(buf);
	}
	return;
}

void wlan_proc(u_char *buf) {	
	/* TODO: Make time function */

	/************ Note ********************
	 * Was getting strange packets, could *
	 * not interpret them correctly. This *
	 * way of parsing (tree parsing?)     *
	 * probably isn't the best... Email me*
	 * with suggestions or ideas. Thanks! *
	 *************************************/
	if (buf[2]>50) {
		//printf("Strange! %02x\n",buf[2]);
		//g_printf("mac:%s\n",get_src_mac(buf));
		
		return;
	}
	//printf("Good! %02x\n",buf[2]);
	/**************************************
	 * Get the input mac, then send it to *
	 * the add_dev function so it can add *
	 * it and perform other processing.   *
	 * add_dev returns a pointer to the   *
	 * device struct that we can fill.    *
	 **************************************/

	gchar *input_mac = get_src_mac(buf);
	device *cdev = add_dev(input_mac);

	/* Now fill the device struct with    *
	 * more information from the packet   */

	/* Time */
	time_t btime;
	struct tm *time_s;
	time(&btime);
	time_s=localtime(&btime);

	cdev->time=asctime(time_s);
	cdev->time[24]=0x00;

	/* Probes (will be specific to packet types) */
	//g_scanf(cdev->probes,"none");
	cdev->probes[0]=0x00;

	return;
}

gchar *get_src_mac(u_char buf[]) {
	uint s = (uint) buf[2]+10;
	gchar *src = g_strdup_printf("%02X:%02X:%02X:%02X:%02X:%02X",
		buf[s],buf[s+1],buf[s+2],buf[s+3],buf[s+4],buf[s+5]);
	return src;
}

char get_type(char buf) {
	return;
}
