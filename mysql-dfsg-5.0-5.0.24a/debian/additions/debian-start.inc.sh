#!/bin/bash
#
# This file is included by /etc/mysql/debian-start
#

## Check all unclosed tables.
# - Requires the server to be up.
# - Is supposed to run silently in background. 
function mycheck() {
  set -e
  set -u

  # But do it in the background to not stall the boot process.
  logger -p daemon.info -i -t$0 "Checking for crashed MySQL tables."

  # Checking for $? is unreliable so the size of the output is checked.
  # Some table handlers like HEAP do not support CHECK TABLE.
  tempfile=`tempfile`
  LC_ALL=C $MYCHECK $MYCHECK_PARAMS \
    2>&1 \
    | perl -e '$_=join("", <>); s/^[^\n]+\n(error|note)\s+: The (handler|storage engine) for the table doesn.t support check\n//smg;print;' \
    > $tempfile
  if [ -s $tempfile ]; then
    (
      /bin/echo -e "\n" \
        "Improperly closed tables are also reported if clients are accessing\n" \
 	"the tables *now*. A list of current connections is below.\n";
       $MYADMIN processlist status
    ) >> $tempfile
    # Check for presence as a dependency on mailx would require an MTA.
    if [ -x /usr/bin/mailx ]; then mailx -e -s"$MYCHECK_SUBJECT" $MYCHECK_RCPT < $tempfile; fi
    (echo "$MYCHECK_SUBJECT"; cat $tempfile) | logger -p daemon.warn -i -t$0
  fi
  rm $tempfile
}

## Check for tables needing an upgrade.
# - Requires the server to be up.
# - Is supposed to run silently in background. 
function myupgrade() {
  set -e
  set -u

  logger -p daemon.info -i -t$0 "Upgrading MySQL tables if necessary."

  LC_ALL=C $MYUPGRADE \
    2>&1 \
    | fgrep -v 'Duplicate column name' \
    | logger -p daemon.warn -i -t$0
}
