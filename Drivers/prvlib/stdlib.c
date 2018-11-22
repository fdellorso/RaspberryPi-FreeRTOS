/**
 *	Quick and very Dirty STDLIB API.
 *
 **/

#include <prvlib/stdlib.h>
// #include <prvlib/string.h>
#include <uspi/util.h>

#include <FreeRTOS.h>
#include <uspi/synchronize.h>


void *calloc(size_t num, size_t size) {
    uspi_EnterCritical();
    int *p = pvPortMalloc(num*size);
    // while(*p++ = 0);
    memset(p, 0, num*size);
    uspi_LeaveCritical();
    return p;
}

void *realloc(void *ptr, size_t new_size) {
    uspi_EnterCritical();
    vPortFree(ptr);
    ptr = pvPortMalloc(new_size);
    uspi_LeaveCritical();
    return ptr;
}

int abs(int n) {
    if(n >= 0) return n;
    else return -n;
}

int atoi(char * str) {
    int i = 0;
    int res = 0;
   
    if(str[i] == '-') {
        for (i = 1; str[i] != '\0'; ++i) res = res*10 + str[i] - '0';
        res = -res;
    }
    else {
        for (i = 0; str[i] != '\0'; ++i) res = res*10 + str[i] - '0';
    }

    return res; 
} 

//this is not random at all
int next = 1;
int rand(void){return (int)((next = next * 1103515245 + 12345) % ((unsigned long)32767 + 1));}
