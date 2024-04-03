Template: netkit-inetd/inetd-dos-services
Type: boolean
Default: true
Description: Disable chargen, echo, daytime/udp, time/udp services?
 The `chargen', `echo', `daytime/udp' and/or `time/udp' internal services
 can be used for denial-of-service attacks and should therefore be
 disabled. For further information please check the CERT advisory CA-1996.01
 (http://www.cert.org/advisories/CA-1996.01.html)
 .
 You should also check your /etc/inetd.conf and disable all unused services
 (especially UDP services).
Description-sv: Avaktivera tj�nsterna chargen, echo, daytime/udp, time/udp?
 De interna tj�nsterna `chargen', `echo', `daytime/udp' och/eller `time/udp'
 kan anv�ndas f�r tj�nstev�gran attacker och b�r d�rf�r avaktiveras. F�r mer
 information, se CERT r�dgivningen CA-1996.01
 (http://www.cert.org/advisories/CA-1996.01.html)
 .
 Du b�r ocks� kontrollera din /etc/inetd.conf och avaktivera alla oanv�nda
 tj�nster (speciellt UDP tj�nster.)
