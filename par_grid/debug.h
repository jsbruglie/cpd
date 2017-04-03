/** @file debug.h
 *  @brief Macros for debug and verbose options
 *
 *  Contains the Macro definitions for debug and
 *  verbose options.
 *
 *  @author João Borrego
 *  @author Pedro Abreu
 *  @author Miguel Cardoso
 *  @bug No known bugs.
 */

/** 
 *  If `VERBOSE` is defined (via compilation flags)
 *  additional verbose output is produced to track program execution in `stdout`
 */
#ifdef VERBOSE
#define debug_print(M, ...) printf("DEBUG: %s:%d:%s: " M "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#else
#define debug_print(M, ...)
#endif

/** 
 *  If `BENCHMARK` is defined (via compilation flags)
 *  the normal program output is supressed and replaced by time measurements
 */
#ifdef BENCHMARK
#define time_print(M, ...) printf(M, ##__VA_ARGS__)
#define out_print(M, ...) 
#else
#define time_print(M, ...)
#define out_print(M, ...) printf(M, ##__VA_ARGS__)
#endif

#define err_print(M, ...) fprintf(stderr, "ERROR: %s:%d:%s: " M "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)