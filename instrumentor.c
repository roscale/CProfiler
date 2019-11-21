//
// Created by roscale on 11/16/19.
//

#include <assert.h>
#include "instrumentor.h"

typedef struct {
   const char *name;
   struct timeval start, end;
   uint32_t thread_id;
} ProfileResult;

typedef struct {
   int profile_count;
   FILE *output;
} Instrumentor;

static Instrumentor *instr = NULL;

#define timersub(a, b, result)                              \
  do {                                          \
    (result)->tv_sec = (a)->tv_sec - (b)->tv_sec;                  \
    (result)->tv_usec = (a)->tv_usec - (b)->tv_usec;                  \
    if ((result)->tv_usec < 0) {                          \
      --(result)->tv_sec;                              \
      (result)->tv_usec += 1000000;                          \
    }                                          \
  } while (0)

static void write_header() {
	fputs("{\"otherData\": {},\"traceEvents\":[", instr->output);

	if (ferror(instr->output)) {
		fprintf(stderr, "Error writing profiling output.\n");
	}
}

static void write_footer() {
	fputs("]}", instr->output);

	if (ferror(instr->output)) {
		fprintf(stderr, "Error writing profiling output.\n");
	}
}

static void write_profile(const ProfileResult result) {
	if (instr->profile_count++ > 0)
		fputs(",", instr->output);

	struct timeval interval;
	timersub(&result.end, &result.start, &interval);

	fputs("{", instr->output);
	fputs("\"cat\":\"function\",", instr->output);
	fprintf(instr->output, "\"dur\":%li,",
	        1000000 * interval.tv_sec + interval.tv_usec);
	fprintf(instr->output, "\"name\":\"%s\",", result.name);
	fputs("\"ph\":\"X\",", instr->output);
	fputs("\"pid\":0,", instr->output);
	fprintf(instr->output, "\"tid\":%u,", result.thread_id);
	fprintf(instr->output, "\"ts\":%li",
	        1000000 * result.start.tv_sec + result.start.tv_usec);
	fputs("}", instr->output);

	if (ferror(instr->output)) {
		fprintf(stderr, "Error writing profiling output.\n");
	}
}

void begin_profiling_session(const char *filename) {
#if PROFILING
	assert(instr == NULL && "Profiling session already started."
	                        "Did you call begin_profiling_session() multiple times?");

	// Initialize instrumentor
	instr = malloc(sizeof(Instrumentor));
	if (instr == NULL) {
		fprintf(stderr, "[Instrumentor] Failed to allocate memory\n");
		abort();
	}
	instr->profile_count = 0;
	instr->output = fopen(filename != NULL ? filename : "results.json", "w");
	if (instr->output == NULL) {
		fprintf(stderr, "[Instrumentor] Failed to open or create file '%s'\n",
		        filename);
		abort();
	}
	write_header();
#endif
}

void end_profiling_session() {
#if PROFILING
	assert(instr != NULL &&
	       "Trying to end a nonexistent profiling session."
	       "Did you forget to call begin_profiling_session()?");

	write_footer();
	fclose(instr->output);
	free(instr);
	instr = NULL;
#endif
}

InstrumentationTimer new_instrumentation_timer(const char *name) {
	struct timeval start;
	gettimeofday(&start, NULL);

	InstrumentationTimer timer = {name, start, false};
	return timer;
}

void
stop_instrumentation_timer(InstrumentationTimer *timer) {
	if (instr != NULL && !timer->stopped) {
		struct timeval end;
		gettimeofday(&end, NULL);

		ProfileResult result = {timer->name, timer->start_timepoint, end, 0};
		write_profile(result);

		timer->stopped = true;
	}
}

void stop_profiling(InstrumentationTimer *timer) {
	stop_instrumentation_timer(timer);
}