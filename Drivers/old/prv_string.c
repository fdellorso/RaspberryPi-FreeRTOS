/**
 *	Quick and very Dirty STRING API.
 *
 **/

#include "prvlib/prv_string.h"

char *strcat(char string1[], char string2[]) {
    unsigned char i, j;

    // calculate the length of string s1
    // and store it in i
    for(i = 0; string1[i] != '\0'; ++i);

    for(j = 0; string2[j] != '\0'; ++j, ++i)
    {
        string1[i] = string2[j];
    }

    string1[i] = '\0';

    return string1;
}

char *strcpy(char string1[], char string2[]) {
    unsigned char i;
 
    while (string1[i] != '\0') {
        string2[i] = string1[i];
        i++;
    }
 
    string2[i] = '\0';

    return string2;
}