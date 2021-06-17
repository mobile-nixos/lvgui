#ifndef URL_H
#define URL_H

char* url_get_path(const char* url);
char* url_get_param(const char* url, const char* name);
int url_get_int_param(const char* url, const char* name, int* value);

#endif
