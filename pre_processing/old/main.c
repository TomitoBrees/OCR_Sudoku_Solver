#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "err.h"

//#include "SDL2/SDL.h"

static const char *usage = "Usage: <operation> <image file>\n\
    operations:\n";

void tst(int i) {
    printf("tst: %i\n", i);
}

int main(int argc, char **argv) {
    void (*t)(int) = NULL;
    
    t = tst;

    t(5);
    return 0;

    if (argc < 3)
        errx(EXIT_FAILURE, usage);

    //void (*func)(*SDL_Surface) = NULL;

    const char *op = argv[1];
    if (strcmp(op, "") == 0) {}

    if (/*func*/ NULL == NULL) {
        printf("%s: operation not known\n", argv[1]);
        errx(EXIT_FAILURE, usage);
    }
}
