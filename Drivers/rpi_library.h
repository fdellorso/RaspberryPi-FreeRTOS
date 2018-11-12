#ifndef _RPI_LIBRARY_H_
#define _RPI_LIBRARY_H_

#ifndef PRVLIB
    #include <assert.h>
    #include <stdarg.h>
    #include <stdbool.h>
    #include <stddef.h>
    #include <stdint.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
#else
    #include <uspi/assert.h>
    #include <uspi/stdarg.h>
    #include <prvlib/stdbool.h>
    #include <prvlib/stddef.h>
    #include <prvlib/stdint.h>
    #include <prvlib/stdio.h>
    #include <prvlib/stdlib.h>
    // #include <prvlib/string.h>
    #include <uspi/util.h>
#endif

#endif
