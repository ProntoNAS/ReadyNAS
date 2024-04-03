/*
 */

enum {
	T_READY, T_RUNNING, T_FINISHED
};

/* target nodes */
struct makenode {
	char *name;
	char *arg0;
	int num_deps;
	struct makelist *depend;
	int num_sels;
	struct makelist *select;
	int status;
	struct makenode *next;
	int interactive;
	int importance;
};

/* dependency and selection list nodes */
struct makelist {
	struct makenode *node;
	struct makelist *next;
};

extern int tree_entries;
extern struct makenode *tree_list;

extern void parse_makefile(const char *path);
extern void check_run_files(const char *action, const char *prev, const char *run);
extern struct makenode *pickup_task(void);
extern void finish_task(struct makenode *n);
extern void *xcalloc(size_t nmemb, size_t size);
extern void print_run_result(int *resvec, struct makenode **nodevec, const char *action);

#define alignof(type)		((sizeof(type)+(sizeof(void*)-1)) & ~(sizeof(void*)-1))
#define strsize(string)		((strlen(string)+1)*sizeof(char))
