//
// Created by roscale on 11/16/19.
//

#include "instrumentor.h"

#define timersub(a, b, result)                              \
  do {                                          \
    (result)->tv_sec = (a)->tv_sec - (b)->tv_sec;                  \
    (result)->tv_usec = (a)->tv_usec - (b)->tv_usec;                  \
    if ((result)->tv_usec < 0) {                          \
      --(result)->tv_sec;                              \
      (result)->tv_usec += 1000000;                          \
    }                                          \
  } while (0)

void write_header(Instrumentor *instr) {
	fputs("{\"otherData\": {},\"traceEvents\":[", instr->output);
}

void write_footer(Instrumentor *instr) {
	fputs("]}", instr->output);
}

void write_profile(Instrumentor *instr, const ProfileResult result) {
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
}

Instrumentor *get_instrumentor() {
	static Instrumentor *instr = NULL;
	if (instr == NULL) {
		instr = malloc(sizeof(Instrumentor));
		instr->profile_count = 0;
		instr->output = NULL;
	}
	return instr;
}

void begin_profiling_session(const char *filename) {
	Instrumentor *instr = get_instrumentor();
	instr->output = fopen(filename != NULL ? filename : "results.json", "w");
	write_header(instr);
}

void end_profiling_session() {
	Instrumentor *instr = get_instrumentor();
	write_footer(get_instrumentor());
	fclose(instr->output);
	instr->output = NULL;
	instr->profile_count = 0;
}

InstrumentationTimer new_instrumentation_timer(const char *name) {
	struct timeval start;
	gettimeofday(&start, NULL);

	InstrumentationTimer timer = {name, start, false};
	return timer;
}

void
stop_instrumentation_timer(Instrumentor *instr, InstrumentationTimer *timer) {
	struct timeval end;
	gettimeofday(&end, NULL);

	ProfileResult result = {timer->name, timer->start_timepoint, end, 0};
	write_profile(instr, result);

	timer->stopped = true;
}

void stop_profiling(InstrumentationTimer *timer) {
	stop_instrumentation_timer(get_instrumentor(), timer);
}