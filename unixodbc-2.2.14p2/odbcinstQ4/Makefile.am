libodbcinstQ4_la_LDFLAGS = \
		-no-undefined \
		-version-info 1:0:0 \
		-export-dynamic \
		-export-symbols @srcdir@/odbcinstQ4.exp \
		-module

if QT4
if QTWIZARD

lib_LTLIBRARIES = libodbcinstQ4.la 

INCLUDES = -I@top_srcdir@/include $(QT_CXXFLAGS) \
    -DTHEPREFIX=\"@prefix@\" -DSYSTEM_FILE_PATH=\"@sysconfdir@\" -DDEFLIB_PATH=\"@libdir@\" -DSHLIBEXT=\"@SHLIBEXT@\" $(LTDLINCL)

libodbcinstQ4_la_LIBADD = \
	$(QT_LDFLAGS) \
	$(QT_LIBS) \
    ../ini/libinilc.la \
	../DriverManager/libodbc.la \
	../odbcinst/libodbcinst.la 

libodbcinstQ4_la_DEPENDENCIES = \
    ../ini/libinilc.la \
	../DriverManager/libodbc.la \
	../odbcinst/libodbcinst.la 

CLEANFILES = \
	mCAbout.cpp \
	mCAdvanced.cpp \
	mCDataSourceNameList.cpp \
	mCDataSourceNamesFile.cpp \
	mCDataSourceNamesFileModel.cpp \
	mCDataSourceNames.cpp \
	mCDriverList.cpp \
	mCDriverPrompt.cpp \
	mCDSNWizardProperties.cpp \
	mCDSNWizardDriver.cpp \
	mCDSNWizardEntre.cpp \
	mCDSNWizardFini.cpp \
	mCDSNWizard.cpp \
	mCDSNWizardType.cpp \
	mCFileSelector.cpp \
	mCHelp.cpp \
	mCManageDataSourceNames.cpp \
	mCManageDrivers.cpp \
	mCODBCConfig.cpp \
	mCPooling.cpp \
	mCPropertiesDelegate.cpp \
	mCPropertiesDialog.cpp \
	mCMonitorHandleCounts.cpp \
	mCMonitor.cpp \
	mCMonitorProcesses.cpp \
	mCThreading.cpp \
	mCTracing.cpp

libodbcinstQ4_la_SOURCES = \
	CAbout.cpp \
	CAdvanced.cpp \
	CDataSourceNameList.cpp \
	CDataSourceNamesFile.cpp \
	CDataSourceNamesFileModel.cpp \
	CDataSourceNames.cpp \
	CDriverList.cpp \
	CDriverPrompt.cpp \
	CDSNWizardProperties.cpp \
	CDSNWizardDriver.cpp \
	CDSNWizardEntre.cpp \
	CDSNWizardFini.cpp \
	CDSNWizard.cpp \
	CDSNWizardType.cpp \
	CFileSelector.cpp \
	CHelp.cpp \
	CManageDataSourceNames.cpp \
	CManageDrivers.cpp \
	CODBCConfig.cpp \
	CODBCInst.cpp \
	CPage.cpp \
	CPooling.cpp \
	CPropertiesDelegate.cpp \
	CPropertiesDialog.cpp \
	CPropertiesModel.cpp \
	CMonitorHandleCounts.cpp \
	CMonitor.cpp \
	CMonitorProcesses.cpp \
	CThreading.cpp \
	CTracing.cpp \
	SQLManageDataSources.cpp \
	mCAbout.cpp \
	mCAdvanced.cpp \
	mCDataSourceNameList.cpp \
	mCDataSourceNamesFile.cpp \
	mCDataSourceNamesFileModel.cpp \
	mCDataSourceNames.cpp \
	mCDriverList.cpp \
	mCDriverPrompt.cpp \
	mCDSNWizardProperties.cpp \
	mCDSNWizardDriver.cpp \
	mCDSNWizardEntre.cpp \
	mCDSNWizardFini.cpp \
	mCDSNWizard.cpp \
	mCDSNWizardType.cpp \
	mCFileSelector.cpp \
	mCHelp.cpp \
	mCManageDataSourceNames.cpp \
	mCManageDrivers.cpp \
	mCODBCConfig.cpp \
	mCPooling.cpp \
	mCPropertiesDelegate.cpp \
	mCPropertiesDialog.cpp \
	mCMonitorHandleCounts.cpp \
	mCMonitor.cpp \
	mCMonitorProcesses.cpp \
	mCThreading.cpp \
	mCTracing.cpp

mCAbout.cpp: @srcdir@/CAbout.h
	@MOC@ @srcdir@/CAbout.h -o mCAbout.cpp

mCAdvanced.cpp: @srcdir@/CAdvanced.h
	@MOC@ @srcdir@/CAdvanced.h -o mCAdvanced.cpp

mCDataSourceNameList.cpp: @srcdir@/CDataSourceNameList.h
	@MOC@ @srcdir@/CDataSourceNameList.h -o mCDataSourceNameList.cpp

mCDataSourceNamesFile.cpp: @srcdir@/CDataSourceNamesFile.h
	@MOC@ @srcdir@/CDataSourceNamesFile.h -o mCDataSourceNamesFile.cpp

mCDataSourceNamesFileModel.cpp: @srcdir@/CDataSourceNamesFileModel.h
	@MOC@ @srcdir@/CDataSourceNamesFileModel.h -o mCDataSourceNamesFileModel.cpp

mCDataSourceNames.cpp: @srcdir@/CDataSourceNames.h
	@MOC@ @srcdir@/CDataSourceNames.h -o mCDataSourceNames.cpp

mCDriverList.cpp: @srcdir@/CDriverList.h
	@MOC@ @srcdir@/CDriverList.h -o mCDriverList.cpp

mCDriverPrompt.cpp: @srcdir@/CDriverPrompt.h
	@MOC@ @srcdir@/CDriverPrompt.h -o mCDriverPrompt.cpp

mCDSNWizardProperties.cpp: @srcdir@/CDSNWizardProperties.h
	@MOC@ @srcdir@/CDSNWizardProperties.h -o mCDSNWizardProperties.cpp

mCDSNWizardDriver.cpp: @srcdir@/CDSNWizardDriver.h
	@MOC@ @srcdir@/CDSNWizardDriver.h -o mCDSNWizardDriver.cpp

mCDSNWizardEntre.cpp: @srcdir@/CDSNWizardEntre.h
	@MOC@ @srcdir@/CDSNWizardEntre.h -o mCDSNWizardEntre.cpp

mCDSNWizardFini.cpp: @srcdir@/CDSNWizardFini.h
	@MOC@ @srcdir@/CDSNWizardFini.h -o mCDSNWizardFini.cpp

mCDSNWizard.cpp: @srcdir@/CDSNWizard.h
	@MOC@ @srcdir@/CDSNWizard.h -o mCDSNWizard.cpp

mCDSNWizardType.cpp: @srcdir@/CDSNWizardType.h
	@MOC@ @srcdir@/CDSNWizardType.h -o mCDSNWizardType.cpp

mCFileSelector.cpp: @srcdir@/CFileSelector.h
	@MOC@ @srcdir@/CFileSelector.h -o mCFileSelector.cpp

mCHelp.cpp: @srcdir@/CHelp.h
	@MOC@ @srcdir@/CHelp.h -o mCHelp.cpp

mCManageDataSourceNames.cpp: @srcdir@/CManageDataSourceNames.h
	@MOC@ @srcdir@/CManageDataSourceNames.h -o mCManageDataSourceNames.cpp

mCManageDrivers.cpp: @srcdir@/CManageDrivers.h
	@MOC@ @srcdir@/CManageDrivers.h -o mCManageDrivers.cpp

mCODBCConfig.cpp: @srcdir@/CODBCConfig.h
	@MOC@ @srcdir@/CODBCConfig.h -o mCODBCConfig.cpp

mCPooling.cpp: @srcdir@/CPooling.h
	@MOC@ @srcdir@/CPooling.h -o mCPooling.cpp

mCPropertiesDelegate.cpp: @srcdir@/CPropertiesDelegate.h
	@MOC@ @srcdir@/CPropertiesDelegate.h -o mCPropertiesDelegate.cpp

mCPropertiesDialog.cpp: @srcdir@/CPropertiesDialog.h
	@MOC@ @srcdir@/CPropertiesDialog.h -o mCPropertiesDialog.cpp

mCMonitor.cpp: @srcdir@/CMonitor.h
	@MOC@ @srcdir@/CMonitor.h -o mCMonitor.cpp

mCMonitorHandleCounts.cpp: @srcdir@/CMonitorHandleCounts.h
	@MOC@ @srcdir@/CMonitorHandleCounts.h -o mCMonitorHandleCounts.cpp

mCMonitorProcesses.cpp: @srcdir@/CMonitorProcesses.h
	@MOC@ @srcdir@/CMonitorProcesses.h -o mCMonitorProcesses.cpp

mCThreading.cpp: @srcdir@/CThreading.h
	@MOC@ @srcdir@/CThreading.h -o mCThreading.cpp

mCTracing.cpp: @srcdir@/CTracing.h
	@MOC@ @srcdir@/CTracing.h -o mCTracing.cpp

endif
endif

EXTRA_DIST = \
	CAbout.h \
	CAdvanced.h \
	CDataSourceNameList.h \
	CDataSourceNamesFile.h \
	CDataSourceNamesFileModel.h \
	CDataSourceNames.h \
	CDriverList.h \
	CDriverPrompt.h \
	CDSNWizardProperties.h \
	CDSNWizardDriver.h \
	CDSNWizardEntre.h \
	CDSNWizardFini.h \
	CDSNWizard.h \
	CDSNWizardType.h \
	CFileSelector.h \
	CHelp.h \
	CManageDataSourceNames.h \
	CManageDrivers.h \
	CMonitor.h \
	CMonitorHandleCounts.h \
	CMonitorProcesses.h \
	CODBCConfig.h \
	CODBCInst.h \
	CPage.h \
	CPooling.h \
	CPropertiesDelegate.h \
	CPropertiesDialog.h \
	CPropertiesModel.h \
	CThreading.h \
	CTracing.h \
	About48.xpm \
	Advanced48.xpm \
	Canada.xpm \
	DataSourceName48.xpm \
	DataSourceNameFile48.xpm \
	DataSourceNameSystem48.xpm \
	DataSourceNameUser48.xpm \
	Driver128.xpm \
	Driver48.xpm \
	GreatBritain.xpm \
	Info16.xpm \
	InfoArrow16.xpm \
	NoFlag.xpm \
	ODBC.xpm \
	Pooling48.xpm \
	Set.xpm \
	Stats48.xpm \
	Threading48.xpm \
	Tracing48.xpm \
	odbcinstQ4.exp \
	ODBCManageDataSources64.xpm \
	WizardCreateDataSourceName.xpm


