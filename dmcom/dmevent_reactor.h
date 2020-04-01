#ifndef __DMEVENT_REACTOR_H_
#define __DMEVENT_REACTOR_H_

#include "dmevent_handler.h"

struct dmevent_reactor;

int dmevent_reactor_register(struct dmevent_reactor *reactor,
							 struct dmevent_handler *handler);
void dmevent_reactor_unregister(struct dmevent_reactor *reactor,
								struct dmevent_handler *handler);

#endif //__DMEVENT_REACTOR_H_
