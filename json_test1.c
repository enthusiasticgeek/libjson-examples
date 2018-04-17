//Copyright (c) 2017 Pratik M Tambe <enthusiasticgeek@gmail.com>
#include "json_parse_test.h"

int main(int argc, char * argv[])
{
    unsigned char temp_buff[MAX_SIZE] =

        "{" \
        "  \"firstName\": \"John\"," \
        "  \"lastName\": \"Smith\"," \
        "  \"isAlive\": true," \
        "  \"age\": 27," \
        "  \"address\": {" \
        "    \"streetAddress\": \"21 2nd Street\"," \
        "    \"city\": \"New York\"," \
        "    \"state\": \"NY\"," \
        "    \"postalCode\": \"10021-3100\"" \
        "  }," \
        "  \"phoneNumbers\": [" \
        "    {" \
        "      \"type\": \"home\"," \
        "      \"number\": \"212 555-1234\"" \
        "    }," \
        "    {" \
        "      \"type\": \"office\"," \
        "      \"number\": \"646 555-4567\"" \
        "    }," \
        "    {" \
        "      \"type\": \"mobile\"," \
        "      \"number\": \"123 456-7890\"" \
        "    }" \
        "  ]," \
        "  \"children\": []," \
        "  \"spouse\": null" \
        "}";

    json_object * jobj_parse = json_tokener_parse((char*)temp_buff);
    printf("jobj from str:\n---\n%s\n---\n", json_object_to_json_string_ext(jobj_parse, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY));

    //Step I: Parse JSON string
    int recursion_guard_count=0;
    json_parse(jobj_parse, &recursion_guard_count);
    //Step II: Free Memory
    json_object_put(jobj_parse);
    return 0;
}
