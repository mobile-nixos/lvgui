#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "conf.h"
#include "app_actions.h"
#include "app_state.h"
#include "tbgui_parts.h"

#include "windows.h"

void handle_app_actions()
{
	if (app->action != APP_ACTION_NONE) {
		switch (app->action) {
			case APP_ACTION_NONE:
				break;
			case APP_ACTION_QUIT:
				exit(0);
				break;
			case _APP_ACTIONS_COUNT:
			default:
				printf("Unhandled action: [%d]\n", app->action);
		}
		app->action = APP_ACTION_NONE;
	}
}
