/**
   @file vslht.c
   
   Implementation for Very Simple Linear Hash Table (VSLHT).
   
   The VSLHT is a simple hash table mapping keys to values.  
   The keys are strings and the values are positive integers. 
   I told you it was simple. 

   $Author$
   $Id$
   $Date$
   $LastChangedDate$
   $LastChangedRevision$
   $LastChangedBy$
*/

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "vslht.h"

/**
Magic numbers 
 */
#define INIT_TABLE_SIZE 16  // Initial table size
#define OCC_FRACTION 0.8    // Max occupancy fraction before resize

#define FALSE 0
#define TRUE 1

/**
   Forward definitions of private parts
 */
static vslht *__vslht_new_size (uint32_t);
static void __vslht_resize (vslht *);
static void __vslht_free_keys (vslht *);
static uint32_t __vslh_key(vslht *, char *);
static uint32_t __vslh_log2 (int);
//static uint32_t __vslh_2_to_the_power (uint32_t);

void *trl_malloc(size_t n) {
  void *foo;

  foo = malloc(n);
  assert(foo!=NULL);
  return (foo);
}

void trl_free(void *foo) {
  free(foo);
}


char *trl_strdup(char *foo) {
  char *another_foo;
  
  another_foo = (char *) trl_malloc(1+strlen(foo));
  strcpy(another_foo, foo);
  return(another_foo);
}
  


/**
   Return a new VSLHT.
   No parameters.  Implementation chooses initial size.
*/
vslht *vslht_new () {
  // Initial size is INIT_TABLE_SIZE 
  return (__vslht_new_size(INIT_TABLE_SIZE));
}


/**
   Empty VSLHT of all key/value pairs.
   Yes, it frees all the memory associated with keys and values for you,
   and undangles references thereto.  
   No, it does not clear the table itself; presumably you still want it 
   or you'd have called [vslht_free] instead, right?
   @param h is the VSLHT you want cleared. 
*/
void vslht_clear (vslht *h) {
  uint32_t i;
  /* Notice that we can't just call __vslht_free_keys here,
     because we don't want to release [h->key].  
     Instead, we free all the strings and then set key pointers to NULL. */
  for (i=0; i<h->size; i++) {
    if (h->key[i] != NULL) 
      trl_free (h->key[i]);
    h->key[i] = NULL;
  }
  h->occ = 0;
}


/**
   Release all memory associated with this VSLHT.
   Yes, it frees the keys as well. 
   @param h is the VSLHT you want freed.  
*/
void vslht_free (vslht *h) {
  __vslht_free_keys(h);
  trl_free(h->val);
  trl_free(h);
}


/**
   Add an key/value pair to the VSLHT.  
   Resize the table, if necessary, before adding the pair.
   A copy of the key (a string) is stored inside the VSLHT.
   If the key is already in the VLSHT, its associated value will
   be overrided.  No, there is no pushback behaviour here. 
   The VSLHT just maintains the \b last association. 
   @param h is the VSHLT to which to add the key/value pair.
   @param k is the key, a string.
   @param v is the value, a positive integer.
*/
/* Details.  
   
In adding a new key/value pair, we resize the table if occupancy
goes over the fraction OCC_FRACTION.                                
   
Our hash table is a very simple one: a pair of arrays.                  
You heard it right, there are no buckets here.                          
I said it was very simple.                                              

We use the hash value, b, that we obtain from                       
our key string as an initial guess at the ``right''                 
array index at which to store the key and its value. 
Starting with b, we search linearly in the h->key array,        
looking for a place to put key,val, wrapping around if necessary.
If, in our linear search, we find key in the table                  
already, then this is an update; we change the corresponding            
value.                                                                  
If, on the other hand, we come across a NULL key that means         
the key is new to the table and so we insert the pair in                
their two arrays at that index.                                         

Notice this OCC_FRACTION stuff.                                     
We require that the occupancy of the table (fraction of array slots     
containing a key) be kept below OCC_FRACTION.       
The value of 0.8 that we use for OCC_FRACTION was chosen empirically
as a good trade-off between unused space and update time. 
*/                                           
void vslht_add (vslht *h, char *key, uint32_t val) {
  uint32_t i,b;
  if ( h->occ >= ((float) h->size) * OCC_FRACTION) 
    // resize hash if necessary
    __vslht_resize (h);
  b = __vslh_key (h,key);
  for (i=b; i<h->size; i++) {
    /* If there is an empty bin at i, then add a key/val pair.              
       Otherwise, if key[i] i matches key, then change the value there. 
       Or, if neither are true, do nothing.     */
    if (h->key[i] == NULL) {
      h->key[i] = (char *) trl_strdup(key);
      h->val[i] = val;
      h->occ ++;
      return ;
    }
    else if ((strcmp(key,h->key[i])) == 0) {
      h->val[i] = val;   
      return;
    }
  }
  for (i=0; i<b; i++) {
    /* If there is an empty bin at i, then add a key/val pair.              
       Otherwise, if the key at i matches key, then change the value there. 
       Or if neither are true, do nothing.  */
    if (h->key[i] == NULL) {
      h->key[i] = (char *) trl_strdup(key);
      h->val[i] = val;
      h->occ ++;
      return ;
    }
    else if ((strcmp(key,h->key[i])) == 0) {
      h->val[i] = val;   
      return;
    }
  }
}


/**
   Determine if a key is in the VSLHT.
   Returns TRUE (1) if the key is present, FALSE (0) otherwise.
   @param h is the VSLHT in which to look
   @param k is the key of interest *
   @return a positive integer standing in for a boolean in this 
   sucky language. 
*/
/* Details. 

Search linearly for key.  If found, return TRUE, else, as
soon as we encounter a NULL key, we know its not there and
so we return FALSE. 
Note that the linear search starts with the target index, 
provided by __vslh_key, but then must wrap around to the beginning.  
*/
uint32_t vslht_mem(vslht *h, char *key) {
  uint32_t i,b;
  b = __vslh_key(h,key);
  for (i=b; i<h->size; i++) {
    // Return false if you find a gap. Return true if there is a match. Else keep looking 
    if (h->key[i] == NULL) 
      return (FALSE);
    else if ((strcmp(h->key[i], key)) == 0) {
      return (TRUE);
    }
  }
  for (i=0; i<b; i++) {
    // Return false if you find a gap. Return true if there is a match. Else keep looking    
    if (h->key[i] == NULL) 
      return (FALSE);
    else if ((strcmp(h->key[i], key)) == 0) {
      return (TRUE);
    }
  }
  return (FALSE);
}


// remove key/val from table
void vslht_remove(vslht *h, char *key) {
  uint32_t i,b;
  b = __vslh_key(h,key);
  for (i=b; i<h->size; i++) {
    // if you find a gap that means key isn't anywhere -- return
    if (h->key[i] == NULL) 
      return;
    else if ((strcmp(h->key[i], key)) == 0) {
      // found it -- remove
      free(h->key[i]);
      h->key[i] = NULL;
      return;
    }
  }
  for (i=0; i<b; i++) {
    // found a gap -- return
    if (h->key[i] == NULL) 
      return;
    else if ((strcmp(h->key[i], key)) == 0) {
      // found it -- remove
      free(h->key[i]);
      h->key[i] = NULL;
      return;
    }
  }
  // I guess this means no gaps and key not there? 
  //  only possible if full occupancy
  return;
}


/**
   Retrieve a value from the VSLHT, given a key. 
   Fails ungracefully if asked about a key not in the VSLHT.
   So please, use [vslht_mem(h,k)], first, if you are unsure if
   you key is there or not. 
   @param h the VSLHT in which to look 
   @param k the key for which you want a value 
   @return a value which is a positive integer. 
*/
/* Details.  
   
Retrieve value from the hash table for key.                        
Fail (as in exit(-1)) if asked for a value that don't exist.                            
Again, we search linearly until we either find key, in which       
case we return the corresponding value, or until we                  
encounter a NULL value in which case we fail since we were    
asked to retreive a value not in the table.                   
*/
uint32_t vslht_find (vslht *h, char *key) {
  uint32_t i,b;
  b = __vslh_key(h,key);
  for (i=b; i<h->size; i++) {
    // Die if you find a gap. Return val if there is a match. Else keep looking
    assert (h->key[i] != NULL);
    if ((strcmp(h->key[i], key)) == 0) 
      return (h->val[i]);
  }
  for (i=0; i<b; i++) {
    // Die if you find a gap. Return val if there is a match. Else keep looking
    assert (h->key[i] != NULL);
    if ((strcmp(h->key[i], key)) == 0) 
      return (h->val[i]);
  }  
  return (h->val[b]);
}


/**
   Copy all the items in one VSLHT into another.
   We don't care a fig if the other VLSHT already contains some
   values.  
   No sharing going on between the two VSLHTs, by the way;
   we copy keys and values over. 
   @param h1 copy key/value pairs *from* this VSLHT
   @param h2 ... *into* this VSLHT
*/
/* Details. 

This is implemented very simply and probably rather sub-optimally. 
We just iterate over the non-NULL keys in h1 and insert them  
along with their values into h2.                        
*/
void vslht_copy (vslht *h1, vslht *h2) {
  uint32_t i;
  for (i=0; i<h1->size; i++) 
    if (h1->key[i] !=NULL) 
      vslht_add (h2, h1->key[i], h1->val[i]);
}


/**
   Returns the key set for a VSLHT.
   This function allocates memory, a brand new array containing the keys
   in the VSLHT.  The size of this array is vslht_occ(h);
   @param h is the VSLHT whose key set you want to retrieve.
   @return an array of strings, the key set for [h]. 
*/
char **vslht_key_set (vslht *h) {
  char **key_set;
  uint32_t i,j;  
  key_set = (char **) trl_malloc (sizeof(char *) * h->occ);
  j=0;
  for (i=0; i<h->size; i++)
    if (h->key[i] != NULL)
      key_set[j++] = (char *) trl_strdup (h->key[i]);
  return (key_set);
}



/**
   Returns some indication of the size of the VSLHT.
   Really, the number of key/value pairs. 
   @param h the VSLHT we want to size 
*/
/* Details.

Hmm.  Why this measure and not h->occ?  
No good answer at present. 
*/
uint32_t vslht_size(vslht *h) {
  return (h->size);
}

uint32_t vslht_occ(vslht *h) {
  return (h->occ);
}

void vslht_check (vslht *h) {
  uint32_t i,o=0;
  for (i=0; i<h->size; i++) 
    o += (h->key[i] != NULL);
  //  printf ("o==%d h->occ=%d\n", o, h->occ);
  assert (o==h->occ);
}


/* \subsection {API helpers}                                                */
/* These are private, thus static.                                          */
/*                                                                          */
/* <API helper functions>=                                                  */
/* Returns a new hash table of this size.  Used internally.                 */
/* The hashing function likes for table size to be a power of two,          */
/* and so we enforce that, making sure that we return a table               */
/* with h->size >= size and h->size a power of two.                 */
/*                                                                          */
/* <Function __vslht_new_size is private and it returns a new table of given size>= */
static vslht *__vslht_new_size (uint32_t size) {
  vslht *h;
  uint32_t i,s2;
  h = (vslht *) trl_malloc (sizeof (vslht));
  s2 = __vslh_log2 (size);
  h->size = 2 << (s2-1);
  if (h->size != size) 
    h->size *= 2;
  h->key = (char **) trl_malloc (sizeof(char *) * h->size);
  h->val = (uint32_t *) trl_malloc (sizeof(uint32_t) * h->size);
  h->occ = 0;
  for (i=0; i<h->size; i++) 
    h->key[i] = NULL;
  return (h);
}

/* Make the table twice as big as it currently is.                          */
/* We have to re-hash everything in order for new table to make sense.      */
/* So we create a new table that is big enough, add the elements one by one, */
/* appropriate the ->key and ->val arrays, and finally free         */
/* the new hash we created and populated.                                   */
/*                                                                          */
/* <Function __vslht_resize doubles the size of a hash table>=          */
static void __vslht_resize (vslht *h) {
  vslht *h2;
  h2 = __vslht_new_size(h->size*2);
  vslht_copy (h,h2); 
  __vslht_free_keys (h);
  trl_free (h->val);
  h->key = h2->key;  
  h->val = h2->val;
  h->size *= 2;
  trl_free (h2);
}

/** Free memory allocated to keys in this VSLHT.                             
    That is, free each of the string in the array and finally free the array itself. 
    @param h is a VSHLT 
*/
static void __vslht_free_keys (vslht *h) {
  uint32_t i;  
  for (i=0; i<h->size; i++)
    trl_free(h->key[i]);
  trl_free(h->key);
}


/**
   \subsection {API helper helpers}
   Not even mine.  
*/

typedef  unsigned long  int  ub4;   // unsigned 4-byte quantities 
typedef  unsigned       char ub1;   // unsigned 1-byte quantities 

// fwd this puppy 
static ub4 jenk_hash(register ub1 *, register ub4, register ub4);

/* <Function __vslh_key is my interface to Jenkins string hashing fn>=  */
static uint32_t __vslh_key(vslht *h, char *key) {
  uint32_t l;
  ub4 jk;
  static ub4 prev_jk=0;
  l = strlen (key);
  if (l==0) {
    return (0);
  }

  jk = jenk_hash((ub1 *) key, (ub4) l, 0);
  prev_jk=jk;
  return (jk & (h->size - 1));
}





/* This is Jenkins' mixing macro.  Also his comments.                       */
/*                                                                          */
/* mix -- mix 3 32-bit values reversibly.                                   */
/*                                                                          */
/* For every delta with one or two bits set, and the deltas of all three    */
/*   high bits or all three low bits, whether the original value of a,b,c   */
/*   is almost all zero or is uniformly distributed,                        */
/*                                                                          */
/* * If mix() is run forward or backward, at least 32 bits in a,b,c         */
/*   have at least 1/4 probability of changing.                             */
/*                                                                          */
/* * If mix() is run forward, every bit of c will change between 1/3 and    */
/*   2/3 of the time.  (Well, 22/100 and 78/100 for some 2-bit deltas.)     */
/*                                                                          */
/* mix() was built out of 36 single-cycle latency instructions in a         */
/*   structure that could supported 2x parallelism, like so:                */
/*                                                                          */
/*       a -= b;                                                        */
/*                                                                          */
/*       a -= c; x = (c>>13);                                           */
/*                                                                          */
/*       b -= c; a ^= x;                                                */
/*                                                                          */
/*       b -= a; x = (a<<8);                                            */
/*                                                                          */
/*       c -= a; b ^= x;                                                */
/*                                                                          */
/*       c -= b; x = (b>>13);                                           */
/*                                                                          */
/*       ...                                                                */
/*   Unfortunately, superscalar Pentiums and Sparcs can't take advantage    */
/*   of that parallelism.  They've also turned some of those single-cycle   */
/*   latency instructions into multi-cycle latency instructions.  Still,    */
/*   this is the fastest good hash I could find.  There were about 2^^68    */
/*   to choose from.  I only looked at a billion or so.                     */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/* <Definitions>=                                                           */
#define jenk_mix(a,b,c) \
{ \
  a -= b; a -= c; a ^= (c>>13); \
  b -= c; b -= a; b ^= (a<<8); \
  c -= a; c -= b; c ^= (b>>13); \
  a -= b; a -= c; a ^= (c>>12);  \
  b -= c; b -= a; b ^= (a<<16); \
  c -= a; c -= b; c ^= (b>>5); \
  a -= b; a -= c; a ^= (c>>3);  \
  b -= c; b -= a; b ^= (a<<10); \
  c -= a; c -= b; c ^= (b>>15); \
}


/*
#define hashsize(n) ((ub4)1<<(n))
#define hashmask(n) (hashsize(n)-1)
*/

/**
   This is Jenkins' hash function for strings .  Also his comments.                      
   
   See http://burtleburtle.net/bob/hash/doobs.html
   
   hash() -- hash a variable-length key into a 32-bit value                 
   
   k       : the key (the unaligned variable-length array of bytes)       
   
   len     : the length of the key, counting by bytes                     
   
   initval : can be any 4-byte value                                      
   
   Returns a 32-bit value.  Every bit of the key affects every bit of       
   the return value.  Every 1-bit and 2-bit delta achieves avalanche.       
   About 6*len+35 instructions.                                             
   
   The best hash table sizes are powers of 2.  There is no need to do       
   mod a prime (mod is sooo slow!).  If you need less than 32 bits,         
   use a bitmask.  For example, if you need only 10 bits, do                
   
   \verb+h = (h & hashmask(10));+                                         
   
   In which case, the hash table should have hashsize(10) elements.         
   
   If you are hashing n strings \verb+(ub1 **)k+, do it like this:      
   
   \verb+for (i=0, h=0; i<n; ++i) h = hash( k[i], len[i], h);+            
   
   By Bob Jenkins, 1996.  bob_jenkins@burtleburtle.net
   You may use this  
   code any way you wish, private, educational, or commercial.  It's free.  
   
   Use for hash table lookup, or anything where one collision in 2^^32 is   
   acceptable.  Do NOT use for cryptographic purposes.                         
*/

static ub4 jenk_hash( k, length, initval)
register ub1 *k;        /* the key */
register ub4  length;   /* the length of the key */
register ub4  initval;  /* the previous hash, or an arbitrary value */
{
   register ub4 a,b,c,len;

   /* Set up the internal state */
   len = length;
   a = b = 0x9e3779b9;  /* the golden ratio; an arbitrary value */
   c = initval;         /* the previous hash value */

   /*---------------------------------------- handle most of the key */
   while (len >= 12)
   {
      a += (k[0] +((ub4)k[1]<<8) +((ub4)k[2]<<16) +((ub4)k[3]<<24));
      b += (k[4] +((ub4)k[5]<<8) +((ub4)k[6]<<16) +((ub4)k[7]<<24));
      c += (k[8] +((ub4)k[9]<<8) +((ub4)k[10]<<16)+((ub4)k[11]<<24));
      jenk_mix(a,b,c);
      k += 12; len -= 12;
   }

   /*------------------------------------- handle the last 11 bytes */
   c += length;
   switch(len)              /* all the case statements fall through */
   {
   case 11: c+=((ub4)k[10]<<24);
   case 10: c+=((ub4)k[9]<<16);
   case 9 : c+=((ub4)k[8]<<8);
      /* the first byte of c is reserved for the length */
   case 8 : b+=((ub4)k[7]<<24);
   case 7 : b+=((ub4)k[6]<<16);
   case 6 : b+=((ub4)k[5]<<8);
   case 5 : b+=k[4];
   case 4 : a+=((ub4)k[3]<<24);
   case 3 : a+=((ub4)k[2]<<16);
   case 2 : a+=((ub4)k[1]<<8);
   case 1 : a+=k[0];
     /* case 0: nothing left to add */
   }
   jenk_mix(a,b,c);
   /*-------------------------------------------- report the result */
   return c;
}

/* \subsection {Miscellaneous functions}                                    */
/*                                                                          */
/* Returns log2(x) rounded down to nearest integer.                         */
/*                                                                          */
/* <Miscellaneous functions>=                                               */
static uint32_t __vslh_log2 (int x) {
  uint32_t i=0;
  while (x>0) {
    x = x>>1;
    i++;
  }
  return (i-1);
}

/* Returns 2**x                                                             */
/*                                                                          */
/* <Miscellaneous functions>=                                               */

