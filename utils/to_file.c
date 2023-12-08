#include "stdio.h"

int utils_digits_to_grid(const char *filename, const char *values) {
    FILE *f = fopen(filename, "wb");
    if (f == NULL)
        return -1;

    size_t i;
    size_t j;

    for (i = 0; i < 9; i++) {
        if (i != 0 && i % 3 == 0)
            if (fwrite("\n", sizeof(char), 1, f) != 1)
                goto err;

        for (j = 0; j < 9; j++) {
            if (j != 0 && j % 3 == 0)
                if (fwrite(" ", sizeof(char), 1, f) != 1)
                    goto err;
            if (fwrite(values + i * 9 + j, sizeof(char), 1, f) != 1)
                goto err;
        }
        if (fwrite("\n", sizeof(char), 1, f) != 1)
            goto err;
    }

    fclose(f);
    return 0;

err:
    fclose(f);
    return -1;
}

