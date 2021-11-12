#include "type.h"

const char *type_to_readable(type_t type)
{
    switch(type) {
    case TYPE_INTEGER:
        return "integer";
    case TYPE_NUMBER:
        return "number";
    case TYPE_STRING:
        return "string";
    case TYPE_BOOL:
        return "boolean";
    case TYPE_NIL:
        return "nil";
    }
    return "unknown-type";
}
