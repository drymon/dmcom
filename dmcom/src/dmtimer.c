#include <string.h>
#include "dmlist.h"
#include "dmtimer.h"
#include "dmclock.h"
#include "dmmem.h"
#include "dmtimer_priv.h"

struct dmtimer_group {
	struct dmlist timer_list;
	int ntimers;
};

struct dmtimer {
	struct dmnode node; //N.B This element should be always first
	int timeval_us;
	uint64_t expire_ts;
	dmtimer_expire_cb_t expire_cb;
	void *cbarg;
	bool repeat;
	bool running;
	struct dmtimer_group *group;
};

#define TIMER_OPERATION_DEFAULT { \
	dmclock_getts64, \
	dmmem_alloc, \
	dmmem_free \
}

static struct dmtimer_op g_operation = TIMER_OPERATION_DEFAULT; 

//This function allow double test to inject its operation
void dmtimer_override_op(struct dmtimer_op *op)
{
	if(op->gettime_ns)
		g_operation.gettime_ns = op->gettime_ns;
	if(op->malloc)
		g_operation.malloc = op->malloc;
	if(op->free)
		g_operation.free = op->free;
}

void dmtimer_restore_op(void)
{
	struct dmtimer_op op = TIMER_OPERATION_DEFAULT;
	
	memcpy(&g_operation, &op, sizeof(op));
}

//Timer manager
struct dmtimer_group *dmtimer_group_create(void)
{
	struct dmtimer_group *group = g_operation.malloc(sizeof *group);
	if(!group)
		return NULL;
	memset(group, 0, sizeof *group);

	dmlist_init(&group->timer_list);

	return group;
}

void dmtimer_group_destroy(struct dmtimer_group *group)
{
	struct dmnode *node;
	
	if(!group)
		return;
	//Stop will remove the timer out of the list
	while((node = dmlist_get_head(&group->timer_list)))
		dmtimer_remove_group((struct dmtimer *)node);
	g_operation.free(group);
}

int dmtimer_group_schedule(struct dmtimer_group *group, int *next_expire_us)
{
	struct dmtimer *timer;
	uint64_t cts = g_operation.gettime_ns();
	
	if(!group || !next_expire_us)
		return DM_E_PARAM;

	while((timer = (struct dmtimer *)dmlist_get_head(&group->timer_list))){
		if(timer->expire_ts > cts)
			break;
		dmtimer_stop(timer);
		if(timer->expire_cb)
			timer->expire_cb(timer, timer->cbarg);
        //Not accept repeat and timeval=0, make system polling
		if(dmtimer_is_repeat(timer) && timer->timeval_us != 0) 
			dmtimer_start(timer, timer->timeval_us);
	}
	if(!timer)
		*next_expire_us = -1;
	else
		*next_expire_us = (int)((timer->expire_ts - cts)/DMCLOCK_US_TO_NS);
	
	return DM_E_OK;
}

int dmtimer_add_group(struct dmtimer *timer, struct dmtimer_group *group)
{
	if(!group || !timer)
		return DM_E_PARAM;
	if(timer->group)
		return DM_E_EXIST;

	group->ntimers++;
	timer->group = group;
	
	return 0;
}

void dmtimer_remove_group(struct dmtimer *timer)
{
	if(!timer || !timer->group)
		return;
	dmtimer_stop(timer);
	if(timer->group->ntimers > 0)
		timer->group->ntimers--;
	timer->group = NULL;
}

int dmtimer_group_ntimers(struct dmtimer_group *group)
{
	if(!group)
		return DM_E_PARAM;
	return group->ntimers;
}

//Timer modules
struct dmtimer *dmtimer_create(void)
{
	struct dmtimer *timer = g_operation.malloc(sizeof *timer);
	if(!timer)
		return NULL;
	memset(timer, 0, sizeof *timer);
	
	return timer;
}

int dmtimer_register_expire_cb(struct dmtimer *timer, dmtimer_expire_cb_t expire_cb, void *cbarg)
{
	if(!timer || !expire_cb)
		return DM_E_PARAM;

	if(timer->expire_cb)
		return DM_E_EXIST;
	
	timer->expire_cb = expire_cb;
	timer->cbarg = cbarg;
	
	return DM_E_OK;
}

void dmtimer_unregister_expire_cb(struct dmtimer *timer)
{
	if(!timer)
		return;
	timer->expire_cb = NULL;
	timer->cbarg = NULL;
}

void dmtimer_destroy(struct dmtimer *timer)
{
	if(!timer)
		return;
	dmtimer_stop(timer);
	g_operation.free(timer);
}

void dmtimer_enable_repeat(struct dmtimer *timer)
{
	if(!timer)
		return;
	timer->repeat = true;
}

void dmtimer_disable_repeat(struct dmtimer *timer)
{
	if(!timer)
		return;
	timer->repeat = false;
}

bool dmtimer_is_repeat(struct dmtimer *timer)
{
	if(!timer)
		return false;
	return timer->repeat;
}

bool dmtimer_is_running(struct dmtimer *timer)
{
	if(!timer)
		return false;
	return timer->running;
}

int dmtimer_restart(struct dmtimer *timer, int timeval_us)
{
	dmtimer_stop(timer);
	return dmtimer_start(timer, timeval_us);
}

int dmtimer_start(struct dmtimer *timer, int timeval_us)
{
	struct dmtimer *last_timer;
	struct dmtimer_group *group;
	
	if(!timer)
		return DM_E_PARAM;
	if(timer->running)
		return DM_E_WSTATE;
	if(!timer->group)
		return DM_E_TM_UNGRP;

	group = timer->group;
	timer->timeval_us = timeval_us;
	timer->expire_ts = g_operation.gettime_ns() + (uint64_t)timeval_us * DMCLOCK_US_TO_NS;

	//Add timer in increase order of expire_ts
	last_timer = (struct dmtimer *)dmlist_get_tail(&group->timer_list);
	if(last_timer){
		if(timer->expire_ts > last_timer->expire_ts){
			dmlist_add_tail(&group->timer_list, &timer->node);
		}else{
			struct dmtimer *curtimer = NULL;
			struct dmnode *node;
			
			DMLIST_FOREACH(&group->timer_list, node){
				curtimer = (struct dmtimer *)node;
				if(curtimer->expire_ts > timer->expire_ts)
					break;
			}
			if(curtimer){
				dmlist_add_before(&group->timer_list, &curtimer->node, &timer->node);
			}
		}
	}else{ //list empty
		dmlist_add_tail(&group->timer_list, &timer->node);
	}
	return DM_E_OK;
}

void dmtimer_stop(struct dmtimer *timer)
{
	if(!timer || !timer->running)
		return;
	
	timer->running = false;
	
	if(timer->group)
		dmlist_remove(&timer->group->timer_list, &timer->node);
}

int dmtimer_get_remain(struct dmtimer *timer, int *remain_us)
{
	uint64_t cts = g_operation.gettime_ns();
	
	if(!timer || !remain_us)
		return DM_E_PARAM;
	if(!timer->running)
		return DM_E_WSTATE;
	
	if(cts >= timer->expire_ts){
		*remain_us = 0;
	}else{
		*remain_us = (int)((timer->expire_ts - cts)/DMCLOCK_US_TO_NS);
	}
	return DM_E_OK;
}

struct dmtimer_group *dmtimer_get_group(struct dmtimer *timer)
{
	if(!timer)
		return NULL;
	return timer->group;
}
