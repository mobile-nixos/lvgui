/**
 * @file anim.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <string.h>
#include "anim.h"
#include "misc/math/math_base.h"
#include "misc/os/ptask.h"
#include "hal/systick/systick.h"


/*********************
 *      DEFINES
 *********************/
#define ANIM_PATH_LENGTH		129	/*Elements in a path array*/
#define ANIM_PATH_START			64  /*In path array a value which corresponds to the start position*/
#define ANIM_PATH_END			192 /* ... to the end position. Not required, just for clearance.*/
#define ANIM_PATH_NORM_SHIFT	7 	/*ANIM_PATH_START - ANIM_PATH_END. Must be 2^N. The exponent goes here. */

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void anim_task (void);
static bool anim_ready_handler(anim_t * a);

/**********************
 *  STATIC VARIABLES
 **********************/
static ll_dsc_t anim_ll;
static uint32_t last_task_run;
static bool anim_del_global_flag = false;

static anim_path_t anim_path_lin[] =
		{64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,  80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,
		 96,  97,  98,  99,  100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127,
		 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
		 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192};

static anim_path_t anim_path_step[] =
		{64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
         64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
         64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 192,};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Init. the animation module
 */
void anim_init(void)
{
	ll_init(&anim_ll, sizeof(anim_t));
	last_task_run = systick_get();
	ptask_create(anim_task, LV_REFR_PERIOD, PTASK_PRIO_MID);
}

/**
 * Create an animation
 * @param anim_p an initialized 'anim_t' variable. Not required after call.
 */
void anim_create(anim_t * anim_p)
{
	/*Add the new animation to the animation linked list*/
	anim_t * new_anim = ll_ins_head(&anim_ll);
	dm_assert(new_anim);

	/*Initialize the animation descriptor*/
	anim_p->playback_now = 0;
	memcpy(new_anim, anim_p, sizeof(anim_t));

	/*Set the start value*/
	new_anim->fp(new_anim->var, new_anim->start);
}

/**
 * Delete an animation for a variable with a given animatior function
 * @param var pointer to variable
 * @param fp a function pointer which is animating 'var',
 *           or NULL to ignore it and delete all animation with 'var
 * @return true: at least 1 animation is deleted, false: no animation is deleted
 */
bool anim_del(void * var, anim_fp_t fp)
{
	bool del = false;
	anim_t * a;
	anim_t * a_next;
	a = ll_get_head(&anim_ll);
	while(a != NULL) {
		/*'a' might be deleted, so get the next object while 'a' is valid*/
		a_next = ll_get_next(&anim_ll, a);

		if(a->var == var && (a->fp == fp || fp == NULL)) {
			ll_rem(&anim_ll, a);
			dm_free(a);
			del = true;
			anim_del_global_flag = true;
		}

		a = a_next;
	}

	return del;
}

/**
 * Calculate the time of an animation with a given speed and the start and end values
 * @param speed speed of animation in unit/sec
 * @param start start value of the animation
 * @param end end value of the animation
 * @return the required time [ms] for the animation with the given parameters
 */
uint16_t anim_speed_to_time(uint16_t speed, int32_t start, int32_t end)
{
	int32_t d = abs((int32_t) start - end);
	uint16_t time = (int32_t)((int32_t)(d * 1000) / speed);

	if(time == 0) {
		time++;
	}

	return time;
}

/**
 * Get a predefine animation path
 * @param name name of the path from 'anim_path_name_t'
 * @return pointer to the path array
 */
anim_path_t * anim_get_path(anim_path_name_t name)
{
	switch (name) {
		case ANIM_PATH_LIN:
			return anim_path_lin;
			break;
		case ANIM_PATH_STEP:
			return anim_path_step;
			break;
		default:
			return NULL;
			break;
	}
}
/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Periodically handle the animations.
 */
static void anim_task (void)
{
	uint32_t elaps;
	elaps = systick_elaps(last_task_run);

	anim_t * a;
	anim_t * a_next;
	a = ll_get_head(&anim_ll);
	while(a != NULL) {
		/*'a' might be deleted, so get the next object while 'a' is valid*/
		a_next = ll_get_next(&anim_ll, a);

		a->act_time += elaps;
		if(a->act_time >= 0) {
			if(a->act_time > a->time) a->act_time = a->time;

			/* Get the index of the path array based on the elapsed time*/
			uint8_t path_i;
			if(a->time == a->act_time) {
                path_i = ANIM_PATH_LENGTH - 1; /*Use the last value id the time fully elapsed*/
			} else {
                path_i = a->act_time * (ANIM_PATH_LENGTH - 1) / a->time;
			}
			/* Get the new value which will be proportional to the current element of 'path_p'
			 * and the 'start' and 'end' values*/
			int32_t new_val;
			new_val =  (int32_t)(a->path[path_i] - ANIM_PATH_START) * (a->end - a->start);
			new_val = new_val >> ANIM_PATH_NORM_SHIFT;
			new_val += a->start;

			a->fp(a->var, new_val);	/*Apply the calculated value*/

			/*If the time is elapsed the animation is ready*/
			if(a->act_time >= a->time) {
				bool invalid;
				invalid = anim_ready_handler(a);
				if(invalid != false) {
					a_next = ll_get_head(&anim_ll);	/*a_next might be invalid if animation delete occurred*/
				}
			}
		}

		a = a_next;
	}

	last_task_run = systick_get();
}

/**
 * Called when an animation is ready to do the necessary thinks
 * e.g. repeat, play back, delete etc.
 * @param a pointer to an animation descriptor
 * @return true: animation delete occurred
 * */
static bool anim_ready_handler(anim_t * a)
{
	bool invalid = false;

	/*Delete the animation if
	 * - no repeat and no play back (simple one shot animation)
	 * - no repeat, play back is enabled and play back is ready */
	if((a->repeat == 0 && a->playback == 0) ||
	   (a->repeat == 0 && a->playback == 1 && a->playback_now == 1)) {
		void (*cb) (void *) = a->end_cb;
		void * p = a->var;
		ll_rem(&anim_ll, a);
		dm_free(a);

		/*Call the callback function at the end*/
		/* Check if an animation is deleted in the cb function
		 * if yes then the caller function has to know this*/
		anim_del_global_flag = false;
		if(cb != NULL) cb(p);
		invalid = anim_del_global_flag;
	}
	/*If the animation is not deleted then restart it*/
	else {
		a->act_time = - a->repeat_pause;	/*Restart the animation*/
		/*Swap the start and end values in play back mode*/
		if(a->playback != 0) {
			/*If now turning back use the 'playback_pause*/
			if(a->playback_now == 0) a->act_time = - a->playback_pause;

			/*Toggle the play back state*/
			a->playback_now = a->playback_now == 0 ? 1: 0;
			/*Swap the start and end values*/
			int32_t tmp;
			tmp = a->start;
			a->start = a->end;
			a->end = tmp;
		}
	}

	return invalid;
}
