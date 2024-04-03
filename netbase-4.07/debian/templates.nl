Template: netbase/upgrade-note/radius-ports-pre-3.05
Description: Important hint for users of radius software
 The official port numbers of the radius service have been changed
 from 1645 and 1646 to 1812 and 1813. If you use the radius service
 please make sure that the client and server software both use the
 same port numbers.
Description-nl: Belangrijke opmerking voor gebruikers van radius programma's
 De officiële poortnummers van de radius service zijn veranderd van
 1645 en 1646 naar 1812 en 1813.  Als u de radius service gebruikt,
 overtuig uzelf er van dat de client en server beide de zelfde
 poortnummers gebruiken.

Template: netbase/upgrade-note/portmap-restart-pre-3.11-2
Description: The old portmapper is still running.
 The old portmapper is still running. This will cause problems, mainly that
 "/etc/init.d/portmap stop" won't actually do anything. To fix this, I'm
 going to try to forcibly stop portmap, and then restart it.
Description-nl: De oude portmapper draait nog steeds.
 De oude portmapper draait nog.  Dit zal problemen veroorzaken,
 voornamelijk dat "/etc/init.d/portmap stop" niets doet.  Om dit goed
 te krijgen zal ik de oude portmap geforceerd stoppen, en het opnieuw
 starten.

Template: netbase/upgrade-note/init.d-split-pre-3.16-1
Description: /etc/init.d/netbase has been split.
 /etc/init.d/netbase is no longer required or used.
 .
 /etc/init.d/portmap (provided by the portmap package) now handles
 stopping and starting the portmapper, /etc/init.d/inetd (provided by
 the netkit-inetd package) handles stopping and starting inetd, and
 /etc/init.d/networking handles spoof protection.
Description-nl: /etc/init.d/netbase is opgesplitst.
 /etc/init.d/netbase is niet langer noodzakelijk of gebruikt.
 .
 /etc/init.d/portmap (voorzien in het portmap pakket) behandelt nu het
 starten en stoppen van de portmapper, /etc/init.d/inetd (uit het
 netkit-inetd pakket) behandelt het starten en stoppen van inetd, en
 /etc/init.d/networking doet de spoof-beveiliging.

Template: netbase/upgrade-note/etc-network-interfaces-pre-3.17-1
Description: /etc/init.d/network superceded by /etc/network/interfaces
 /etc/init.d/network is no longer directly supported. You may, of course,
 continue using it to setup your networking, however new Debian installs
 will use the ifup/ifdown commands to configure network interfaces based
 on the settings in /etc/network/interfaces.
 .
 If you do convert to using /etc/network/interfaces in place of
 /etc/init.d/network you will probably want to remove /etc/init.d/network
 and the /etc/rcS.d/S40network symlink. These will not be touched by
 netbase or other Debian packages in future.
 .
 Note that the old default /etc/init.d/network used to add a route for the
 loopback interface. This is no longer necessary for 2.2.x series kernels,
 and will result in a (non-fatal) SIOCADDRT error message at bootup.
Description-nl: /etc/init.d/network gaat over in /etc/network/interfaces
 /etc/init.d/network wordt niet langer direct ondersteund.  U kunt het
 natuurlijk blijven gebruiken om uw netwerk in te stellen, maar nieuwe
 Debian installaties zullen de commando's ifup/ifdown gebruiken om
 netwerk interfaces te configureren gebaseerd op de instellingen uit
 /etc/network/interfaces.
 .
 Als u /etc/network/interfaces gaat gebruiken in plaats van
 /etc/init.d/network, wilt u waarschijnlijk /etc/init.d/network en
 de koppeling /etc/rcS.d/S40network verwijderen.  Ze zullen niet meer
 aangeraakt worden door netbase of een ander Debian pakket in de
 toekomst.
 .
 Merk op dat het oude /etc/init.d/network een route toevoegde voor de
 lokale interface.  Dit is voor 2.2.x kernels niet langer
 noodzakelijk, en resulteert in een (niet-fatale) foutmelding over
 SIOCADDRT tijdens het opstarten.

Template: netbase/ipv6-hosts
Description: Would you like IPv6 addresses added to /etc/hosts?
 Sooner or later, Debian will include out-of-the box support
 for IPv6 (see http://www.ipv6.org/). As such, you might like
 to start playing with this, and seeing what things break as
 we try to add support for IPv6.
Description-nl: Wilt u IPv6 adressen toevoegen aan /etc/hosts?
 Vroeg of laat zal Debian standaard IPv6 ondersteuning leveren (zie
 http://www.ipv6.org/).  Zodoende wilt u er misschien alvast mee
 spelen, om te zien wat er misgaat als wij ondersteuning voor IPv6
 toevoegen.

Template: netbase/spoofprot
Description: Spoof protection for pre-2.2 kernels
 If you are running a pre-2.2 series kernel, IP spoof 
 protection cannot be enabled without special configuration,
 found in /etc/network/spoof-protect and provided by answering
 the following questions. 
 .
 For 2.2.x and later kernels, this information will be determined
 automatically at boot time, so you don't need to enter anything here
 unless you also use pre-2.2 kernels.
Description-nl: Spoof-bescherming voor pre-2.2 kernels
 Als u een pre-2.2 kernel draait, kan IP spoof-bescherming niet
 aangezet worden zonder speciale configuratie uit
 /etc/network/spoof-protect en de volgende vragen.
 .
 Voor 2.2.x en latere kernels, kan deze informatie automatisch
 opgevraagd worden bij het opstarten, zodat u geen andere informatie
 hoeft op te geven tenzij u ook pre-2.2 kernels draait.

Template: netbase/spoofprot/pre-2.2-ip
Description: What IP addresses (or address ranges) should be considered local?
 IP addresses and ranges should be listed in any order, and separated by 
 spaces. Addresses should be specified as a dotted quad, while ranges should
 be specified in CIDR-style. So the class C network 192.168.42.0-192.168.42.255
 would be specified as 192.168.42.0/24.
Description-nl: Welke IP adressen (of adresgebieden) zijn lokaal?
 IP adressen en gebieden moeten opgegeven worden, in willekeurige
 volgorde, en gescheiden door spaties.  Adressen moeten opgegeven
 worden als vier getallen gescheiden door punten, terwijl gebieden
 opgegeven moeten worden in CIDR-stijl.  Dus het klasse-C netwerk
 192.168.42.0-192.168.42.255 wordt opgegeven als 192.168.42.0/24.

Template: netbase/spoofprot/pre-2.2-interfaces
Description: What remote interfaces does this host have?
Description-nl: Welke publieke interfaces heeft deze computer?
