#ifdef PROF
// make SHARED=0 "CC=gcc-3.2 -DPROF -finstrument-functions"

#include <stdio.h>

#define BITS 15
#define HASH32(x) (0x9e370001u           * (unsigned          )(x) >> (32-BITS))
#define HASH64(x) (0x9e37fffffffc0001ull * (unsigned long long)(x) >> (64-BITS))

#define HASH(x) (  ((unsigned)(x) >> 2)  &  ((1<<BITS)-1)  )

#ifdef __powerpc__
// Ticks at 1/16 the memory bus clock (6.25 MHz on Albert's Mac Cube)
#define CLOCK_ASM(tbl) asm volatile("mftb %0" : "=r" (tbl))
#endif
#ifdef __i386__
// get 6.25 MHz out of a 200 MHz Pentium-MMX
#define CLOCK_ASM(tbl) do {\
  unsigned long long ull;
  asm volatile("rdtsc" : "=A" (ull));
  tbl = ull >> 5u;  // 200/32 is 6.25
  }while(0)
#endif

typedef struct fninfo {
  void *ptr;      // address of function
  unsigned min;   // minimum cycles used
  unsigned max;   // maximum cycles used
  unsigned tot;   // total cycles used (to compute mean)
  unsigned calls; // number of calls
} fninfo;

static fninfo table[1<<BITS];

static unsigned long stack[128]; // allow calls to go 128 deep
static unsigned stackptr;


void __attribute__((__no_instrument_function__)) __cyg_profile_func_enter(void *tfn, void *csite){
  unsigned long tbl;
  fninfo *entry = table + HASH(tfn);
  if(entry->ptr && entry->ptr != tfn) return; // slot full
  entry->ptr = tfn;                           // claim the slot!
  CLOCK_ASM(tbl);
  stack[stackptr++] = tbl;
}



void __attribute__((__no_instrument_function__)) __cyg_profile_func_exit(void *tfn, void *csite){
  unsigned long tbl, old;
  fninfo *entry;

  CLOCK_ASM(tbl);

  entry = table + HASH(tfn);
  if(entry->ptr != tfn) return; // maybe somebody else claimed the slot
  entry->calls++;

  old = stack[--stackptr];

  tbl -= old; // should be safe for wrap, within 11 minutes

  entry->tot += tbl;
  if(tbl > entry->max) entry->max = tbl;
  if(tbl < entry->min || !entry->min) entry->min = tbl;
}

extern int main(int argc, char *argv[]);

static void __attribute__((__destructor__)) __attribute__((__no_instrument_function__)) spewdata(void){
  unsigned u;

  for(u=0; stack[u]; u++);
  printf("stack was %u deep\n", u);

  printf("main is at %08x\n", (unsigned)(main) );
  printf("spewdata is at %08x\n", (unsigned)(spewdata) );

  u = 1<<BITS;
  while(u--){
    double mean;
    fninfo *entry = table + u;
    if(!entry->ptr) continue;
    mean = (double)entry->tot / entry->calls;
    printf(
      "%08x %10u %10u %10u %8u %#12.2f\n",
      (unsigned)(entry->ptr),
      entry->min,
      entry->max,
      entry->tot,
      entry->calls,
      mean
    );
  }
}

#endif
