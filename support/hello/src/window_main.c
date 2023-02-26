#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/limits.h>

#include "conf.h"
#include "tbgui_parts.h"
#include "app_state.h"
#include "window_main.h"

window_t* tbgui_main_window_init(void)
{
	window_t* window = create_window("Hello GUI");

	lv_obj_t * description_label = lv_label_create(window->main_container, NULL);
	lv_label_set_long_mode(description_label, LV_LABEL_LONG_BREAK);
	lv_obj_set_width(description_label, lv_obj_get_width_fit(window->main_container));
	lv_label_set_text(
		description_label,
		"Hello!\n\nHopefully this works!"
	);

	// Buttons

	add_button(
		window->actions_container,
		APP_ACTION_QUIT,
		"Quit"
	);

	finalize_window(window);

	app->main_window = window;

	return window;
}
