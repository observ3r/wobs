=======
wobs
====

Detects near-by devices such as cell phones, tablets, and laptops. Does this through 802.11, Bluetooth, cell phone protocols, etc..

WOBS: A wireless observer. The final release will passively detect devices through 802.11 (WiFi), Bluetooth, and cell phone traffic. It will also display information about each device; i.e. MAC address, last time heard from, probes (networks the device is searching for or its SSID), capabilities, signal strength, device name, etc... Also, if enough data from multiple points (multiple other, observing, devices) is collected, the device in question's location through trilateration. There could also be a notification system that would send a email out when a new device has been spotted.

So, basically, if someone brings a cell phone near your house (or wherever you have this setup) this system will alert you to its presence. Actually, not just cell phones, but tablets, laptops, etc...

I know projects like Kismet already do most of this. WOBS would be more of a GUI alternative and more aimed at passively detecting ANY device in the area.

======
COMPILE
======
Just run make:
  make

======
RUNNING
======
./wobs

Currently it will not set the "Type". The default is client, so you won't see any Access Points. This is because I changed the way it's parsing packets.. and have not finished it.

Another note, it doesn't know how to handle some packets; so for best results, be sure you're not connected to a network.

And another note, wobs will create an interface called "wob0". It will do this with whatever interface is entered in the "Interface" text box at the moment of clicking "Listen". Currently, if you run two instances of wobs at once, one of them will try to create another wob0 interface, so watch out.
>>>>>>> 0c22cfab609204d2ca4a3339678b71d5a982834f
