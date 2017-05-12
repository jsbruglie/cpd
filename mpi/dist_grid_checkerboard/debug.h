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

#define VERBOSE 1
//#define TIMING 1

#ifdef VERBOSE
#define debugPrint(M, ...) do {printf("DEBUG: %s:%d:%s: " M "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__); fflush(stdout);} while(0)
#else
#define debugPrint(M, ...)
#endif

#ifdef VERBOSE
#define rankPrint(a) do{ printf("@RANK %d - ", a); fflush(stdout);} while(0)
#else
#define rankPrint(a)
#endif

#ifdef TIMING
#define timePrint(a,b) do{ printf("@Rank %d - Time elapsed: %f\n", a,b);fflush(stdout);} while(0)
#else
#define timePrint(a,b)
#endif

#define errPrint(M, ...) fprintf(stderr, "ERROR: %s:%d:%s: " M "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)
