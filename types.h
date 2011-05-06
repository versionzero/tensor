
#ifndef _TYPES_H_
#define _TYPES_H_

/* Some very basic types we use everywhere, and are too lazy to use
   the full name */
typedef unsigned char uchar;
typedef unsigned int  uint;

/* Define if the data for the ownded an created by the user, or if it
   is just a view of the data.  We may extend this in the future to
   include copy-on-write. */
typedef enum {
  creator,
  viewer
} ownership_t;

#endif

