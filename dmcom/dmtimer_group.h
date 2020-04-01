#ifndef __DMTIMER_GROUP_H_
#define __DMTIMER_GROUP_H_

struct dmtimer_group;

struct dmtimer_group *dmtimer_group_create(void);

void dmtimer_group_destroy(struct dmtimer_group *group);

int dmtimer_group_schedule(struct dmtimer_group *group, int *next_expire_us);

int dmtimer_group_ntimers(struct dmtimer_group *group);

#endif //__DMTIMER_GROUP_H_
