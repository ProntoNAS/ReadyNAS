all: it

it:
	po4a -L iso-8859-1 shadow.conf

shadow.conf:
	echo -e "[po4a_paths] shadow-man.pot it:it.po\n" > $@
	for i in `ls -1 ../../*.[0-9]`; do \
	  page=`basename $$i`; \
	  if po4a-gettextize -f man -m ../../$$page -p /dev/null; then \
	    echo "[type:man] ../../$$page it:../../it/$$page add_it:addendum.it" >> $@; \
	  else \
	    echo "# ../../$$page" >> $@; \
	  fi; \
	done
