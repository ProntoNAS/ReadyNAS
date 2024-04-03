/*
* 
* ***** BEGIN LICENSE BLOCK *****
* Source last modified: $Id: mkdepend.c,v 1.1.1.1 2006/03/29 16:45:39 hagi Exp $
* 
* Portions Copyright (c) 1995-2004 RealNetworks, Inc. All Rights Reserved.
* 
* The contents of this file, and the files included with this file,
* are subject to the current version of the RealNetworks Public
* Source License (the "RPSL") available at
* http://www.helixcommunity.org/content/rpsl unless you have licensed
* the file under the current version of the RealNetworks Community
* Source License (the "RCSL") available at
* http://www.helixcommunity.org/content/rcsl, in which case the RCSL
* will apply. You may also obtain the license terms directly from
* RealNetworks.  You may not use this file except in compliance with
* the RPSL or, if you have a valid RCSL with RealNetworks applicable
* to this file, the RCSL.  Please see the applicable RPSL or RCSL for
* the rights, obligations and limitations governing use of the
* contents of the file.
* 
* Alternatively, the contents of this file may be used under the
* terms of the GNU General Public License Version 2 or later (the
* "GPL") in which case the provisions of the GPL are applicable
* instead of those above. If you wish to allow use of your version of
* this file only under the terms of the GPL, and not to allow others
* to use your version of this file under the terms of either the RPSL
* or RCSL, indicate your decision by deleting the provisions above
* and replace them with the notice and other provisions required by
* the GPL. If you do not delete the provisions above, a recipient may
* use your version of this file under the terms of any one of the
* RPSL, the RCSL or the GPL.
* 
* This file is part of the Helix DNA Technology. RealNetworks is the
* developer of the Original Code and owns the copyrights in the
* portions it created.
* 
* This file, and the files included with this file, is distributed
* and made available on an 'AS IS' basis, WITHOUT WARRANTY OF ANY
* KIND, EITHER EXPRESS OR IMPLIED, AND REALNETWORKS HEREBY DISCLAIMS
* ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES
* OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET
* ENJOYMENT OR NON-INFRINGEMENT.
* 
* Technology Compatibility Kit Test Suite(s) Location:
*    http://www.helixcommunity.org/content/tck
* 
* Contributor(s):
* 
* ***** END LICENSE BLOCK *****
*/

#define _BSD_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

#ifdef _WIN32
#define OBJECT_EXT ".obj"
#define PATH_SEP "\\"

/*
 * Returns true if the path in s is an absolute path
 */
int isabs(char *s)
{
  if(s[0] == '/') return 1;
  if(s[0] == '\\') return 1;
  if(s[0] && s[1] == ':')
      if(s[2] == '/' || s[2]== '\\') return 1;
  return 0;
}

#else
#define OBJECT_EXT ".o"
#define PATH_SEP "/"

/*
 * Returns true if the path in s is an absolute path
 */
int isabs(char *s)
{
  return s[0] == '/';
}
#endif

/*
 * Returns a string with a path separator if the string s does not 
 * already end with a path separator.
 */
char *xslash(char *s)
{
  int l=strlen(s)-1;
  if(s[l] == '/' || s[l] == '\\')
    return "";
  else
    return PATH_SEP;
}


/******************************/

/*
 * Easy memory allocator, exit with error if we run out of memory.
 */
char *xalloc(int size)
{
  char *ret=(char *)malloc(size);
  if(!ret)
  {
    fprintf(stderr,"Out of memory (while trying to allocate %d bytes)!\n", size);
    exit(1);
  }
  return ret;
}


/******************************/


/*
 * hash a string
 */
unsigned int hash(char *s, int l)
{
  unsigned int e,ret=l * 0x10001;
  if(l < 64)
  {
    for(e=0;e<l;e++) ret+=(ret<<5) | s[e];
  }else{
    for(e=0;e<32;e++)
      ret+=(ret<<5) | s[e] | (s[l-32+e]<<7);
  }
  return ret;
}


#define STRING_HASH_SIZE 99997
struct STRING
{
  struct STRING *next;
  int refs;
  unsigned int hash;
  int len;
  char str[1];
};


#ifdef VERIFY

int zero;

#define MY_ASSERT(X,OP,Y) do {						\
  if( !( (X) OP (Y) ))							\
  {									\
    fprintf(stderr,"Assertion failed: %s %s %s\n",#X,#OP,#Y);	        \
    fprintf(stderr,"%s=%ld\n",#X,(long)(X));				\
    fprintf(stderr,"%s=%ld\n",#Y,(long)(Y));				\
    fprintf(stderr,"%d\n",1/zero);                                      \
  }									\
}while(0)
 

void check_string(struct STRING *s)
{
  MY_ASSERT(s->refs,>,0);
  MY_ASSERT(s->str[s->len],==,0);
  MY_ASSERT(hash(s->str, s->len),==,s->hash);
}
#else
#define check_string(s) do{}while(0)
#endif

struct STRING *all_strings[STRING_HASH_SIZE];

struct STRING *alloc_string(int l)
{
  struct STRING * ret=(struct STRING *)xalloc(sizeof(struct STRING) + l);
  ret->len=l;
  return ret;
}

struct STRING *finish_string(struct STRING *str)
{
  struct STRING *ret, **p;
  int l=str->len;
  unsigned int h=hash(str->str,l);
  unsigned int H=h%STRING_HASH_SIZE;
  str->str[str->len]=0;

  for(p=all_strings+H;(ret=*p);p = & ret->next)
    {
      if(ret->hash == h && ret->len == l &&
	 !memcmp(str->str, ret->str, l))
	{
	  *p=ret->next;
	  ret->next=all_strings[H];
	  all_strings[H]=ret;
	  free(str);
	  check_string(ret);
	  ret->refs++;
	  return ret;
	}
    }

  ret=str;
  ret->refs=1;
  ret->hash=h;

  ret->next=all_strings[H];
  all_strings[H]=ret;
  check_string(ret);
  return ret;
}

struct STRING *mkstring(char *str, int l)
{
  struct STRING *ret, **p;
  unsigned int h=hash(str,l);
  unsigned int H=h%STRING_HASH_SIZE;

  for(p=all_strings+H;(ret=*p);p = & ret->next)
    {
      if(ret->hash == h &&
	 ret->len == l &&
	 !memcmp(str, ret->str, l))
	{
	  *p=ret->next;
	  ret->next=all_strings[H];
	  all_strings[H]=ret;
	  ret->refs++;
	  check_string(ret);
	  return ret;
	}
    }

  ret=alloc_string(l);
  ret->refs=1;
  ret->hash=h;
  memcpy(ret->str, str, l);
  ret->str[l]=0;

  ret->next=all_strings[H];
  all_strings[H]=ret;
  check_string(ret);
  return ret;
}

struct STRING *mkstring2(char *str)
{
  return mkstring(str, strlen(str));
}


void really_free_string(struct STRING *str)
{
  struct STRING *ret, **p;
  int H=str->hash % STRING_HASH_SIZE;
  for(p=all_strings+H;(ret=*p);p = & ret->next)
  {
    if(ret == str)
    {
      *p=str->next;
      break;
    }
  }
  free(str);
}

void free_string(struct STRING *str)
{
  if(0>=--str->refs) really_free_string(str);
}

#define free_string(X) \
 do { struct STRING *_x=(X); if(0>=--_x->refs) really_free_string(_x); } while(0)

/******************************/

enum StringType
  {
    S_null,
    S_string,
    S_source,
    S_include,
    S_prefix_include,
    S_import,
    S_ignore
  };



struct string_list_entry
{
  enum StringType type;
  struct STRING *s;
};


struct string_list
{
  int len;
  int avail;
  struct string_list_entry *entries;
};

struct string_list zero_string_list;


void string_list_append(struct string_list *lst,
			enum StringType type,
			struct STRING *str)
{
#if DEBUG - 0 > 2
  fprintf(stderr,"string_list_append(%s)\n",str->str);
#endif
  if(lst->avail == lst->len)
    {
      int ns=lst->avail * 2 + 10;
      lst->entries=(struct string_list_entry *)realloc(lst->entries,
						       sizeof(struct string_list_entry) * ns);
      if(!lst->entries)
	{
	  fprintf(stderr,"Out of memory!\n");
          exit(1);
	}
      lst->avail=ns;
    }
  lst->entries[lst->len].s=str;
  lst->entries[lst->len].type=type;
  lst->len++;
  str->refs++;
}

void string_list_append2(struct string_list *lst, char *str)
{
  struct STRING *tmp=mkstring2(str);
  string_list_append(lst, S_string, tmp);
  free_string(tmp);
}

struct string_list *new_string_list()
{
  struct string_list * ret;
#if DEBUG - 0 > 3
  fprintf(stderr,"new_string_list()\n");
#endif
  ret=(struct string_list *)xalloc(sizeof(struct string_list));
  ret->avail=0;
  ret->entries=0;
  ret->len=0;
  return ret;
}

void free_string_list(struct string_list *lst)
{
  int e;
  for(e=0;e<lst->len;e++)
    if(lst->entries[e].s)
      free_string(lst->entries[e].s);

  lst->len=0;
  lst->avail=0;
  if(lst->entries)
  {
    free( (char *) lst->entries);
    lst->entries=0;
  }
}

int string_list_contains(struct string_list *lst,
			 struct STRING *str)
{
  int e;
  for(e=0;e<lst->len;e++) if(lst->entries[e].s == str) return 1;
  return 0;
}

int string_list_contains_type(struct string_list *lst,
			      enum StringType type,
			      struct STRING *str)
{
  int e;
  for(e=0;e<lst->len;e++)
    if(lst->entries[e].s == str &&
      lst->entries[e].type == type) return 1;
  return 0;
}

/******************************/

char *strsplt(char *s, char c)
{
  char *ret;
  if((ret=strchr(s,c))) return ret;
  return s+strlen(s);
}

void split_string(struct string_list *lst, char *str, char sep)
{
  char *tmp;
#if DEBUG - 0 > 4
  fprintf(stderr,"SPLIT: %s / %c\n", str, sep);
#endif
  while((tmp = strsplt(str,sep)))
  {
    if(tmp-str > 1)
    {
      struct STRING *s=mkstring(str, tmp-str);
      string_list_append(lst, S_string, s);
      free_string(s);
#if DEBUG - 0 > 3
      fprintf(stderr,"SPLIT => %s\n",s->str);
#endif
    }
    if(!*tmp) break;
    str=tmp+1;
  }
}

void string_replace(char *s, char from, char to)
{
  while((s = strchr(s,from))) *s=to;
}

char *join_string(struct string_list *lst, char sep)
{
  int len,e;
  char *ret, *tmp;;
  
  for(len=e=0;e<lst->len;e++)
    if(lst->entries[e].type != S_ignore)
      len+=lst->entries[e].s->len+1;
  
  tmp=ret=xalloc(len);
  for(len=e=0;e<lst->len;e++)
  {
    if(lst->entries[e].type == S_ignore) continue;
    memcpy(tmp, lst->entries[e].s->str, lst->entries[e].s->len);
    tmp[lst->entries[e].s->len]=sep;
    tmp+=lst->entries[e].s->len+1;
  }
  tmp[-1]=0;
#if DEBUG -0 > 3
  fprintf(stderr,"JOIN => %s\n",ret);
#endif
  return ret;
}

char *strjoin2(char *a, char *b)
{
  char *ret=xalloc(strlen(a)+strlen(b)+1);
  strcpy(ret,a);
  strcat(ret,b);
  return ret;
}

struct STRING *strjoin2_shared(char *a, char *b)
{
  int l1=strlen(a);
  int l2=strlen(b);
  struct STRING *ret=alloc_string(l1+l2);
  memcpy(ret->str,a,l1);
  memcpy(ret->str+l1,b,l2);
  return finish_string(ret);
}

char *strjoin3(char *a, char *b, char *c)
{
  char *ret=xalloc(strlen(a)+strlen(b)+strlen(c) +1);
#if DEBUG - 0 > 4
  fprintf(stderr,"STRJOIN3: %s   %s   %s\n",a,b,c);
#endif
  strcpy(ret,a);
  strcat(ret,b);
  strcat(ret,c);
  return ret;
}

char *strjoin4(char *a, char *b, char *c, char *d)
{
  char *ret=xalloc(strlen(a)+strlen(b)+strlen(c)+strlen(d) +1);
#if DEBUG - 0 > 4
  fprintf(stderr,"STRJOIN4: %s +  %s +  %s + %s\n",a,b,c,d);
#endif
  strcpy(ret,a);
  strcat(ret,b);
  strcat(ret,c);
  strcat(ret,d);
  return ret;
}

struct STRING *strjoin4_shared(char *a, char *b, char *c, char *d)
{
  int l1=strlen(a);
  int l2=strlen(b);
  int l3=strlen(c);
  int l4=strlen(d);
  struct STRING *ret=alloc_string(l1+l2+l3+l4);
#if DEBUG - 0 > 4
  fprintf(stderr,"STRJOIN4: %s + %s + %s + %s\n",a,b,c,d);
#endif
  memcpy(ret->str, a, l1);
  memcpy(ret->str+l1, b, l2);
  memcpy(ret->str+l1+l2, c, l3);
  memcpy(ret->str+l1+l2+l3, d, l4);
  return finish_string(ret);
}

struct STRING * dirname(struct STRING *file)
{
  char *tmp1=strrchr(file->str,'/');
  char *tmp2=strrchr(file->str,'\\');
  if(tmp2>tmp1) tmp1=tmp2;
  if(tmp1)
  {
    return mkstring(file->str, tmp1 - file->str);
  }else{
    return mkstring2(".");
  }
}

#define ISSPACE(X) ( (X) == ' ' || (X) == '\t' || (X) == '\r' || (X) == '\n')

struct STRING * strip(struct STRING *s)
{
  char *tmp=s->str;
  int l=s->len;
  while(ISSPACE(*tmp)) { tmp++; l--; }
  while(l && ISSPACE(tmp[l-1])) l--;
  return mkstring(tmp, l);
}


/******************************/

/* FIXME: hashes needs to be able to keep track of
 *        string types! (At least on the values)
 */

#define HASH(X) ((unsigned long) (X) )
#define HCHAIN_LEN 2
#define HASHBLOCK 4096

struct hash_entry
{
  struct hash_entry *next;
  struct STRING *key;
  void *value;
  enum StringType type;
};


struct hash
{
  unsigned int hsize, size;
  struct hash_entry ** table;
};

struct hash_entry *free_entries;

struct hash_entry *get_hash_entry(void)
{
  struct hash_entry *ret;
  if(!free_entries)
    {
      int e;
      struct hash_entry *ret=(struct hash_entry *)xalloc(sizeof(struct hash_entry) * HASHBLOCK);
      for(e=0;e<HASHBLOCK;e++)
	{
	  ret->next=free_entries;
	  free_entries=ret;
	  ret++;
	}
    }

  ret=free_entries;
  free_entries=ret->next;
  return ret;
}

void free_hash_entry(struct hash_entry *e)
{
  e->next=free_entries;
  free_entries=e;
}


#ifdef VERIFY
void check_hash(struct hash *hsh)
{
  unsigned int q, s=0;
  struct hash_entry *h;
  for(q=0;q<hsh->hsize;q++)
    {
      for(h=hsh->table[q];h;h=h->next)
	{
	  check_string(h->key);
	  s++;
	  MY_ASSERT(h->value,!=,0);
	  MY_ASSERT(HASH(h->key) % hsh->hsize,==,q);
	}
    }
  MY_ASSERT(hsh->size,==,s);
}
#else
#define check_hash(hsh) do{}while(0)
#endif

void *hash_lookup3(struct hash *hsh,
		   struct STRING *key,
		   int num,
		   int del)
{
  struct hash_entry **hp, *h;
  unsigned int hval=HASH(key);

  check_hash(hsh);

  if(hsh->size == 0) return 0;
  
#if DEBUG - 0 > 2
  fprintf(stderr,"hash_lookup2(key=%s, %d)\n",key->str, num);
#endif
  
  for(hp=hsh->table + (hval % hsh->hsize);(h=*hp);hp=&h->next)
  {
    if(h->key == key)
    {
      void *ret;
      if(num--) continue;
      *hp=h->next;
      ret=h->value;
      if(!del)
      {
	h->next=hsh->table[hval % hsh->hsize];
	hsh->table[hval % hsh->hsize]=h;
      }else{
	hsh->size--;
	free_string(h->key);
	free_hash_entry(h);
      }
      return ret;
    }
  }
  return 0;
}

void *hash_lookup2(struct hash *hsh,
		   struct STRING *key,
		   int num)
{
  return hash_lookup3(hsh, key, num, 0);
}

void *hash_lookup(struct hash *hsh, struct STRING *key)
{
  return hash_lookup2(hsh, key, 0);
}

void hash_insert2(struct hash *hsh,
		  struct STRING *key,
		  void *value,
		  enum StringType type)
{
  unsigned int hval=HASH(key);
  struct hash_entry *h;
#if DEBUG - 0 > 2
  fprintf(stderr,"hash_insert(key=%s)\n",key->str);
#endif

  check_string(key);
  check_hash(hsh);

  if(hsh->size >= hsh->hsize * HCHAIN_LEN)
  {
    int q;
    struct hash_entry **nt;
    unsigned int nsize=hsh->hsize;
    nsize = nsize < 15 ? 15 : nsize * 7;
    /* if(!(nsize % 3)) nsize+=2; */
    nt=(struct hash_entry **)xalloc(sizeof(struct hash_entry *) *  nsize);
    memset(nt,0,sizeof(struct hash_entry *) * nsize);
    
    for(q=0;q<hsh->hsize;q++)
      {
	while(hsh->table[q])
	  {
	    h=hsh->table[q];
	    hsh->table[q]=h->next;
	    h->next=nt[HASH(h->key) % nsize];
	    nt[HASH(h->key) % nsize]=h;
	  }
      }
    if(hsh->table) free(hsh->table);
    hsh->table=nt;
    hsh->hsize=nsize;
    check_hash(hsh);
  }

  h=get_hash_entry();
  key->refs++;
  h->key=key;
  h->value=value;
  h->type=type;
  h->next=hsh->table[hval % hsh->hsize];
  hsh->table[hval % hsh->hsize]=h;
  hsh->size++;
  check_hash(hsh);
}

void hash_insert(struct hash *hsh,
		 struct STRING *key,
		 void *value)
{
  hash_insert2(hsh, key, value, S_null);
}

void free_hash(struct hash *hsh, void (*free_func)(void *))
{
  struct hash_entry *h;
  unsigned int N;
  for(N=0;N<hsh->hsize;N++)
  {
    while((h=hsh->table[N]))
    {
      hsh->table[N]=h->next;
      free_string(h->key);
      free_func(h->value);
      free_hash_entry(h);
    }
  }
  if(hsh->table) free(hsh->table);
}


/*********************************/

void write_string_list(char *filename, struct string_list *lst, char sep)
{
  FILE *f;
  char *tmp=join_string(lst, sep);
#if DEBUG - 0 > 2
  fprintf(stderr,"write_string_list(%s)\n",filename);
#endif
  f=fopen(filename,"w");
  if(!f)
    {
      perror("fopen");
      fprintf(stderr,"Failed to open %s\n",filename);
      exit(1);
    }
  fwrite(tmp,strlen(tmp),1,f);
  fclose(f);
  free(tmp);
}

char *readfile(struct STRING *filename, int *len)
{
  char *ret;
  FILE *f;
#if DEBUG - 0 > 2
  fprintf(stderr,"READFILE: %s\n",filename->str);
#endif
  f=fopen(filename->str,"r");

  if(!f)
    {
      perror("fopen");
      fprintf(stderr,"Failed to open %s\n",filename->str);
      exit(1);
    }

  fseek(f, 0, SEEK_END);
  *len=ftell(f);
  fseek(f, 0, SEEK_SET);

#if DEBUG - 0 > 2
  fprintf(stderr,"LEN: %d\n",*len);
#endif

  ret=xalloc(*len * 2 +1);
  *len=fread(ret, 1, *len, f);
  if(!*len || *len < 0)
    {
      perror("fread");
      fprintf(stderr,"While reading reading %s\n",filename->str);
      exit(1);
    }
  ret[*len]=0;

  fclose(f);

  return ret;
}

/*********************************/

struct hash stat_cache;

struct STAT
{
  struct stat stat;
  int status;
};

struct stat *mystat(struct STRING *filename)
{
  struct STAT *s;
#if DEBUG - 0 > 3
  fprintf(stderr,"STAT: %s\n",filename->str);
#endif
  s=(struct STAT *)hash_lookup(&stat_cache, filename);
  if(!s)
    {
      s=(struct STAT *)xalloc(sizeof(struct STAT));
      s->status=stat(filename->str, & s->stat);
      hash_insert(&stat_cache, filename, s);
    }

  if(s->status) return 0;
  return & s->stat;
}

long mtime(struct STRING *filename)
{
  long ret=0;
  struct stat *st=mystat(filename);
  if(st) ret=st->st_mtime;
#if DEBUG - 0 > 2
  fprintf(stderr,"MTIME: %s => %ld\n",filename->str,ret);
#endif
  return ret;
}

int isfile(struct STRING *filename)
{
  long ret=0;
  struct stat *st=mystat(filename);
  if(st) ret=(st->st_mode & S_IFMT) == S_IFREG ;
#if DEBUG - 0 > 2
  fprintf(stderr,"ISFILE: %s => %ld\n",filename->str,ret);
#endif
  return ret;
}

/*********************************/

struct ulist
{
  struct hash h;
  struct string_list l;
};

struct ulist zero_ulist;

struct ulist *new_ulist(void)
{
  struct ulist * ret=(struct ulist *)xalloc(sizeof(struct ulist));
  *ret=zero_ulist;
  return ret;
}

int ulist_contains(struct ulist *u,
		   enum StringType type,
		   struct STRING *s)
{
  struct hash_entry **hp, *h;
  int loc;
  struct hash *hsh=&u->h;
  unsigned int hval=HASH(s);

  if(!hsh->hsize) return 0;
  
  for(hp=hsh->table + (hval % hsh->hsize);(h=*hp);hp=&h->next)
    {
      if(h->key == s)
	{
	  loc=(long)h->value;
	  if(u->l.entries[loc-1].type == type)
	    {
	      *hp=h->next;
	      h->next=hsh->table[hval % hsh->hsize];
	      hsh->table[hval % hsh->hsize]=h;
	      
	      return loc;
	    }
	  
	}
    }

  return 0;
}

int ulist_add(struct ulist *u,
	      enum StringType type,
	      struct STRING *s)
{
  if(ulist_contains(u,type,s)) return 0;
  hash_insert(& u->h, s, (void *) (u->l.len + 1));
  string_list_append(& u->l, type, s);
  return 1;
}

int ulist_add_last(struct ulist *u,
		   enum StringType type,
		   struct STRING *s)
{
  int ret=1;
  int pos=ulist_contains(u,type,s);
  if(pos)
  {
    if(pos == u->l.len - 1) return 0;
    pos--;
    if(u->l.entries[pos].s)
       free_string(u->l.entries[pos].s);
    u->l.entries[pos].s=0;
    u->l.entries[pos].type=S_null;
    ret=0;
  }
  hash_lookup3(&u->h, s, 0, 1); /* Delete */
  hash_insert(& u->h, s, (void *) (u->l.len + 1));
  string_list_append(& u->l, type, s);
  return ret;
}

int ulist_add2(struct ulist *u,
	       char *str)
{
  int ret;
  struct STRING * tmp=mkstring2(str);
  ret=ulist_add(u, S_string, tmp);
  free_string(tmp);
  return ret;
}

void nullfree(void *n) {}

void free_ulist(struct ulist *u)
{
  free_hash(& u->h, nullfree);
  free_string_list(& u->l);
}

/*********************************/

int hash_insert_ustr2(struct hash *hsh,
		      struct STRING *key,
		      struct STRING *value,
		      enum StringType type)
{
  struct ulist *u=(struct ulist *)hash_lookup(hsh, key);
  if(!u)
  {
    u=new_ulist();
    hash_insert(hsh, key, u);
  }

  return ulist_add(u, type, value);
}

int hash_insert_ustr(struct hash *hsh,
		     struct STRING *key,
		     struct STRING *value)
{
  return hash_insert_ustr2(hsh, key, value, S_string);
}


struct string_list *hash_lookup_all(struct hash *hsh,
				   struct STRING *key)
{
  struct ulist *u=(struct ulist *)hash_lookup(hsh, key);
  if(u)
  {
    return & u->l;
  }else{
    return & zero_string_list;
  }
}

/*********************************/

struct string_builder
{
  char *data;
  int len;
  int malloced;
};

struct string_builder zero_string_builder;

void string_builder_trunc(struct string_builder *b,
			  int len)
{
  b->len=len;
  if(b->data)
  {
    b->data[len]=0;
#if DEBUG - 0 > 5
  fprintf(stderr,"SB: %s (=%d %d/%d)\n", b->data, len, b->len, strlen(b->data));
#endif
  }
}

void string_builder_append(struct string_builder *b,
			   char *data,
			   int len)
{
  if(b->len + len >= b->malloced)
    {
      char *tmp;
      int nl=b->malloced;
      while(b->len + len >= nl) nl+=nl+32;
      tmp=(char *)realloc(b->data,nl);
      if(!tmp)
	{
	  fprintf(stderr,"Out of memory!\n");
	  exit(1);
	}
      b->data=tmp;
      b->malloced=nl;
    }

  memcpy(b->data+b->len, data, len);
  b->len+=len;
  b->data[b->len]=0;
#if DEBUG - 0 > 5
  fprintf(stderr,"SB: %s (+%d %d/%d)\n", b->data, len, b->len, strlen(b->data));
#endif
}

void string_builder_putc(struct string_builder *b,
			 char data)
{
  string_builder_append(b, &data, 1);
}

/*********************************/



struct ulist include_list;
struct string_list prefix_includes;
struct string_list source_list;
struct string_list mrlist;
struct hash dependency_cache;
struct hash include_cache;
struct ulist extentions;
struct ulist all_files;

struct ulist to_expand;

int gnumake = 0;
int absolute = 0;
int process_defines = 0;

struct hash defines;
struct hash define_root;
struct hash define_locations;

struct STRING * current_define_root=0;
enum StringType current_define_root_type;

struct SourceFile
{
  struct STRING *name;
  char *data;
  int pos;
  int len;
  struct ulist includes;
};

#define GOBBLE(X) ((X)->data[(X)->pos++])
#define NEXT(X) ((X)->data[(X)->pos])

void skipspace(struct SourceFile *s)
{
#if DEBUG - 0 > 3
  fprintf(stderr,"skipspace(%d)\n",s->pos);
#endif
  while(1)
    {
      switch(NEXT(s))
	{
	case ' ':
	case '\n':
	case '\r':
	case '\t':
	  s->pos++;
	  break;
	default:
	  return;
	}
    }
}

void skipregspace(struct SourceFile *s)
{
#if DEBUG - 0 > 3
  fprintf(stderr,"skipregspace(%d)\n",s->pos);
#endif
  while(1)
    {
      switch(NEXT(s))
	{
	case ' ':
	case '\t':
	  s->pos++;
	  break;
	default:
	  return;
	}
    }
}

void skipident(struct SourceFile *s)
{
#if DEBUG - 0 > 3
  fprintf(stderr,"skipident(%d)\n",s->pos);
#endif
  while(1)
    {
      int c=NEXT(s);
      if( (c >= '0' && c <= '9') ||
	  (c >= 'a' && c <= 'z') ||
	  (c >= 'A' && c <= 'Z') ||
	  c=='_')
	{
	  s->pos++;
	  continue;
	}
      return;
    }
}

/* Skip over a C-style string */
void skipstr(struct SourceFile *s)
{
#if DEBUG - 0 > 3
  fprintf(stderr,"skipstr()\n");
#endif
  s->pos++;
  while(1)
    {
      switch(GOBBLE(s))
	{
	case 0:
	case '"': return;
	case '\\': s->pos++;
	}
    }
}
/* Skip over a C-style char */
void skipchr(struct SourceFile *s)
{
#if DEBUG - 0 > 3
  fprintf(stderr,"skipchr()\n");
#endif
  s->pos++;
  while(1)
    {
      switch(GOBBLE(s))
	{
	case 0:
	case '\'': return;
	case '\\': s->pos++;
	}
    }
}
/* Skip to > */
void skiptogt(struct SourceFile *s)
{
#if DEBUG - 0 > 3
  fprintf(stderr,"skiptogt()\n");
#endif
  s->pos++;
  while(1)
    {
      switch(GOBBLE(s))
	{
	case 0:
	case '>': return;
	}
    }
}

void skiptoeol(struct SourceFile *s)
{
#if DEBUG - 0 > 3
  fprintf(stderr,"skiptoeol(%d)\n",s->pos);
#endif
  while(1)
    {
      switch(NEXT(s))
	{
	case '\\':
	  s->pos++;
	  if(NEXT(s) == '\r')
	    {
	      s->pos++;
	      if(NEXT(s) == '\n')
		s->pos++;
	    }
	  else if(NEXT(s) == '\n')
	    {
	      s->pos++;
	      if(NEXT(s) == '\r')
		s->pos++;
	    }
	  break;
	    
	case '\n':
	case '\r':
	case 0:
	  return;
	default:
	  s->pos++;
	}
    }
}

void skipccomment(struct SourceFile *s)
{
#if DEBUG - 0 > 3
  fprintf(stderr,"skipccomment()\n");
#endif
  while(1)
    {
      switch(GOBBLE(s))
	{
	case 0:
	case '*':
	  if(NEXT(s) == '/')
	    {
	      s->pos++;
	      return;
	    }
	}
    }
}

void parseinclude(struct SourceFile *s,
		  char *data, 
		  enum StringType type)
{
  struct STRING *inc;
  char *p;
  int l;
  int pd=0;

  while(*data == ' ' || *data == '\t') data++;
  p=data;
  l=0;
  switch(*data)
    {
    case '"':
      data++;
      while(1)
	{
	  switch(*(data++))
	    {
	    case 0: case '"': break;
	    case '\\': data++;
	    default:
	      continue;
	    }
	  break;
	}
      break;
	
    case '<':
      while(*(data++) != '>');
      break;

    default:
      {
	while(1)
	  {
	    switch(*data)
	      {
	      case '\\':
		data++;
		if(*data == '\r') { data++; if(*data == '\n') data++;continue;}
		if(*data == '\n') { data++; if(*data == '\r') data++;continue;}
		continue;
	      default: data++; continue;
	      case '\n': case '\r': case 0: break;
	      }
	    break;
	  }
	l=data - p;
	pd=1;
#ifdef DEBUG
	fprintf(stderr,"Process_defines=%d\n",process_defines);
#endif      
      }
    }
      
  l=data-p;
#if DEBUG - 0 > 4
  fprintf(stderr,"ILEN=%d\n",l);
#endif

  inc=alloc_string(l);
  memcpy(inc->str, p, l);
  inc=finish_string(inc);

#if DEBUG - 0 > 0
  fprintf(stderr,"INC=%s\n",inc->str);
#endif
  
  if(ulist_add_last(& s->includes, type, inc))
    process_defines+=pd;

  free_string(inc);
}


void define(struct STRING *,struct STRING *);

void parsepp(struct SourceFile *s)
{
#if DEBUG - 0 > 3
  fprintf(stderr,"parsepp(%d) %c%c%c%c%c%c%c%c%c%c%c\n",
	  s->pos,
	  s->data[s->pos],
	  s->data[s->pos+1],
	  s->data[s->pos+2],
	  s->data[s->pos+3],
	  s->data[s->pos+4],
	  s->data[s->pos+5],
	  s->data[s->pos+6],
	  s->data[s->pos+7],
	  s->data[s->pos+8],
	  s->data[s->pos+9],
	  s->data[s->pos+10]);
#endif
  s->pos++;
  skipregspace(s);
  if(!strncmp(s->pos+s->data,"include",7))
  {
    parseinclude(s, s->data+s->pos+7,S_include);
  }
  else if(!strncmp(s->pos+s->data,"import",6))
  {
    parseinclude(s, s->data+s->pos+6,S_import);
  }
  else if(!strncmp(s->pos+s->data,"define",6))
  {
    int l,p;
    struct STRING *name, *def;
    s->pos+=6;
    skipregspace(s);
    p=s->pos;
    skipident(s);
    l=s->pos - p;
    
    name=mkstring(s->data+p, l);
    
    p=s->pos;
    skiptoeol(s);
    l=s->pos - p;
    
    def=mkstring(s->data+p, l);
    
    define(name, def);
    free_string(name);
    free_string(def);
  }
  skiptoeol(s);
}

#define FINDHASH() do				\
		{				\
		  ptr--;			\
		  while(ptr >= data)		\
		    {				\
		      switch(*ptr)		\
			{			\
			default: break;		\
			case '#': return ptr;	\
			case ' ':		\
			case '\t':		\
			  ptr--;		\
			  continue;		\
			}			\
		      break;			\
		    }				\
		}while(0)

/* #include  */
/* #import   */
/* #define   */
/*  01234567 */

char * find_last_include(char *data, int len)
{
  char *ptr=data+len-6;
  if(!process_defines)
  {
    while(ptr >= data)
    {
      switch(*ptr)
      {
	case 'm': case 'n': ptr-=1; break;
	case 'p': case 'c': ptr-=2; break;
	case 'o': case 'l': ptr-=3; break;
	case 'r': case 'u': ptr-=4; break;
	case 't': case 'd': ptr-=5; break;
	  
	case 'i':
	  if(!memcmp(ptr+1,"nclude",6) ||
	     !memcmp(ptr+1,"mport",5))
	    FINDHASH();
	default: ptr-=6;
      }
    }
  }else{
    while(ptr >= data)
    {
      switch(*ptr)
      {
	case 'd':
	  if(!memcmp(ptr+1,"efine",5)) FINDHASH();
	  ptr-=5;
	  break;
	  
	case 'm': case 'n': case 'e': ptr-=1; break;
	case 'p': case 'c': case 'f': ptr-=2; break;
	case 'o': case 'l':     /*i*/ ptr-=3; break;
	case 'r': case 'u':     /*n*/ ptr-=4; break;
	case 't':     /*e*/     /*e*/ ptr-=5; break;
	  
	case 'i':
	  if(!memcmp(ptr+1,"nclude",6) ||
	     !memcmp(ptr+1,"mport",5))
	    FINDHASH();
	  ptr-=3;
	  break;
	  
	default: ptr-=6;
      }
    }
  }
  return 0;
}

#define ISALPHA(X) (((X) >= 'a' && (X) <= 'z') || ((X) >= 'A' && (X) <= 'Z'))
#define ISNUM(X) ((X) >= '0' && (X) <= '9')
#define ISALNUM(X) (ISALPHA(X) || ISNUM(X))
#define ISID(X) (ISALNUM(X) || (X) == '_')
#define SKIP(X,F) do { while(F(*(X)) ) (X)++; } while(0)

#define SKIPARG(X) do {				\
  int parlvl=0;					\
  for(;;(X)++)					\
    {						\
      switch(*(X))				\
	{					\
	case '(': parlvl++; continue;		\
	case ')':				\
	  if(!parlvl) break;			\
	  parlvl--;				\
	  continue;				\
	case ',': if(!parlvl) break;		\
	default: continue;			\
        case 0: break;                          \
	}					\
      break;					\
    }						\
} while(0)

struct prep
{
  void (*fun)(struct prep *);
  struct prep *next;
  char *data;
  struct string_builder *out;
  struct defarg *defargs;
  int recur;

  struct ulist *expanded;
};

struct defarg
{
  struct defarg * next;
  char *name;
  int namelen;
  char *value;
};

void preprocess(struct prep *prep);

char *my_strndup(char *x, int len)
{
  char *ret=xalloc(len+1);
  memcpy(ret, x, len);
  ret[len]=0;
  return ret;
}

#define OUTPUT (&out)
#define OUT(X)  string_builder_putc(OUTPUT, (X))
#define OUTSTR(X,Y) string_builder_append(OUTPUT, (X),(Y))

char *fix_hashes(char *fix)
{
  struct string_builder out;
  struct SourceFile s;
  int p;

  out.len=0;
  out.data=0;
  out.malloced=0;

#if DEBUG - 0 > 1
  fprintf(stderr,"fix_hashes(%s)\n", fix);
#endif

  s.name=0;
  s.data=fix;
  s.pos=0;
  s.len=strlen(s.data);
  
  while(1)
    {
      switch(NEXT(&s))
	{
	case (char)255: s.pos++; break;

	case '#':
	  s.pos++;
	  if(NEXT(&s) == '#')
	    {
	      s.pos++;
	      while(ISSPACE(NEXT(&s))) s.pos++;
	      while(OUTPUT->len &&
		    ISSPACE(OUTPUT->data[OUTPUT->len-1]))
		OUTPUT->len--;
	      continue;
	    }
	  OUT('#');
	  break;

	case 0:
#if DEBUG - 0 > 0
	  fprintf(stderr,"fix_hashes(%s) => %s\n", fix, out.data);
#endif
	  return out.data;

	case '"':
	  p=s.pos;
	  skipstr(&s);
	  OUTSTR(s.data+p, s.pos - p);
	  break;

	case '\'':
	  p=s.pos;
	  skipchr(&s);
	  OUTSTR(s.data+p, s.pos - p);
	  break;

	case '\\':
	  s.pos++;
	  if(NEXT(&s) == '\r')
	    {
	      s.pos++;
	      if(NEXT(&s) == '\n')
		s.pos++;
	      continue;
	    }
	  else if(NEXT(&s) == '\n')
	    {
	      s.pos++;
	      if(NEXT(&s) == '\r')
		s.pos++;
	      continue;
	    }
	  OUT('\\');
	  break;

	case '/':
	  s.pos++;
	  if(NEXT(&s) == '*')
	    {
	      skipccomment(&s);
	      break;
	    }
	  if(NEXT(&s) == '/')
	    {
	      skiptoeol(&s);
	      break;
	    }
	  OUT('/');
	  break;

	default:
	  p=s.pos;
	  s.pos++;
	  while(ISID(NEXT(&s)) || ISSPACE(NEXT(&s))) s.pos++;
	  OUTSTR(s.data+p, s.pos - p);
	}
    }
}

#undef OUTPUT
#define OUTPUT (prep->out)


void preprocess_next(struct prep *prep)
{
  preprocess(prep->next);
}

void preprocess_again(struct prep *prep)
{
  struct prep p1;
  p1=*prep->next;
  p1.fun=preprocess_next;
  p1.next=prep->next;
  p1.recur++;
  if(prep->out->data)
  {
    p1.data=fix_hashes(prep->out->data);

#if DEBUG - 0 > 1
    fprintf(stderr,"Expanded: %s\n", p1.data);
#endif
    preprocess(&p1);
    
    free(p1.data);
  }else{
    preprocess(prep->next);
  }
}
void preprocess_next_space(struct prep *prep)
{
  int sl=prep->out->len;
  OUT(' ');
  preprocess(prep->next);
  string_builder_trunc(prep->out, sl);
}

struct ulist *readincludefile(struct STRING *include,
			      enum StringType sftype,
			      struct STRING *basedir);


void add_dependency(struct ulist *deps,
		    enum StringType type,
		    struct STRING *dep,
		    struct STRING *filename)
{
  if(ulist_add(deps, type, dep))
    hash_insert_ustr2(&define_locations, dep, filename, type);
}

void add_dependencies(struct ulist *deps,
		      struct ulist *t,
		      struct STRING *filename)
{
  int l;

  if(ulist_contains(deps, t->l.entries[0].type, t->l.entries[0].s))
    return;

  for(l=0;l<t->l.len;l++)
    add_dependency(deps, t->l.entries[l].type, t->l.entries[l].s, filename);
}

void preprocess_mklist(struct prep *prep)
{
  char *tmp=prep->out->data;
  struct STRING *s;
  int l;
  if(!tmp) return;

  while(ISSPACE(*tmp)) tmp++;
  l=strlen(tmp);
  while(l && ISSPACE(tmp[l-1])) l--;
  s=mkstring(tmp, l);

#ifdef DEBUG
  fprintf(stderr,"Expanded: %s\n",s->str);
#endif
  ulist_add(prep->expanded, S_string, s);

  free_string(s);
}

void preprocess_mklist_fixhashes(struct prep *prep)
{
  char *tmp=fix_hashes(prep->out->data);
  char *tofree=tmp;
  struct STRING *s;
  int l;
  
  if(!tmp) return;

  while(ISSPACE(*tmp)) tmp++;
  l=strlen(tmp);
  while(l && ISSPACE(tmp[l-1])) l--;
  s=mkstring(tmp, l);

#ifdef DEBUG
  fprintf(stderr,"Expanded (fixhash): %s\n",s->str);
#endif

  ulist_add(prep->expanded, S_string, s);

  free(tofree);
  free_string(s);
}

void skipallspace(struct SourceFile *s)
{
  while(1)
  {
    switch(NEXT(s))
    {
      case ' ':
      case '\n':
      case '\r':
      case '\t':
      case '\f':
	s->pos++;
	break;
	
	
      case '/':
	switch(s->data[s->pos + 1])
	{
	  case '*':
	    s->pos++;
	    skipccomment(s);
	    continue;
	  case '/':
	    s->pos++;
	    skiptoeol(s);
	    continue;
	    
	  default:
	    return;
	}
	
      case '\\':
	switch(s->data[s->pos + 1])
	{
	  case '\r':
	  case '\n':
	    s->pos++;
	    continue;
	}
	
      default:
	return;
    }
  }
}

void preprocess(struct prep *prep)
{
  struct prep p1, p2;
  struct SourceFile s;
  int p;
  int saved_len=prep->out->len;
  
  if(prep->recur > 30) return;
  
#ifdef DEBUG
  if(prep->out->data)
    prep->out->data[prep->out->len]=0;
  fprintf(stderr,"Preprocessing(%s + %s, recur=%d)\n",
	  prep->out->data ? prep->out->data : "",
	  prep->data, prep->recur);
#endif
  
  s.name=0;
  s.data=prep->data;
  s.pos=0;
  s.len=strlen(s.data);

  skipallspace(&s);

  while(1)
  {
    p=s.pos;
    skipallspace(&s);
    if(NEXT(&s) && p != s.pos) OUT(' ');
      
    switch(NEXT(&s))
    {
      case 0:
	while(prep->out->len > saved_len &&
	      ISSPACE(prep->out->data[prep->out->len-1]))
	      prep->out->len--;

	prep->fun(prep);
#if DEBUG - 0 > 1
	fprintf(stderr,"Preprocessor returning\n");
#endif
	string_builder_trunc(prep->out, saved_len);
	return;
	
      case '"':
	p=s.pos;
	skipstr(&s);
	OUTSTR(s.data+p, s.pos - p);
	break;
	
      case '\'':
	p=s.pos;
	skipchr(&s);
	OUTSTR(s.data+p, s.pos - p);
	break;
	
      default:
	if(ISALPHA(NEXT(&s)) || NEXT(&s) == '_')
	{
	  p=s.pos;
	  skipident(&s);
	  
	  if(prep->defargs)
	  {
	    struct defarg *da;
	    for(da=prep->defargs;da;da=da->next)
	    {
	      if(s.pos -p  == da->namelen &&
		 !strncmp(da->name, s.data + p, s.pos - p))
	      {
		char *c;
		int x=prep->out->len-1;
		int prehash=0;
		int posthash=0;

#if DEBUG - 0 > 2
		fprintf(stderr,"Inserting %s\n", da->value);
#endif

		while(x >= saved_len && ISSPACE(prep->out->data[x]))
		  x--;
		
		if(x>=saved_len && prep->out->data[x] == '#')
		  prehash++,x--;
		if(x>=saved_len && prep->out->data[x] == '#')
		  prehash++,x--;
		
		if(prehash == 1)
		{
		  x++;
		  prep->out->len=x;
		  prep->out->data[x]=0;
		  OUT('"');
		  c=da->value;
		  while(ISSPACE(*c)) c++;
		  while(1)
		  {
		    switch(*c)
		    {
		      case '\n':
		      case '\t':
		      case '\r':
		      case ' ':
			while(ISSPACE(*c)) c++;
			if(*c)
			{
			  OUT(' ');
			  continue;
			}
			
		      case 0:
			break;
			
		      case '"':
		      case '\\': OUT('\\');
		      default: OUT(*(c++));
			continue;
		    }
		    break;
		  }
		  OUT('"');
		  break;
		}else{
		  skipregspace(&s);
		  if(NEXT(&s) == '#' && s.data[s.pos+1]=='#')
		    posthash=2;
		  
		  if(!prehash && !posthash)
		  {
		    OUT(' ');
		    p1.fun=preprocess_next_space;
		    p1.next=&p2;
		    p1.data=da->value;
		    p1.out=prep->out;
		    p1.defargs=0;
		    p1.recur=prep->recur+1;
		    
		    p2=*prep;
		    p2.data=s.data+s.pos;
		    p2.recur++;

		    preprocess(&p1);

		    string_builder_trunc(prep->out, saved_len);
		    return;
		    
		  }else{
		    char *n=da->value;
		    while(*n)
		      if(!ISSPACE(*n))
			break;
		    if(*n)
		      OUTSTR(da->value, strlen(da->value));
		    else
		      OUT((char)255);
		  }
		}
		break;
	      }
	    }
	    if(!da)
	      OUTSTR(s.data+p, s.pos - p);
	  }
	  else
	  {
	    struct ulist expandlist = zero_ulist;
	    struct ulist expandlist2 = zero_ulist;
	    struct string_list *alldefs;
	    struct STRING *tmp;
	    int D, L;

	    struct string_builder tmp2=zero_string_builder;

	    tmp=mkstring(s.data + p, s.pos - p);
	    skipregspace(&s);

	    alldefs=hash_lookup_all(&defines, tmp);

	    for(D=0;D<alldefs->len;D++)
	    {
	      struct defarg *defargs=0;
	      struct STRING *def=alldefs->entries[D].s;
	      char *d=def->str;
	      char *a=s.data+s.pos;
	      tmp->refs++;
	      current_define_root->refs++;
	      hash_insert_ustr2(&define_root, tmp, current_define_root,
				current_define_root_type);
#ifdef DEBUG
	      fprintf(stderr,"Expanding define(%d): %s %s\n", D,tmp->str,def->str);
#endif
	      if(d[0] == '(' && a[0] == '(')
	      {
		d++;
		a++;
		
		SKIP(d,ISSPACE);
		SKIP(a,ISSPACE);
		while(*a != ')' && *d != ')')
		{
		  struct defarg * defarg;
		  char *dstart, *astart;
		  
#if DEBUG - 0 > 2
		  fprintf(stderr,"ARGPARSE, a=%p (%c%c%c%c), d=%p (%c%c%c%c)\n",
			  a,a[0],a[1],a[2],a[3],
			  d,d[0],d[1],d[2],d[3]);
		  fflush(stderr);
#endif
		  
		  SKIP(d,ISSPACE);
		  SKIP(a,ISSPACE);
		  astart=a;
		  dstart=d;
		  SKIPARG(a);
		  SKIP(d,ISID);
		  defarg=(struct defarg *)xalloc(sizeof(struct defarg));
		  defarg->name=dstart;
		  defarg->namelen=d - dstart;
		  defarg->value=my_strndup(astart, a - astart);
		  defarg->next=defargs;
		  defargs=defarg;
		  
		  SKIP(a,ISSPACE);
		  SKIP(d,ISSPACE);
		  if(*d == ',') d++;
		  if(*a == ',') a++;
		  SKIP(d,ISSPACE);
		  SKIP(a,ISSPACE);
		}
		
		if(*a == ')') a++;
		if(*d == ')') d++;

		s.pos = a - s.data;

		while(ISSPACE(*d)) d++;
#ifdef DEBUG
		fprintf(stderr,"Expanding(%d): %s\n",D,def->str);
#endif
		p1.fun=preprocess_mklist_fixhashes;
		p1.data=d;
		p1.out=&tmp2;
		p1.defargs=defargs; /* sometimes Null */
		p1.recur=prep->recur+1;
		p1.expanded = &expandlist;
		preprocess(&p1);
		
		while(defargs)
		{
		  struct defarg *d;
		  d=defargs;
		  defargs=d->next;
		  free(d->value);
		  free(d);
		}
	      }
	      else if(d[0] != '(')
	      {
		ulist_add(&expandlist, S_string, def);
	      }

	    }

	    /* DANGEROUS */
	    L=alldefs->len;
	    alldefs->len=0;
	    

#if DEBUG - 0 > 2
	    if(D>1)
	      fprintf(stderr,"D: %d (%s)\n",D,tmp->str);
#endif
	    free_string(tmp);

	    for(D=0;D<expandlist.l.len;D++)
	    {
	      p1.fun=preprocess_mklist;
	      p1.data=expandlist.l.entries[D].s->str;
	      p1.out=&tmp2;
	      p1.defargs=0;
	      p1.recur=prep->recur+1;
	      p1.expanded = &expandlist2;
	      preprocess(&p1);
	    }

	    alldefs->len=L;

	    for(D=0;D<expandlist2.l.len;D++)
	    {
	      int L=prep->out->len;
#if DEBUG - 0 > 2
	      fprintf(stderr,"INSERT EXPANDED: %d (%s)\n",D,expandlist2.l.entries[D].s->str);
#endif
	      OUTSTR(expandlist2.l.entries[D].s->str,
		     expandlist2.l.entries[D].s->len);

	      p1=*prep;
	      p1.data=s.data+s.pos;
	      p1.recur++;
	      preprocess(&p1);
	      
	      string_builder_trunc(prep->out, L);
	    }

	    free_ulist(&expandlist);
	    free_ulist(&expandlist2);
	    if(tmp2.data) free(tmp2.data);

	    if(D)
	    {
	      string_builder_trunc(prep->out, saved_len);
	      return;
	    }
	    OUTSTR(s.data+p, s.pos - p);
	  }
	  break;
	}

	OUT(NEXT(&s));
	s.pos++;
    }
  }
}

void preprocess_simple(struct STRING *def,
		       enum StringType type,
		       void (*fun)(struct prep *),
		       struct ulist * expanded)
{
  struct STRING *old_define_root;
  struct string_builder sb;
  struct prep p;
  enum StringType old_define_root_type;

  if(!def) return;

#ifdef DEBUG
  fprintf(stderr,"PPSIMP: %s\n",def->str);
#endif

  old_define_root_type=current_define_root_type;
  old_define_root=current_define_root;
  current_define_root=def;
  current_define_root_type=type;

  sb.data=0;
  sb.len=0;
  sb.malloced=0;

  p.next=0;
  p.data=def->str;
  p.out=&sb;
  p.defargs=0;
  p.recur=0;
  p.expanded = expanded;
  p.fun=fun;

  preprocess(&p);

  if(sb.data) free(sb.data);
  current_define_root=old_define_root;
  current_define_root_type=old_define_root_type;
}

/* Eats refs */
void define(struct STRING *name, struct STRING *def)
{
#if DEBUG - 0 > 0
  fprintf(stderr,"#define %s %s\n",name->str, def->str);
#endif
  
  if(hash_insert_ustr(&defines, name, def))
  {
    int D;
    struct string_list *defines;

    defines=hash_lookup_all(&define_root, name);

    for(D=0;D<defines->len;D++)
      ulist_add(&to_expand, defines->entries[D].type, defines->entries[D].s);
  }
}

void process_delayed(void)
{
  while(to_expand.l.len)
  {
    int D;
    struct string_list defines=to_expand.l;
    free_hash(& to_expand.h, nullfree);
    to_expand=zero_ulist;
    
    for(D=0;D<defines.len;D++)
    {
      int p;
      struct string_list *deflocs;
      struct ulist expanded=zero_ulist;
      preprocess_simple(defines.entries[D].s,
			defines.entries[D].type,
			preprocess_mklist,
			&expanded);
      
      
      deflocs=hash_lookup_all(&define_locations, defines.entries[D].s);
      
      for(p=0;p<deflocs->len;p++)
      {
	int e;
	struct ulist *deps;
	struct STRING *dir;
	struct STRING * filename=deflocs->entries[p].s;
	
	if(!filename) break;
	
	deps=hash_lookup(&dependency_cache,filename);
	
	if(!deps) continue;
	
	for(e=0;e<defines.len;e++)
	  ulist_add(deps, S_ignore, defines.entries[e].s);
	
	dir=dirname(filename);
	
	for(e=0;e<expanded.l.len;e++)
	{
	  struct ulist *t;
	  struct STRING *s=expanded.l.entries[e].s;
	  
	  /* fprintf(stderr,"XDEFEXP: %s (%s)\n",tmp, filename); */
	  if((t=readincludefile(s, defines.entries[D].type, dir)))
	    add_dependencies(deps, t, filename);
	  
	}
	free_string(dir);
      }
      free_ulist(&expanded);
    }
    
    free_string_list(&defines);
  }
}


struct ulist parsecfile(struct STRING *filename)
{
  struct SourceFile s;
  char *li;
#ifdef DEBUG
  fprintf(stderr,"parsecfile(%s)\n",filename->str);
#endif
  
  s.data=readfile(filename, &s.len);
  s.pos=0;
  s.name=filename;
  
  s.includes=zero_ulist;
  
  li=find_last_include(s.data, s.len);
  
#ifdef DEBUG
  fprintf(stderr,"Last include = %p (%d bytes to parse)\n",li, li-s.data);
#endif
  
  while(s.data + s.pos <= li)
  {
    switch(NEXT(&s))
    {
      case 0:
	free(s.data);
	return s.includes;
	
      case '"': skipstr(&s); break;
      case '\'': skipchr(&s); break;
      case '#': parsepp(&s); break;
      case '/':
	s.pos++;
	if(NEXT(&s) == '*')
	{
	  skipccomment(&s);
	  break;
	}
	if(NEXT(&s) == '/')
	{
	  skiptoeol(&s);
	  break;
	}
	continue;
      default:
	s.pos++;
    }
  }
#ifdef DEBUG
  fprintf(stderr,"ParseCfile done at %d\n",s.pos);
#endif
  free(s.data);
  
  return s.includes;
}


struct ulist *readsourcefile(struct STRING *filename,
			     enum StringType sftype)
{
  int e;
  struct ulist includes=zero_ulist;
  struct ulist *deps;
  struct STRING *dir;

#ifdef DEBUG
  fprintf(stderr,"readsourcefile(%s)\n",filename->str);
#endif

  deps=hash_lookup(&dependency_cache, filename);
  if(deps) return deps;

  deps=(struct ulist *)xalloc(sizeof(struct ulist));
  deps[0] = zero_ulist;
  ulist_add(deps, sftype, filename);

  hash_insert(&dependency_cache, filename, deps);

  if(sftype != S_import)
  {
    struct ulist expanded=zero_ulist;
    includes=parsecfile(filename);
    
    dir=dirname(filename);

    

    for(e=0;e<includes.l.len;e++)
    {
      char *q;
      if(!includes.l.entries[e].s) continue;
      q=includes.l.entries[e].s->str;
      while(ISSPACE(*q)) q++;
      if(*q == '<' || *q=='"')
	preprocess_simple(includes.l.entries[e].s,
			  includes.l.entries[e].type,
			  preprocess_mklist,
			  &expanded);
      else
	ulist_add(&to_expand, includes.l.entries[e].type, includes.l.entries[e].s);

      ulist_add(deps, S_ignore, includes.l.entries[e].s);
    }
#ifdef DEBUG
    fprintf(stderr,"After Expanstion: %d\n",expanded.l.len);
#endif

    for(e=0;e<expanded.l.len;e++)
    {
      struct ulist *t;
      struct STRING *s=expanded.l.entries[e].s;
      
      if((t=readincludefile(s, expanded.l.entries[e].type, dir)))
	add_dependencies(deps, t, filename);
      
    }
    
    free_ulist(&includes);
    free_ulist(&expanded);
    free_string(dir);
  }
  
  return deps;
}

struct ulist *readincludefile2(struct STRING *include,
			       enum StringType sftype,
			       struct STRING *dir)
{
  struct ulist *ret=0;
  struct STRING *joined;
  char *c;
  int l,x,il;
  
  if(include->str[0] == '"' || include->str[0] == '<')
  {
    il=include->len;
    
    if(include->str[0] == '"')
    {
      while(il && include->str[il] != '"') il--;
    }else{
      while(il && include->str[il] != '>') il--;
    }
    
    il--;
    if(il <= 0) return ret;
    
    l=dir->len;
    x=dir->str[l-1] != '/' && dir->str[l-1] != '\\';
    joined=alloc_string(l+il+x);
    memcpy(joined->str, dir->str, l);
    if(x) { joined->str[l]='/'; l++; }
    memcpy(joined->str+l, include->str+1, il);
    joined->str[l+il]=0;
    for(c=joined->str;*c;c++) if(*c == '\\') *c='/';
    joined=finish_string(joined);
    
#if DEBUG - 0 > 1
    fprintf(stderr,"JOINED: %s\n",joined->str);
#endif
    if(isfile(joined))
      ret=readsourcefile(joined, sftype);
    free_string(joined);
  }
  return ret;
}

struct ulist *readincludefile(struct STRING *include,
			      enum StringType sftype,
			      struct STRING *basedir)
{
  struct ulist *l;
  int d;
  
#ifdef DEBUG
  fprintf(stderr,"readincludefile(%s,%s)\n",include->str, basedir->str);
#endif
  
  if((l=readincludefile2(include, sftype, basedir)))
    return l;
  
  if((l=hash_lookup(&include_cache, include)))
    return l;
  
  for(d=0;d<include_list.l.len;d++)
  {
    if((l=readincludefile2(include, sftype, include_list.l.entries[d].s)))
    {
      hash_insert(&include_cache, include, l);
      return l;
    }
  }

  return 0;
}

void mkdirhier(struct STRING *dir)
{
  struct STRING *d;
#ifdef DEBUG
  fprintf(stderr,"mkdirhier(%s)\n",dir->str);
#endif
  if(!dir || !dir->len || mtime(dir)) return;
  d=dirname(dir);
  mkdirhier(d);
  free_string(d);
  mkdir(dir->str,0777);
}


void mkdirhier1(struct STRING *dir)
{
  struct STRING *d;
#ifdef DEBUG
  fprintf(stderr,"mkdirhier(%s)\n",dir->str);
#endif
  if(!dir || !dir->len || mtime(dir)) return;
  d=dirname(dir);
  mkdirhier(d);
  free_string(d);
}

#ifdef _WIN32
struct STRING *mkrule(char *obj, char *dep)
{
  return strjoin4_shared(obj, ": \"", dep, "\"");
}
#else

struct STRING *mkrule(char *obj, char *dep)
{
  int lo=strlen(obj);
  struct STRING *ret=alloc_string(lo+strlen(dep)*2+5);
  char *tmp=ret->str;
  char *to=dep;
  char *s=0;

  memcpy(tmp,obj,lo); tmp+=lo;
  *(tmp++)=':';
  *(tmp++)=' ';
  while(*to)
  {
    switch(*to)
    {
      case ' ':
      case '"': if(s) while(s++<to) *(tmp++)='\\'; *(tmp++)='\\';
      default:	*(tmp++)=(*to++); s=0; break;
      case '\\': if(!s) s=to; *(tmp++)=(*to++); break;
    }
  }
  *tmp=0;
  ret->len=tmp - ret->str;

  /* fprintf(stderr,"MKRULE(%s,%s) => %s\n",obj,dep,ret->str); */

  return finish_string(ret);
}
#endif

#define MAGIC "# Dependency magic by $Id: mkdepend.c,v 1.1.1.1 2006/03/29 16:45:39 hagi Exp $\n"

enum ArgState
{
  state_normal,
  state_define,
  state_include,
  state_preinc,
  state_targ,
  state_marg
};

void add_include_dir(char *dir)
{
  if(!isabs(dir) || absolute)
  {
    struct STRING *i=mkstring2(dir);
    ulist_add(&include_list, S_string, i);
    free_string(i);
  }
}


int main(int argc, char **argv)
{
  int e;
  enum ArgState state=state_normal;
  struct STRING *makefile=0;
  char *obj_path="";
  char *tmp;
  char *makefile_data;
  
  for(e=1;e<argc;e++)
  {
#ifdef DEBUG
    fprintf(stderr,"ARG: %s\n",argv[e]);
#endif
    
    switch(state)
    {
      case state_define:
	/* FIXME */
	state=state_normal;
	break;
      case state_include:
	add_include_dir(argv[e]);
	state=state_normal;
	break;
	
      case state_preinc:
	string_list_append(&prefix_includes, S_prefix_include,
			   mkstring2(argv[e]));
	state=state_normal;
	break;
	
      case state_targ:
	obj_path=argv[e];
	state=state_normal;
	break;
	
      case state_marg:
	makefile=mkstring2(argv[e]);
	state=state_normal;
	break;
	
      case state_normal:
	switch(argv[e][0])
	{
	  
	  case '-':
	  case '/':
	    switch(argv[e][1])
	    {
	      case '-':
		if(!strcmp(argv[e],"--gnumake"))
		{
		  gnumake=1;
		  continue;
		}
		if(!strcmp(argv[e],"--defines"))
		{
		  process_defines=0xffffff;
		  continue;
		}
		if(!strcmp(argv[e],"--absolute"))
		{
		  absolute=1;
		  continue;
		}
		break;
		
	      case 'D':
		if(argv[e][2]) continue;
		state=state_define;
		continue;
		
	      case 'I':
	      case 'J':
		if(argv[e][2])
		{
		  add_include_dir(argv[e]+2);
		}else{
		  state=state_include;
		}
		continue;
		
	      case 't':
		if(argv[e][2])
		  obj_path=argv[e]+2;
		else
		  state=state_targ;
		continue;
		
	      case 'm':
		if(argv[e][2])
		  makefile=mkstring2(argv[e]+2);
		else
		  state=state_marg;
		continue;
		
	      case 'F':
		if(argv[e][2] == 'I')
		{
		  if(argv[e][3])
		    string_list_append(&prefix_includes,
				       S_prefix_include,
				       mkstring2(argv[e]+3));
		  else
		    state=state_preinc;
		  continue;
		}
	    }
	}
	
      default:
	string_list_append(&source_list, S_source, mkstring2(argv[e]));
    }
  }
  
  if(makefile)
  {
    if(!isfile(makefile))
    {
      fprintf(stderr,"Makefile=\"%s\" not found\n", makefile->str);
      exit(1);
    }
  }else{
    if(!gnumake)
    {
      fprintf(stderr,"No makefile specified.\n");
      exit(1);
    }
  }
  
  if(makefile)
  {
    int len;
    makefile_data=readfile(makefile, &len);
    if(gnumake && strstr(makefile_data, MAGIC))
    {
      fprintf(stdout,"Dependencies have already been updated automagically.\n");
      exit(0);
    }
    
    tmp=strstr(makefile_data,"\n# DO NOT DELETE");
    if(tmp) *tmp=0;
    
    string_list_append2(&mrlist,makefile_data);
    
    string_list_append2(&mrlist, 
		       "# DO NOT DELETE -- mkdepend depends on this line");
  }
  
#if 0 
  if((tmp=getenv("include")))
    split_string(&include_list, tmp,';');
  
  if((tmp=getenv("C_INCLUDE_PATH")))
    split_string(&include_list, tmp,':');
#endif
  
  ulist_add2(&extentions, ".c");
  ulist_add2(&extentions, ".cc");
  ulist_add2(&extentions, ".cxx");
  
  for(e=0;e<source_list.len;e++)
  {
    struct string_list tmp_rules = zero_string_list;
    struct ulist hlist = zero_ulist;
    struct ulist *hlistp;
    struct STRING *source = source_list.entries[e].s;
    char *ext=0;
    char *base, *oext;
    long deptime, mt;
    int x;
    struct STRING *obj, *dep, *flist;
    
    
#ifdef DEBUG
    fprintf(stderr,"source[%d]=%s\n",e,source->str);
#endif
    
    ext=strrchr(source->str,'.');
    
    if(ext) ulist_add2(&extentions, ext);
    
    base=strdup(source->str);
    if(ext) base[ext-source->str]=0;
#ifdef DEBUG
    fprintf(stderr,"BASE1: %s\n",base);
#endif 
    if(isabs(source->str))
    {
      string_replace(base, '\\', '_');
      string_replace(base, ':', '_');
      string_replace(base, '/', '_');
    }else{
      char *t;
      t=base;
      x=0;
      while(1)
      {
	if(t[0] == '.')
	{
	  if(t[1] == '.')
	  {
	    if(t[2] == '/') { x++; t+=3; continue; }
	    if(t[2] == '\\') { x++; t+=3; continue; }
	  }
	  if(t[1] == '/' ) { t+=2; continue; }
	  if(t[1] == '\\' ) { t+=2; continue; }
	}
	break;
      }
      
      if(x)
      {
	char tmp[30];
	sprintf(tmp, "par%d" PATH_SEP, x);
	t=strjoin2(tmp, t);
	free(base);
	base=t;
      }
    }
    
    
    if(!strcmp(ext,".rc") || !strcmp(ext,".Rc") || !strcmp(ext,".rC")|| !strcmp(ext,".RC"))
      oext=".res";
    else
      oext=OBJECT_EXT;
    
    obj=strjoin4_shared(obj_path,xslash(obj_path), base, oext);
    dep=strjoin4_shared(obj_path,xslash(obj_path), base, ".dep");
    flist=strjoin2_shared(dep->str,"-flist");
    
    deptime=mtime(dep);
    mt=mtime(flist);
    
    if(deptime && mt && mt <= deptime)
    {
      int f, foo;
      tmp=readfile(dep, &foo);
      split_string(&tmp_rules, tmp,'\n');
      free(tmp);
      
      tmp=readfile(flist, &foo);
      split_string(&hlist.l, tmp, '\n');
      free(tmp);
      
      for(f=0;f<hlist.l.len;f++)
      {
	struct STRING *file=hlist.l.entries[f].s;
	mt=mtime(file);
	if(mt > deptime || !mt)
	{
	  deptime=0;
	  break;
	}
      }
    }
    
    if(!deptime)
    {
      free_ulist(&hlist);
      free_string_list(&tmp_rules);
      
      hlistp=readsourcefile(source, S_source);
      process_delayed(); /* Somewhat inefficient */
    }else{
      hlistp=&hlist;
    }
    
    for(x=0;x<prefix_includes.len;x++)
    {
      struct STRING *pi=prefix_includes.entries[x].s;
      struct ulist *l=readsourcefile(pi, S_prefix_include);
      /* This looks suspicious, as it modifies the cache */
      add_dependencies(hlistp, l, source);
      free_string_list(&tmp_rules);
    }
    
    if(!tmp_rules.len)
    {
      if(gnumake)
      {
	tmp=strjoin3(obj->str, " ", dep->str);
      }else{
	tmp=strjoin3(obj->str,"","");
      }
      for(x=0;x<hlistp->l.len;x++)
      {
	if(hlistp->l.entries[x].type != S_ignore)
	  string_list_append(&tmp_rules,
			     S_string,
			     mkrule(tmp,hlistp->l.entries[x].s->str));
      }
      free(tmp);
    }
    
    if(!deptime)
    {
      mkdirhier1(flist);
      write_string_list(flist->str, & hlistp->l,'\n');
      write_string_list(dep->str, &tmp_rules,'\n');
    }
    
    for(x=0;x<hlistp->l.len;x++)
      ulist_add(&all_files,
		hlistp->l.entries[x].type,
		hlistp->l.entries[x].s);
    
    if(!gnumake)
      for(x=0;x<tmp_rules.len;x++)
	string_list_append(&mrlist, S_string, tmp_rules.entries[x].s);
    
    free(base);
    free_string(obj);
    free_string(flist);
    free_string(dep);
  }
  
  if(!makefile)
    exit(0);
  
  if(gnumake)
  {
    /* FIXME */
    struct string_list tmplist=zero_string_list;
    char *p=strdup(obj_path);
    while(1)
    {
      char *q;
      if(!*p) break;
      if(*p == '.')
      {
	if(!p[1]) break;
	if(p[1] == '.' && !p[2]) break;
      }
      if(*p == ':' && !p[1]) break;
      string_list_append(&tmplist,
			 S_string,
			 strjoin4_shared("\ttest -d ",p," || mkdir ",p));
      q=strrchr(p,'/');
      if(q) *q=0; else break;
    }
    free(p);
    
    string_list_append2(&mrlist, MAGIC);
    string_list_append2(&mrlist, "ifneq ($MAKECMDGOALS),clean)");
    string_list_append2(&mrlist, "-include $(COMPILED_OBJS:" OBJECT_EXT "=.dep)");
    string_list_append2(&mrlist, "endif");
    for(e=0;e<extentions.l.len;e++)
    {
      int x;
      string_list_append2(&mrlist, "");
      string_list_append2(&mrlist,
			  strjoin3(obj_path, "/%.dep: %", extentions.l.entries[e].s->str));
      for(x=tmplist.len-1;x>=0;x--)
	string_list_append(&mrlist,S_string, tmplist.entries[x].s);
    }
    
    string_list_append2(&mrlist, "");
  }
  
  string_list_append2(&mrlist,"");
  
  write_string_list(makefile->str,&mrlist, '\n');
  write_string_list(strjoin3(makefile->str,"-mkdep.o",""),& all_files.l, '\n');
  return 0;
}
