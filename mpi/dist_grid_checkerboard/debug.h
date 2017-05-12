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

//#define VERBOSE 1
#define TIMING 1

#ifdef VERBOSE
#define debug_print(M, ...) do {printf("DEBUG: %s:%d:%s: " M "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__); fflush(stdout);} while(0)
#else
#define debug_print(M, ...)
#endif

#ifdef VERBOSE
#define rank_print(a) do{ printf("@RANK %d - ", a); fflush(stdout);}while(0)
#else
#define rank_print(a)
#endif

#ifdef TIMING
#define time_print(a,b) do{ printf("@Rank %d - Time elapsed: %f\n", a,b);fflush(stdout);}while(0)
#else
#define time_print(a,b)
#endif

#define err_print(M, ...) fprintf(stderr, "ERROR: %s:%d:%s: " M "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)
