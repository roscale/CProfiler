//
// Created by roscale on 11/16/19.
//

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/time.h>

/*
 * Turn profiling on and off
 */
#define PROFILING true

typedef struct {
   const char *name;
   struct timeval start_timepoint;
   bool stopped;
} InstrumentationTimer;

void begin_profiling_session(const char *filename);

void end_profiling_session();

InstrumentationTimer new_instrumentation_timer(const char *name);

void stop_instrumentation_timer(InstrumentationTimer *timer);

void stop_profiling(InstrumentationTimer *timer);

#if PROFILING
#define PROFILE_SCOPE(name)\
    __attribute__((cleanup(stop_profiling)))\
    InstrumentationTimer timer##__LINE__ = new_instrumentation_timer(name)

#else
#define PROFILE_SCOPE(name)
#endif

#define PROFILE_FUNCTION() PROFILE_SCOPE(__func__)