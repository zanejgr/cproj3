
#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

// This only works for C
static const char *  currentTime () {
  time_t rawtime;
  struct tm * timeinfo;

  time (&rawtime);
  timeinfo = localtime (&rawtime);
  char * thetime = asctime(timeinfo);
  thetime[strlen(thetime)-1] = '\0';
  return (const char *) thetime;
  //return asctime(timeinfo);
}

// What is the current time
#define DATE_STRING currentTime()

// Add code the only works if the debug flag is enabled
#ifdef NDEBUG
#define debug(M, ...)
#else
#define debug(M, ...) fprintf(stderr, "DEBUG %s:%d: " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#endif


// If using clang, enable the -Wno-gnu-zero-variadic-macro-arguments flag
#define log_trace(M, ...) fprintf(stderr, "%s [trace] (%s:%d) | " M "\n", DATE_STRING,  __FILE__, __LINE__, ##__VA_ARGS__)

#define log_debug(M, ...) fprintf(stderr, "%s [debug] (%s:%d) | " M "\n", DATE_STRING,  __FILE__, __LINE__, ##__VA_ARGS__)

#define log_info(M, ...) fprintf(stderr, "%s [info] (%s:%d) | " M "\n", DATE_STRING,  __FILE__, __LINE__, ##__VA_ARGS__)

#define log_err(M, ...) fprintf(stderr, "%s [error] (%s:%d: errno: %s) | " M "\n", DATE_STRING, __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)

#define log_warn(M, ...) fprintf(stderr, "%s [warn] (%s:%d: errno: %s) | " M "\n", DATE_STRING, __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)

#define check_mem(A) check((A), "Out of memory.")

#define log_timer(tic, toc, M, ...) fprintf(stderr, "%s [INFO] (%s:%d) | " M " (%lf msecs) \n", DATE_STRING,  __FILE__, __LINE__, ##__VA_ARGS__, 1000.0*(toc-tic)/CLOCKS_PER_SEC)


#endif  // UTIL_H
