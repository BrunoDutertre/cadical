#include "internal.hpp"

/*------------------------------------------------------------------------*/

// This is pretty Linux specific code for reporting resource usage.
// TODO: port these functions to different OS.

// BD: windows compatible variants
// we cross-compile using mingw from cygwin

extern "C" {
#include <time.h>
#include <stdint.h>
#include <Windows.h>
}

namespace CaDiCaL {

// double absolute_real_time () {
//   struct timeval tv;
//   if (gettimeofday (&tv, 0)) return 0;
//   return 1e-6 * tv.tv_usec + tv.tv_sec;
// }

double absolute_real_time () {
  /*
   * GetSystemTimeAsFileTime: returns the current time using file_times.
   * 
   * A file time is a 64-bit value that represents the number of
   * 100-nanosecond intervals that have elapsed since 12:00
   * A.M. January 1, 1601 Coordinated Universal Time (UTC). The system
   * records file times when applications create, access, and write to
   * files.
   *
   * ft contains two 32bit word:
   *   dwLowDateTime = lower-order half
   *   dwHighDateTime = high-order half
   *
   * We convert to milliseconds to be consitent with the Linux implementation
   * above (but we don't adjust for a different start time).
   */
  LPFILETIME ft;
  uint64_t full_time;
  GetSystemTimeAsFileTime(&ft);
  full_time = ((uint64_t) ft.dwLowDateTime) + (((uint64_t) ft.dwHighDateTime) << 32);

  return (double) (full_time/10);
}

double Internal::real_time () {
  return absolute_real_time () - stats.time.real;
}

/*------------------------------------------------------------------------*/

// We use 'getrusage' for 'process_time' and 'maximum_resident_set_size'
// which is pretty standard on Unix but probably not available on Windows
// etc.  For different variants of Unix not all fields are meaningful.


// double absolute_process_time () {
//   struct rusage u;
//   double res;
//   if (getrusage (RUSAGE_SELF, &u)) return 0;
//   res = u.ru_utime.tv_sec + 1e-6 * u.ru_utime.tv_usec;  // user time
//   res += u.ru_stime.tv_sec + 1e-6 * u.ru_stime.tv_usec; // + system time
//   return res;
// }


// Windows compatible variant
double absolute_process_time () {
  return ((double) clock())/CLOCKS_PER_SEC;
}

double Internal::process_time () {
  return absolute_process_time () - stats.time.process;
}




/*------------------------------------------------------------------------*/

// This seems to work on Linux (man page says since Linux 2.6.32).

// size_t maximum_resident_set_size () {
//   struct rusage u;
//   if (getrusage (RUSAGE_SELF, &u)) return 0;
//   return ((size_t) u.ru_maxrss) << 10;
// }

// peak size in MB
size_t maximum_resident_set_size () {
  PROCESS_MEMORY_COUNTERS pmc;
  size_t bytes;

  bytes = 0;
  if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
    bytes = pmc.PeakWorkingSetSize;
  }
  return bytes >> 20;  
}

// Unfortunately 'getrusage' on Linux does not support current resident set
// size (the field 'ru_ixrss' is there but according to the man page
// 'unused'). Thus we fall back to use the '/proc' file system instead.  So
// this is not portable at all and needs to be replaced on other systems
// The code would still compile though (assuming 'sysconf' and
// '_SC_PAGESIZE' are available).

// size_t current_resident_set_size () {
//   char path[40];
//   sprintf (path, "/proc/%" PRId64 "/statm", (int64_t) getpid ());
//   FILE * file = fopen (path, "r");
//   if (!file) return 0;
//   int64_t dummy, rss;
//   int scanned = fscanf (file, "%" PRId64 " %" PRId64 "", &dummy, &rss);
//   fclose (file);
//   return scanned == 2 ? rss * sysconf (_SC_PAGESIZE) : 0;
// }

size_t current_resident_set_size () {
  PROCESS_MEMORY_COUNTERS pmc;
  size_t bytes;

  bytes = 0;
  if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
    bytes = pmc.WorkingSetSize;
  }
  return bytes >> 20;  
}


}

