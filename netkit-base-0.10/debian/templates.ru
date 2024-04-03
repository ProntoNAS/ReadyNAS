Template: netkit-inetd/inetd-dos-services
Type: boolean
Default: true
Description: Disable chargen, echo, daytime/udp, time/udp services?
 The `chargen', `echo', `daytime/udp' and/or `time/udp' internal services
 can be used for denial-of-service attacks and should therefore be
 disabled. For further information please check the CERT advisory CA-1996.01
 (http://www.cert.org/advisories/CA-1996.01.html)
 .
 You should also check your /etc/inetd.conf and disable all unused
 services (especially UDP services).
Description-ru: Запретить сервисы chargen, echo, daytime/udp, time/udp?
 Внутренние сервисы `chargen', `echo', `daytime/udp' and/or `time/udp' могут
 использоваться злоумышленниками для атак типа "отказ в обслуживании", и потому
 рекомендуется запретить эти сервисы. Подробности можно узнать в CERT advisory
 CA-1996.01 (http://www.cert.org/advisories/CA-1996.01.html)
 .
 Также проверьте ваш файл /etc/inetd.conf и запретите все неиспользуемые сервисы
 (особенно UDP сервисы).
