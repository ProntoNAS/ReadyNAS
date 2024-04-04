Ship the rescan-scsi-bus.sh script
--- a/scripts/Makefile.am
+++ b/scripts/Makefile.am
@@ -1,2 +1,3 @@
 bin_SCRIPTS = scsi_logging_level scsi_mandat scsi_readcap scsi_ready \
-	      scsi_satl scsi_start scsi_stop scsi_temperature
+	      scsi_satl scsi_start scsi_stop scsi_temperature \
+	      rescan-scsi-bus.sh
