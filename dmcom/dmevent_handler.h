#ifndef __DMEVENT_HANDLER_H_
#define __DMEVENT_HANDLER_H_

typedef int dmevent_handle_t;

typedef void (*dmevent_handle_event_cb_t)(void *instance);
typedef dmevent_handle_t (*dmevent_get_handle_cb_t)(void *instance);

struct dmevent_handler {
	void *instance;
	dmevent_handle_event_cb_t handle_event;
	dmevent_get_handle_cb_t get_handle;
};

#endif //__DMEVENT_HANDLER_H_
