//Copyright (c) 2017 Pratik M Tambe <enthusiasticgeek@gmail.com>
#ifndef _JSON_PARSE_
#define _JSON_PARSE_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <json/json.h>
#include <sys/types.h>

#define MAX_SIZE 1024

//Guard against infinite recursion
#define MAX_RECURSION_COUNT 50
//#define JSON_TOKENER_DEFAULT_DEPTH 32

void recursion_guard_increment(int *count);
void json_parse(json_object * jobj, int *recursion_guard_count);

void recursion_guard_increment(int *count)
{
    int tmp = *count;
    tmp++;
    *count = tmp;
}

void json_parse(json_object * jobj, int *recursion_guard_count) {
    recursion_guard_increment(recursion_guard_count);
    if (*recursion_guard_count > MAX_RECURSION_COUNT) {
        printf("==== MAX RECURSION LIMIT HIT ==== \n");
        return;
    }
    enum json_type type;
    json_object_object_foreach(jobj, key, val) {
        type = json_object_get_type(val);
        switch (type) {
        case json_type_null:
        {
            printf("type: json_type_null, \n");
            json_object * tmp_null;
            json_object_object_get_ex(jobj, key, &tmp_null);
            printf("key: %s\n",key);
            printf("value: %s\n",json_object_get_string(tmp_null));
            break;
        }
        case json_type_object:
        {
            printf("type: json_type_object, \n");
            json_object * tmp_object;
            json_object_object_get_ex(jobj, key, &tmp_object);
            json_parse(tmp_object, recursion_guard_count);
            break;
        }
        case json_type_array:
        {
            printf("type: json_type_array, \n");
            json_object * tmp_array;
            json_object_object_get_ex(jobj, key, &tmp_array);
            printf("key: %s\n",key);
            int arraylen = json_object_array_length(tmp_array);
            printf("Array Length: %d\n",arraylen);
            int i;
            json_object * jvalue;
            for (i=0; i< arraylen; i++) {
                jvalue = json_object_array_get_idx(tmp_array, i);
                printf("value[%d]: %s\n",i, json_object_get_string(jvalue));
                enum json_type type0 = json_object_get_type(jvalue);
                switch(type0) {
                case json_type_object:
                case json_type_array:
                {
                    json_parse(jvalue, recursion_guard_count);
                    break;
                }
                default:
                {}
                }
            }
            break;
        }
        case json_type_string:
        {
            printf("type: json_type_string, \n");
            json_object * tmp_string;
            json_object_object_get_ex(jobj, key, &tmp_string);
            printf("key: %s\n",key);
            printf("value: %s\n",json_object_get_string(tmp_string));
            break;
        }
        case json_type_boolean:
        {
            printf("type: json_type_boolean, \n");
            json_object * tmp_boolean;
            json_object_object_get_ex(jobj, key, &tmp_boolean);
            printf("key: %s\n",key);
            printf("value: %s\n",json_object_get_boolean(tmp_boolean)? " true " : " false ");
            break;
        }
        case json_type_int:
        {
            printf("type: json_type_int, \n");
            json_object * tmp_int;
            json_object_object_get_ex(jobj, key, &tmp_int);
            printf("key: %s\n",key);
            printf("value: %d\n",json_object_get_int(tmp_int));
            break;
        }
        case json_type_double:
        {
            printf("type: json_type_double, \n");
            json_object * tmp_double;
            json_object_object_get_ex(jobj, key, &tmp_double);
            printf("key: %s\n",key);
            printf("value: %f\n",json_object_get_double(tmp_double));
            break;
        }
        default:
        {
            printf("type: default, \n");
        }
        }
    }
}

#endif
