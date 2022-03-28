
#ifndef MAIN_H
#define MAIN_H 1

//#include <stddef.h>
	
/*
The __linkl structure for each source module that has file scope initialization
or termination routines.
*/
struct __linkl {
  struct __linkl
		*next;		/* Pointer to the next struct __linkl in
				   the linked list. */
  void		(*ctor)();	/* Pointer to the initialization function,
				   NULL, if none. */
  void		(*dtor)();	/* Pointer to the termination function,
				   NULL, if none. */
};


EXTERN struct __linkl	*__head 
				/* Pointer to the head of the linked list
				   of initialization and termination
				   structures. */
#if VAR_INITIALIZERS
                        = NULL
#endif /* VAR_INITIALIZERS */
                              ;
typedef void (*a_void_function_ptr)();
extern a_void_function_ptr _ctors[];
extern a_void_function_ptr _dtors[];

#endif /* MAIN_H */

