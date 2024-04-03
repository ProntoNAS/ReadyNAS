#ifndef SELINUX_H
#define SELINUX_H

extern void setup_selinux_pty(const char *, const char *);
extern void setup_selinux_exec_context(const char *);

#endif /* SELINUX_H */
