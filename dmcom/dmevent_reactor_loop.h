#ifndef __DMEVENT_REACTOR_LOOP_H_
#define __DMEVENT_REACTOR_LOOP_H_

struct dmevent_reactor;

struct dmevent_reactor *dmevent_reactor_create(void);
void dmevent_reactor_destroy(struct dmevent_reactor *reactor);
int dmevent_reactor_handle_events(struct dmevent_reactor *reactor);

#endif //__DMEVENT_REACTOR_LOOP_H_
