#############################################################
#
# ddrescue
#
#############################################################

DDRESCUE_VERSION = 1.18.1
DDRESCUE_SITE = http://www.readynas.com/download/ddrescue/
DDRESCUE_LICENSE = GPLv3+
DDRESCUE_LICENSE_FILES = COPYING
DDRESCUE_CONF_OPT = CXX=$(TARGET_CROSS)c++

$(eval $(autotools-package))
