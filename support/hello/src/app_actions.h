#ifndef HELLO_APP_ACTIONS_H
#define HELLO_APP_ACTIONS_H

enum app_actions {
	APP_ACTION_NONE,
	APP_ACTION_QUIT,
	_APP_ACTIONS_COUNT,
};
typedef enum app_actions app_actions_t;

void handle_app_actions();

#endif
