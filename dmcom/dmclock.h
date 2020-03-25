#ifndef __DMCLOCK_H_
#define __DMCLOCK_H_

#include <stdint.h>
#include <time.h>

#define DMCLOCK_SEC_TO_MS 1000u
#define DMCLOCK_MS_TO_US 1000u
#define DMCLOCK_SEC_TO_US 1000000u
#define DMCLOCK_US_TO_NS 1000u
#define DMCLOCK_MS_TO_NS 1000000u
#define DMCLOCK_SEC_TO_NS 1000000000u

#ifndef CLOCK_MONOTONIC
#define DMCLOCK_ID CLOCK_MONOTONIC
#elif defined CLOCK_REALTIME
#define DMCLOCK_ID CLOCK_REALTIME
#else
#error "Clock is unavailable"
#endif

static inline uint64_t dmclock_getts64(void)
{
	struct timespec cts;
	uint64_t cts64;	
	
	if(clock_gettime(DMCLOCK_ID, &cts) < 0){
		return 0;
	}
	
	cts64 = (uint64_t)cts.tv_sec * DMCLOCK_SEC_TO_NS + (uint64_t)cts.tv_nsec;
	
	return cts64;
}

#endif //__DMCLOCK_H_
