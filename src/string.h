#ifndef STRING_HEADER_H
#define STRING_HEADER_H

typedef struct String
{
 char *val;
 int length;
} String;

void string_create(String** str);

void string_destroy(String** str);

const char *string_get(String* str);

int cstring_length(const char *cstr);

String *string_from_cstring(const char* cstr);

String *string_concat_cstring(int num, ...);

#endif // STRING_HEADER_H
