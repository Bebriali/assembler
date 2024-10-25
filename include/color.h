#ifndef COLOR_H
#define COLOR_H

#define COLOR

#define RED_C     "\x1b[31m"
#define GREEN_C   "\x1b[32m"
#define YELLOW_C  "\x1b[33m"
#define BLUE_C    "\x1b[34m"
#define MAGENTA_C "\x1b[35m"
#define CYAN_C    "\x1b[36m"
#define RESET     "\x1b[0m"

    #ifdef COLOR

        #define RED(arg)     RED_C     arg RESET
        #define GREEN(arg)   GREEN_C   arg RESET
        #define YELLOW(arg)  YELLOW_C  arg RESET
        #define BLUE(arg)    BLUE_C    arg RESET
        #define MAGENTA(arg) MAGENTA_C arg RESET
        #define CYAN(arg)    CYAN_C    arg RESET

    #else

        #define RED(arg)     arg
        #define GREEN(arg)   arg
        #define YELLOW(arg)  arg
        #define BLUE(arg)    arg
        #define MAGENTA(arg) arg
        #define CYAN(arg)    arg
    #endif

#endif
