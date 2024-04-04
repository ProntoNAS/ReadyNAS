#!/usr/bin/awk -f

# dpkg-diff.awk - compare and print differencies in the installed packages of two dpkg status files
# by xmb - localhack
# xmb@skilled.com

# status in the array:
# 1 = installed in the reference file, but not in the other one
# 2 = both matching
# 3 = newly intsalled on the other status file
# decided to use additionally a plain index for faster processing of the END block

# Changes by Justin Maggard (justin.maggard@netgear.com)
# 2010/05/27:
# * Don't print out anything except control data present in the reference file and not in the other file
# * Remove some unused functionality

BEGIN {
	if (! status) status = "/var/lib/dpkg/status"
	
	if (ARGC <= 1 && ! stdin) {
		exit 1
	}
	
	if (stdin)
		ARGC = 1
	else
		ARGC = 2

	while (getline < status) {
		if ($1 == "Package:") {
			tmp = $2

			getline < status
			if ($1 != "Status:")
				getline < status
			
			if (! /install ok/)
				continue
			
			Package[tmp] = 1
			idx = idx tmp " "
		}
	} close(status)
}

$1 == "Package:" {
	tmp = $2

	getline
	if ($1 != "Status:")
		getline
	
	if (/install ok/) {
		if (Package[tmp]) {
			Package[tmp] = 2
		} else {
			Package[tmp] = 3
			idx = idx tmp " "
		}
	}

	if (! valid_file)
		valid_file = 1
}

END {
	if (! valid_file) {
		exit 1
	}
	
	# 'iin' instead of 'in' cause 'in' is an awk keyword
	split(idx, Idx)
	while (Idx[++i]) {
		if (Package[Idx[i]] == 1)
			Res[1, ++iin] = Idx[i]
	} iin = 0

	while (Res[1, ++iin])
		;

	if (iin > 1) {
		while (getline < status) {
			if ($1 == "Package:") {
				tmp = $2
				printit = 0

				iin = 0
				while (Res[1, ++iin]) {
					if (tmp == Res[1, iin]) {
						print $0
						printit = 1
					}
				}
			}
			else {
				if (printit)
					print $0
			}
		} close (status)
	}
}
