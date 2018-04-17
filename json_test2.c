//Copyright (c) 2017 Pratik M Tambe <enthusiasticgeek@gmail.com>
#include "json_parse_test.h"

int main(int argc, char * argv[])
{
    //Step I: Convert to JSON string

    /*Creating a json object*/
    json_object *jobj = json_object_new_object();

    /*Creating a json string*/
    json_object *jstring = json_object_new_string("Joys of Programming");

    /*Creating a json integer*/
    json_object *jint = json_object_new_int(10);

    /*Creating a json boolean*/
    json_object *jboolean = json_object_new_boolean(1);

    /*Creating a json double*/
    json_object *jdouble = json_object_new_double(2.14);

    /*Creating a json array*/
    json_object *jarray = json_object_new_array();

    /*Creating json strings*/
    json_object *jstring1 = json_object_new_string("c");
    json_object *jstring2 = json_object_new_string("c++");
    json_object *jstring3 = json_object_new_string("php");

    /*Adding the above created json strings to the array*/
    json_object_array_add(jarray,jstring1);
    json_object_array_add(jarray,jstring2);
    json_object_array_add(jarray,jstring3);

    /*Form the json object*/
    /*Each of these is like a key value pair*/
    json_object_object_add(jobj,"Site Name", jstring);
    json_object_object_add(jobj,"Technical blog", jboolean);
    json_object_object_add(jobj,"Average posts per day", jdouble);
    json_object_object_add(jobj,"Number of posts", jint);
    json_object_object_add(jobj,"Categories", jarray);

    printf("Size of JSON object- %lu\n", sizeof(jobj));

    unsigned char temp_buff[MAX_SIZE];
    if (memcpy(temp_buff, (unsigned char*)json_object_to_json_string(jobj),MAX_SIZE+1) == NULL)
    {
        perror("strcpy");
        return EXIT_FAILURE;
    }
    //printf("jobj from str:\n---\n%s\n---\n", json_object_to_json_string_ext(jobj, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY));
    //printf("%s\n",temp_buff);

    //free all memory
    json_object_put(jstring);
    json_object_put(jint);
    json_object_put(jboolean);
    json_object_put(jdouble);
    json_object_put(jarray);
    json_object_put(jstring1);
    json_object_put(jstring2);
    json_object_put(jstring3);
    json_object_put(jobj);


    json_object * jobj_parse = json_tokener_parse((char *)temp_buff);
    printf("jobj from str:\n---\n%s\n---\n", json_object_to_json_string_ext(jobj_parse, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY));

    //Step II: Parse JSON string

    int recursion_guard_count=0;
    json_parse(jobj_parse, &recursion_guard_count);

    //Step III: Free memory

    json_object_put(jobj_parse);
    return 0;
}
