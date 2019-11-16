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
   struct timeval start, end;
   uint32_t thread_id;
} ProfileResult;

typedef struct {
   int profile_count;
   FILE *output;
} Instrumentor;

typedef struct {
   const char *name;
   struct timeval start_timepoint;
   bool stopped;
} InstrumentationTimer;

void write_header(Instrumentor *instr);

void write_footer(Instrumentor *instr);

void write_profile(Instrumentor *instr, ProfileResult result);

Instrumentor *get_instrumentor();

void begin_profiling_session(const char *filename);

void end_profiling_session();

InstrumentationTimer new_instrumentation_timer(const char *name);

void
stop_instrumentation_timer(Instrumentor *instr, InstrumentationTimer *timer);

void stop_profiling(InstrumentationTimer *timer);

#if PROFILING
#define PROFILE_SCOPE(name)\
    __attribute__((cleanup(stop_profiling)))\
    InstrumentationTimer timer##__LINE__ = new_instrumentation_timer(name)

#else
#define PROFILE_SCOPE(name)
#endif

#define PROFILE_FUNCTION() PROFILE_SCOPE(__func__)