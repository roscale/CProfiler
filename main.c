#include <unistd.h>
#include "instrumentor.h"

void just_wait() {
	PROFILE_FUNCTION();

	sleep(1);
}

int sum(size_t n) {
	PROFILE_FUNCTION();

	int s = 0;
	for (size_t i = 0; i < n; i++) {
		s += i;
	}

	just_wait();
	return s;
}

int main() {
	begin_profiling_session("results.json");

	just_wait();
	printf("%i\n", sum(1000000));
	just_wait();

	end_profiling_session();

	return 0;
}