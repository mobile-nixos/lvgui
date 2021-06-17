#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * Given a string, returns a copy up to and excluding the first '?', or the end.
 */
char* url_get_path(char* url)
{
	if (url == NULL) {
		return NULL;
	}

	char* end = strchr(url, '?');

	if (end == NULL) {
		end = url + strlen(url);
	}

	unsigned int length = end - url;
	char* ret = calloc(length+1, 1);
	strncpy(ret, url, length);

	return ret;
}

/**
 * Simplistic "URL parameter" parsing.
 * Only returns the first found parameter.
 * Currently does not parse percent-encoded values.
 *
 * Returns:
 *  - newly allocated string with value for parameter when found
 *  - NULL when the parameter is not found
 *
 * Consumer must free.
 */
char* url_get_param(const char* url, const char* name)
{
	// No valid inputs?
	if (url == NULL || name == NULL || strlen(name) == 0) {
		return NULL;
	}

	// Find the URL parameters; which here is assumed to be the first
	// instance of '?'
	char* start = strchr(url, '?');

	// No URL params? No params!
	if (start == NULL) {
		return NULL;
	}

	// Parameters start *after* the question mark.
	start += 1;

	char* needle = calloc(strlen(name)+2, 1);
	strcat(needle, name);
	strcat(needle, "=");
	unsigned int needle_skip = strlen(needle);

	char* candidate = start;
	char* end = NULL;

	// Search forward for the needle
	while((candidate = strstr(candidate, needle)) != NULL) {
		// Either first param, or after a separator (&)
		if (candidate == start || *(candidate-1) == '&') {
			// Skip the needle in the result
			candidate += needle_skip;

			// Get to the next separator
			end = strchr(candidate, '&');
			// Oh, or maybe we're at the end!
			if (end == NULL) {
				end = candidate+strlen(candidate);
			}
			// Copy the string
			char* ret = calloc(end-candidate+1, 1);
			strncpy(ret, candidate, end-candidate);

			return ret;
		}

		// Skip further than the needle we found.
		candidate += needle_skip;
	}

	// No candidate found :(
	return NULL;
}

int url_get_int_param(const char* url, const char* name, int* value)
{
	char* result = url_get_param(url, name);

	if (result == NULL) {
		return 0;
	}

	*value = atoi(result);

	free(result);

	return 1;
}

/*
// clear; gcc -Werror -Wall -Wpedantic url.c -o url && ./url
int main() {

	char* path = NULL;

	path = url_get_path("aaa.svg");
	printf("→ %s\n", path);
	free(path);

	path = url_get_path("aaa.svg?width=10");
	printf("→ %s\n", path);
	free(path);

	path = url_get_path("?hi=hello");
	printf("→ %s\n", path);
	free(path);

	path = url_get_path("");
	printf("→ %s\n", path);
	free(path);

	path = url_get_path(NULL);
	printf("→ %s\n", path);
	free(path);

	printf("\n----\n");

	printf(
		"→ %s\n",
		url_get_param("aaa.svg", NULL)
	);
	printf(
		"→ %s\n",
		url_get_param(NULL, "bb")
	);
	printf(
		"→ %s\n",
		url_get_param("aaa.svg?bb=cdefg&hijk=lmnop", "")
	);
	printf(
		"→ %s\n",
		url_get_param("", "bb")
	);
	printf(
		"→ %s\n",
		url_get_param("bb.svg", "bb")
	);

	printf(
		"→ %s\n\n",
		url_get_param("aaa.svg?bbbbb=nope", "bb")
	);

	printf(
		"→ %s\n\n",
		url_get_param("aaa.svg?bb", "bb")
	);

	printf("----\n");

	printf(
		"→ %s\n\n",
		url_get_param("aaa.svg?bb=", "bb")
	);

	printf("----\n");

	printf(
		"→ %s\n\n",
		url_get_param("aaa.svg?hijk=lmnopbb=nope&bb=okay", "bb")
	);

	printf(
		"→ %s\n\n",
		url_get_param("aaa.svg?bb=okay&hijk=lmnop&bb=nope", "bb")
	);

	printf(
		"→ %s\n\n",
		url_get_param("aaa.svg?hijk=lmnop&bb=okay&", "bb")
	);

	printf(
		"→ %s\n\n",
		url_get_param("aaa.svg?bbbbb=nope&bb=okay", "bb")
	);

	int tmp = 0;
	int res = 0;

	res = url_get_int_param("aaa.svg", "height", &tmp);                           printf("val: %d res: %d;\n", tmp, res);
	res = url_get_int_param("aaa.svg?aaaaaaaaaaaaaaaaaaaaaa", "height", &tmp);    printf("val: %d res: %d;\n", tmp, res);
	res = url_get_int_param("aaa.svg?width=1234&height=5678", "width",  &tmp);    printf("val: %d res: %d;\n", tmp, res);
	res = url_get_int_param("aaa.svg?width=1234&height=5678", "height", &tmp);    printf("val: %d res: %d;\n", tmp, res);
	res = url_get_int_param("aaa.svg?aaaaaaaaaaaaaaaaaaaaaa", "height", &tmp);    printf("val: %d res: %d;\n", tmp, res);
	res = url_get_int_param("aaa.svg?width=aaaa&height=bbbb", "height", &tmp);    printf("val: %d res: %d;\n", tmp, res);
}

// */
