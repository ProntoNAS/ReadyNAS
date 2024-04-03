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
Description-sv: Avaktivera tjänsterna chargen, echo, daytime/udp, time/udp?
 De interna tjänsterna `chargen', `echo', `daytime/udp' och/eller `time/udp'
 kan användas för tjänstevägran attacker och bör därför avaktiveras. För mer
 information, se CERT rådgivningen CA-1996.01
 (http://www.cert.org/advisories/CA-1996.01.html)
 .
 Du bör också kontrollera din /etc/inetd.conf och avaktivera alla oanvända
 tjänster (speciellt UDP tjänster.)
