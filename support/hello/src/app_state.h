#ifndef HELLO_APP_STATE_H
#define HELLO_APP_STATE_H

#include "app_actions.h"

typedef struct app_state {
	app_actions_t action;
	struct window* main_window;
} app_state_t;

extern app_state_t* app;

#endif
