#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "strnatcmp.h"

#if defined(__GNUC__)
#  define UNUSED __attribute__((__unused__))
#endif

static char const *version UNUSED =
    "$Id: strnatcmp.c,v 1.4 2000/03/05 03:28:42 mbp Exp $";


static int strnatcmp0(char const *a, char const *b, int fold_case)
{
     int ai, bi;
     char ca, cb;
     
     assert(a && b);
     ai = bi = 0;
     while (1) {
	  ca = a[ai]; cb = b[bi];

	  /* skip over leading spaces or zeros */
	  while (isspace(ca)  ||  ca == '0')
	       ca = a[++ai];

	  while (isspace(cb)  ||  cb == '0')
	       cb = b[++bi];

	  /* process run of digits */
	  if (isdigit(ca)  &&  isdigit(cb)) {
	       int bias = 0;
	       /* The longest run of digits (stripping off leading
		  zeros) wins.  That aside, the greatest value wins,
		  but we can't know that it will until we've scanned
		  both numbers to know that they have the same
		  magnitude, so we remember it in BIAS. */
	       while (1) {
		    if (!isdigit(ca)  &&  !isdigit(cb))
			 goto done_number;
		    else if (!isdigit(ca))
			 return -1;
		    else if (!isdigit(cb))
			 return +1;
		    else if (ca < cb) {
			 if (!bias)
			      bias = -1;
		    } else if (ca > cb) {
			 if (!bias)
			      bias = +1;
		    } else if (!ca  &&  !cb)
			 return bias;

		    ca = a[++ai]; cb = b[++bi];
	       }
	  done_number:
	       if (bias)
		    return bias;
	  }

	  if (!ca && !cb) {
	       /* The strings compare the same.  Perhaps the caller
                  will want to call strcmp to break the tie. */
	       return 0;
	  }

	  if (fold_case) {
	       ca = toupper(ca);
	       cb = toupper(cb);
	  }
	  
	  if (ca < cb)
	       return -1;
	  else if (ca > cb)
	       return +1;

	  ++ai; ++bi;
     }
}



int strnatcmp(char const *a, char const *b) {
     return strnatcmp0(a, b, 0);
}


/* Compare, recognizing numeric string and ignoring case. */
int strnatcasecmp(char const *a, char const *b) {
     return strnatcmp0(a, b, 1);
}

