#ifndef __DMTIMER_H_
#define __DMTIMER_H_

#include <stdbool.h>
#include "dmerror.h"

struct dmtimer;

struct dmtimer_group;

typedef void (*dmtimer_expire_cb_t)(struct dmtimer *timer, void *cbarg);

struct dmtimer_group *dmtimer_group_create(void);

void dmtimer_group_destroy(struct dmtimer_group *group);

int dmtimer_group_schedule(struct dmtimer_group *group, int *next_expire_us);

int dmtimer_group_ntimers(struct dmtimer_group *group);

struct dmtimer *dmtimer_create(void);

int dmtimer_register_expire_cb(struct dmtimer *timer,
							   dmtimer_expire_cb_t expire_cb, void *cbarg);

void dmtimer_unregister_expire_cb(struct dmtimer *timer);

int dmtimer_add_group(struct dmtimer *timer, struct dmtimer_group *group);

void dmtimer_remove_group(struct dmtimer *timer);

struct dmtimer_group *dmtimer_get_group(struct dmtimer *timer);
	
void dmtimer_enable_repeat(struct dmtimer *timer);

void dmtimer_disable_repeat(struct dmtimer *timer);

int dmtimer_start(struct dmtimer *timer, int timeval_us);

void dmtimer_stop(struct dmtimer *timer);

int dmtimer_restart(struct dmtimer *timer, int timeval_us);

bool dmtimer_is_running(struct dmtimer *timer);

bool dmtimer_is_repeat(struct dmtimer *timer);

int dmtimer_get_remain(struct dmtimer *timer, int *remain_us);

void dmtimer_destroy(struct dmtimer *timer);

#endif //__DMTIMER_H_
