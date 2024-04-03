# Some distributions have both gdbm and ndbm
# Prefer gdbm to avoid the broken ndbm in some distributions
# (no null key support)
# Jonathan Stowe <gellyfish@gellyfish.com>
$self->{LIBS} = ['-lgdbm_compat'];
