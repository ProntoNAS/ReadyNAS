/*============================================================================*
 *
 * (c) 1995-2002 RealNetworks, Inc. Patents pending. All rights reserved.
 *
 *============================================================================*/
 
// We can only overload new/malloc on the platform; dll stuff on emulator prevents this.


#ifdef __MARM__

#include <e32base.h>
#include <estlib.h>
#include <string.h>



void* operator new(size_t size)
{
    /* based on Symbian document, passing 
       size > KMaxTInt/2 causes the Alloc function
       to throw USER 47 exception.  In order to avoid
       the exception such that the player will gracefully 
       shutdown with out of memory condition,  the following 
       check is added.*/

    if( size > (KMaxTInt/2) )
        User::Leave(KErrNoMemory);

    void *p = User::Alloc(size);    

    if (p)
	memset(p, 0, size);
    else			// allocation failure
	User::Leave(KErrNoMemory);

    return p;
}

void* operator new[](size_t size)
{
    if( size > (KMaxTInt/2) )
        User::Leave(KErrNoMemory);

    void *p = User::Alloc(size);

    if (p)
	memset(p, 0, size);
    else			// allocation failure
	User::Leave(KErrNoMemory);

    return p;
}

void operator delete(void* p)
{
    if (p)
	User::Free(p);
}

void operator delete[](void* p)
{
    if (p)
	User::Free(p);
}

extern "C"
{
    void* malloc(size_t size);
    void* realloc(void* pOld, size_t size);
    void free(void* p);
}

void* malloc(size_t size)
{
    if( size > (KMaxTInt/2) )
        User::Leave(KErrNoMemory);

    void *p = User::Alloc(size);

    if (p == 0)			// allocation failure
	User::Leave(KErrNoMemory);

    return p;
}

void* realloc(void* pOld, size_t size)
{
    void* p = User::ReAlloc(pOld, size);

    if (p == 0)			// allocation failure
	User::Leave(KErrNoMemory);

    return p;
}

void free(void* p)
{
    if (p)
	User::Free(p);
}

#endif // __MARM__
