//Copyright (c) 2018 Pratik M Tambe <enthusiasticgeek@gmail.com>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <json/json.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAX_SIZE 1024

int main()
{
    /* all previous code until
    printf("Size of string- %lu\n", sizeof(json_object_to_json_string(jobj)))*/

    int fd = 0;
    struct sockaddr_in demoserverAddr;

    fd = socket(AF_INET, SOCK_STREAM, 0);

    if (fd < 0)
    {
        printf("Error : Could not create socket\n");
        return 1;
    }
    else
    {
        demoserverAddr.sin_family = AF_INET;
        demoserverAddr.sin_port = htons(8888);
        demoserverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
        //demoserverAddr.sin_addr.s_addr = inet_addr("10.200.242.230");
        memset(demoserverAddr.sin_zero, '\0', sizeof(demoserverAddr.sin_zero));
    }

    if (connect(fd, (const struct sockaddr *)&demoserverAddr, sizeof(demoserverAddr)) < 0)
    {
        printf("ERROR connecting to server\n");
        return 1;
    }

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

    char temp_buff[MAX_SIZE];

    if (strcpy(temp_buff, json_object_to_json_string(jobj)) == NULL)
    {
        perror("strcpy");
        return EXIT_FAILURE;
    }

//Alternatively use the following.
/*
    char temp_buff[MAX_SIZE] =

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
*/


    if (write(fd, temp_buff, strlen(temp_buff)) == -1)
    {
        perror("write");
        return EXIT_FAILURE;
    }

    printf("Written data\n");
    return EXIT_SUCCESS;
}
