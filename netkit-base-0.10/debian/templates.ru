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
Description-ru: ��������� ������� chargen, echo, daytime/udp, time/udp?
 ���������� ������� `chargen', `echo', `daytime/udp' and/or `time/udp' �����
 �������������� ���������������� ��� ���� ���� "����� � ������������", � ������
 ������������� ��������� ��� �������. ����������� ����� ������ � CERT advisory
 CA-1996.01 (http://www.cert.org/advisories/CA-1996.01.html)
 .
 ����� ��������� ��� ���� /etc/inetd.conf � ��������� ��� �������������� �������
 (�������� UDP �������).
