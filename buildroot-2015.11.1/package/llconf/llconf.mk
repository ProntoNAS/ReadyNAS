#############################################################
#
# llconf
#
#############################################################

LLCONF_VERSION = 0.4.6
LLCONF_SITE = http://www.readynas.com/download/archive/llconf
LLCONF_INSTALL_STAGING = YES

$(eval $(autotools-package))
