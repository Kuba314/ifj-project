#pragma once

typedef enum
{
    TYPE_INTEGER,
    TYPE_NUMBER,
    TYPE_STRING,
    TYPE_BOOL,
    TYPE_NIL
} type_t;

const char *type_to_readable(type_t type);
