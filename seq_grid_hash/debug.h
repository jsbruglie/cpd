/** @file debug.h
 *  @brief Macros for debug and verbose options
 *
 *  Contains the Macro definitions for debug and
 *  verbose options.
 *  `debug_print` call functions are not processed by the 
 *  compiler if `VERBOSE` is not defined.
 *
 *  @author João Borrego
 *  @author Pedro Abreu
 *	@author Miguel Cardoso
 *  @bug No known bugs.
 */

#define VERBOSE

#ifdef VERBOSE
#define debug_print(M, ...) printf("DEBUG: %s:%d:%s: " M "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#else
#define debug_print(M, ...)
#endif

#define err_print(M, ...) fprintf(stderr, "ERROR: %s:%d:%s: " M "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)