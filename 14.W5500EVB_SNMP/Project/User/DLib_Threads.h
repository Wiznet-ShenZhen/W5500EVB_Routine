/***************************************************
 *
 * DLib_Threads.h is the library threads manager.
 *
 * Copyright (C) 2004 IAR Systems.  All rights reserved.
 *
 * $Revision: 40607 $
 *
 * This configuration header file sets up how the thread support in the library
 * should work.
 *
 ***************************************************
 *
 * DO NOT MODIFY THIS FILE!
 *
 ***************************************************/

#ifndef _DLIB_THREADS_H
#define _DLIB_THREADS_H

#ifndef _SYSTEM_BUILD
  #pragma system_include
#endif

/*
 * Symbol usage
 * ------------
 * _DLIB_MULTI_THREAD     0 No thread support
 *                        1 Multithread support
 * _DLIB_GLOBAL_VARIABLES 0 Use external TLS interface for the libraries global
 *                          and static variables
 *                        1 Use a lock for accesses to the locale and no 
 *                          security for accesses to other global and static
 *                          variables in the library
 * _DLIB_FILE_OP_LOCKS    0 No file-atomic locks
 *                        1 File-atomic locks

 * _DLIB_COMPILER_TLS     0 No Thread-Local-Storage support in the compiler
 *                        1 Thread-Local-Storage support in the compiler
 * _DLIB_TLS_QUAL         The TLS qualifier, define only if _COMPILER_TLS == 1
 *
 * _DLIB_THREAD_MACRO_SETUP_DONE Whether to use the standard definitions of
 *                               TLS macros defined in xtls.h or the definitions
 *                               are provided here.
 *                        0 Use default macros
 *                        1 Macros defined for xtls.h
 *
 * _DLIB_THREAD_LOCK_ONCE_TYPE
 *                        type for control variable in once-initialization of 
 *                        locks
 * _DLIB_THREAD_LOCK_ONCE_MACRO(control_variable, init_function)
 *                        expression that will be evaluated at each lock access
 *                        to determine if an initialization must be done
 * _DLIB_THREAD_LOCK_ONCE_TYPE_INIT
 *                        initial value for the control variable
 *
 ****************************************************************************
 * Description
 * -----------
 *
 * If _DLIB_MULTI_THREAD != 0, the following lock interface must be defined: 
 *
 *   typedef void *__iar_Rmtx;            // Lock info object
 *   void __iar_Mtxinit(__iar_Rmtx *);    // Initialize a lock
 *   void __iar_Mtxdst(__iar_Rmtx *);     // Destroy a lock
 *   void __iar_Mtxlock(__iar_Rmtx *);    // Lock a lock
 *   void __iar_Mtxunlock(__iar_Rmtx *);  // Unlock a lock
 * 
 * and the three once-initialization symbols must be defined (done in 
 * DLib_product.h):
 *
 *   _DLIB_THREAD_LOCK_ONCE_TYPE
 *   _DLIB_THREAD_LOCK_ONCE_MACRO(control_variable, init_function)
 *   _DLIB_THREAD_LOCK_ONCE_TYPE_INIT
 *
 * and, if an external TLS interface is used, the following must
 * be defined:
 *   typedef int __iar_Tlskey_t;
 *   typedef void (*__iar_Tlsdtor_t)(void *);
 *   int __iar_Tlsalloc(__iar_Tlskey_t *, __iar_Tlsdtor_t); 
 *                                                    // Allocate a TLS element
 *   int __iar_Tlsfree(__iar_Tlskey_t);               // Free a TLS element
 *   int __iar_Tlsset(__iar_Tlskey_t, void *);        // Set a TLS element
 *   void *__iar_Tlsget(__iar_Tlskey_t);              // Get a TLS element
 *
 */

/* We don't have a compiler that supports tls declarations */
#define _DLIB_COMPILER_TLS 0
#define _DLIB_TLS_QUAL 

#if _DLIB_THREAD_SUPPORT == 0

  /* No support for threading. */

  #define _DLIB_MULTI_THREAD 0
  #define _DLIB_GLOBAL_VARIABLES 0
  #define _DLIB_FILE_OP_LOCKS 0

#elif _DLIB_THREAD_SUPPORT == 1

  /* Thread support, TLS via external interface, locks on heap and optionally
     on FILE */

  #define _DLIB_MULTI_THREAD 1
  #define _DLIB_GLOBAL_VARIABLES 0

  _C_LIB_DECL
  typedef void *__iar_Rmtx;

  /* Must be defined */
  __ATTRIBUTES void __iar_Mtxinit(__iar_Rmtx *m);
  __ATTRIBUTES void __iar_Mtxdst(__iar_Rmtx *m);
  __ATTRIBUTES void __iar_Mtxlock(__iar_Rmtx *m);
  __ATTRIBUTES void __iar_Mtxunlock(__iar_Rmtx *m);

  /* Must be defined */
  typedef int __iar_Tlskey_t;
  #if !_DLIB_COMPILER_TLS
    typedef void (*__iar_Tlsdtor_t)(void *);
    __ATTRIBUTES int __iar_Tlsalloc(__iar_Tlskey_t *, __iar_Tlsdtor_t);
    __ATTRIBUTES int __iar_Tlsfree(__iar_Tlskey_t);
    __ATTRIBUTES int __iar_Tlsset(__iar_Tlskey_t, void *);
    __ATTRIBUTES void *__iar_Tlsget(__iar_Tlskey_t);
  #endif /* !_DLIB_COMPILER_TLS */

  _END_C_LIB_DECL

#elif _DLIB_THREAD_SUPPORT == 2

  /* Thread support, no TLS, locks on heap and optionally on FILE */

  #define _DLIB_MULTI_THREAD 1
  #define _DLIB_GLOBAL_VARIABLES 1

  _C_LIB_DECL
  typedef void *__iar_Rmtx;

  /* Must be defined */
  __ATTRIBUTES void __iar_Mtxinit(__iar_Rmtx *m);
  __ATTRIBUTES void __iar_Mtxdst(__iar_Rmtx *m);
  __ATTRIBUTES void __iar_Mtxlock(__iar_Rmtx *m);
  __ATTRIBUTES void __iar_Mtxunlock(__iar_Rmtx *m);

  _END_C_LIB_DECL

#elif _DLIB_THREAD_SUPPORT == 3

  /* Thread support, TLS in a dynamically allocated memory area, locks on heap 
     and optionally on FILE */
  
  /*
   * All variables that should be thread-locale in the library have a place in 
   * a dynamically allocated memory area. The library accesses each variable
   * through a global pointer variable that needs to be thread local (exchanged
   * at each thread context change). Example:
   *   void *__TLSp;
   *
   * Remember to define _DLIB_TLS_POINTER in DLib_config.h. Example:
   *   #define _DLIB_TLS_POINTER __TLSp
   * 
   * At creation of the thread that pointer needs to be initialized to point
   * to enough memory for the __TLS used variables. The needed amount of memory
   * can be found by evaluating 
   *   __segment_end("__TLS_INIT") - __segment_begin("__TLS_INIT");
   * Then the allocated memory has to be initialized. At the destruction of the
   * thread the allocated memory has to be destroyed. Use the following two 
   * functions:
   *   extern void *__iar_Allocate_TLS(void);
   *   extern void  __iar_Deallocate_TLS(void *p);
   * Note that these must be called with functioning locks.
   *
   * For example:
   *
   * at thread_creation:
   *   #include <DLib_Threads.h>
   *   __TLSp = __iar_Allocate_TLS();
   *
   * at thread_destruction:
   *   #include <DLib_Threads.h>
   *   __iar_Deallocate_TLS(__TLSp);
   *
   * These two symbols will change the default for placing the allocated memory
   * area and the initialier area:
   *   _DLIB_TLS_INITIALIZER_MEMORY which memory to place the initializers for 
   *                                the TLS memory area
   *   _DLIB_TLS_MEMORY             which memory to use for the TLS memory area
   */

  #define _DLIB_MULTI_THREAD 1
  #define _DLIB_GLOBAL_VARIABLES 0

  _C_LIB_DECL
  typedef void *__iar_Rmtx;

  /* Must be defined */
  __ATTRIBUTES void __iar_Mtxinit(__iar_Rmtx *m);
  __ATTRIBUTES void __iar_Mtxdst(__iar_Rmtx *m);
  __ATTRIBUTES void __iar_Mtxlock(__iar_Rmtx *m);
  __ATTRIBUTES void __iar_Mtxunlock(__iar_Rmtx *m);

  _END_C_LIB_DECL

  #define _DLIB_THREAD_MACRO_SETUP_DONE 1

  #if !defined(_DLIB_TLS_POINTER)
    #error "Must define symbol _DLIB_TLS_STRUCT_POINTER"
  #endif 

  #if !defined(_DLIB_TLS_INITIALIZER_MEMORY)
    #define _DLIB_TLS_INITIALIZER_MEMORY
  #endif

  #if !defined(_DLIB_TLS_MEMORY)
    #define _DLIB_TLS_MEMORY
  #endif

  extern void *__iar_Allocate_TLS(void);
  extern void __iar_Deallocate_TLS(void *p);

  #pragma language = extended
  
  #define _TLS_DEFINITION(scope, type, name) \
    _Pragma("location = \"__TLS_INIT\" ") \
    scope _DLIB_TLS_INITIALIZER_MEMORY type const name

  #define _INIT_MEM _DLIB_TLS_INITIALIZER_MEMORY char const
  #define _MEM _DLIB_TLS_MEMORY char
  #pragma segment = "__TLS_INIT"

  #pragma inline
  _MEM *__Pointer(_MEM *__pointer, _INIT_MEM *__symbol)
  {
    return __pointer +
           (__symbol - (_INIT_MEM *) __segment_begin("__TLS_INIT"));
  }

  #define _TLS_REFERENCE(type, name, pointer) \
  ((type _DLIB_TLS_MEMORY *) __Pointer((_MEM *) pointer, (_INIT_MEM *) &name))

  #define _TLS_REFERENCE_DEF(type, name) \
    _TLS_REFERENCE(type, name, _DLIB_TLS_POINTER)


  #define _TLS_DATA_DECL(type, name) \
    extern _DLIB_TLS_INITIALIZER_MEMORY type const name

  #define _TLS_DEFINE_INIT(scope, type, name) \
    _TLS_DEFINITION(scope, type, name)

  #define _TLS_DEFINE_NO_INIT(scope, type, name) 

  #define _TLS_DATA_DEF(scope, type, name, init) \
    _TLS_DEFINE_INIT(scope, type, name) = init 

  #define _TLS_DATA_DEF_DT(scope, type, name, init, dtor) \
    _TLS_DEFINE_INIT(scope, type, name) = init 
    /* Make sure that each destructor is inserted into _Deallocate_TLS */
  
  #define _TLS_DATA_PTR(type, name) \
    _TLS_REFERENCE_DEF(type, name)

  #define _TLS_ARR_DEF(scope, type, name, elts) \
    _TLS_DEFINITION(scope, type, name)[elts]

  #define _TLS_ARR(type, name) \
    _TLS_REFERENCE_DEF(type, name)

  #pragma diag_suppress = Pe381

#else
  #error "Thread support erroneously setup"
#endif



#if _DLIB_THREAD_SUPPORT >= 1 && _DLIB_THREAD_SUPPORT <= 3
  _C_LIB_DECL

  #if !defined(_DLIB_THREAD_LOCK_ONCE_TYPE)
    #error "Must define _DLIB_THREAD_LOCK_ONCE_TYPE"
  #endif
  #if !defined(_DLIB_THREAD_LOCK_ONCE_MACRO)
    #error "Must define _DLIB_THREAD_LOCK_ONCE_MACRO"
  #endif
  #if !defined(_DLIB_THREAD_LOCK_ONCE_TYPE_INIT)
    #error "Must define _DLIB_THREAD_LOCK_ONCE_TYPE_INIT"
  #endif

  typedef _DLIB_THREAD_LOCK_ONCE_TYPE _Once_t;
  #define _Once _DLIB_THREAD_LOCK_ONCE_MACRO
  #define _ONCE_T_INIT  _DLIB_THREAD_LOCK_ONCE_TYPE_INIT

  _END_C_LIB_DECL
#endif /* _DLIB_THREAD_SUPPORT >= 1 && _DLIB_THREAD_SUPPORT <= 3 */



#define _MULTI_THREAD _DLIB_MULTI_THREAD
#define _GLOBAL_LOCALE _DLIB_GLOBAL_VARIABLES
#define _FILE_OP_LOCKS _DLIB_FILE_OP_LOCKS

#define _COMPILER_TLS _DLIB_COMPILER_TLS
#define _TLS_QUAL _DLIB_TLS_QUAL

#endif /* _DLIB_THREADS_H */

