/**
 * @file lv_templ.h
 * 
 */


/*Search an replace: template -> object normal name with lower case (e.g. button, label etc.)
 * 					 templ -> object short name with lower case(e.g. btn, label etc)
 *                   TEMPL -> object short name with upper case (e.g. BTN, LABEL etc.)
 *
 */

#ifndef LV_TEMPL_H
#define LV_TEMPL_H

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_TEMPL != 0

#include "../lv_obj/lv_obj.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
/*Data of template*/
typedef struct
{
    /*Ext. of ancestor*/
    /*New data for this type */
}lv_templ_ext_t;

/*Style of template*/
typedef struct
{
	/*Style of ancestor*/
	/*New style element for this type */
}lv_templs_t;

/*Built-in styles of template*/
typedef enum
{
	LV_TEMPLS_DEF,
}lv_templs_builtin_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a template objects
 * @param par pointer to an object, it will be the parent of the new template
 * @param copy pointer to a template object, if not NULL then the new object will be copied from it
 * @return pointer to the created template
 */
lv_obj_t * lv_templ_create(lv_obj_t * par, lv_obj_t * copy);

/**
 * Signal function of the template
 * @param templ pointer to a template object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return true: the object is still valid (not deleted), false: the object become invalid
 */
bool lv_templ_signal(lv_obj_t * templ, lv_signal_t sign, void * param);

/**
 * Return with a pointer to a built-in style and/or copy it to a variable
 * @param style a style name from lv_templs_builtin_t enum
 * @param copy copy the style to this variable. (NULL if unused)
 * @return pointer to an lv_templs_t style
 */
lv_templs_t * lv_templs_get(lv_templs_builtin_t style, lv_templs_t * copy);

/**********************
 *      MACROS
 **********************/

#endif

#endif
