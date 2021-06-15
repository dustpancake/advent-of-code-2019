#include <stdlib.h>
#include <math.h>
#include <inputio.h>

int convert(int value) {
    return (int) floor(value / 3) - 2;
}

int convert_part_two(int value) {
    int fuel = 0;

    do {
        value = convert(value);
        if (value < 0) {
            value = 0;
        }
        fuel += value;

    } while (value > 0);

    return fuel;
}


int main() {
    int value;
    unsigned long sum = 0;
    unsigned long sum2 = 0;

    GPtrArray *data = f_read_lines("./input.txt");
    // check if error
    if (data == NULL) {
        return 1;
    }
    fprintf(stdout, "Read in %d values.\n", data->len);

    for (int i = 0; i < data->len; ++i) {
        value = (int) strtol(data->pdata[i], NULL, 10);
        sum += convert(value);
        sum2 += convert_part_two(value);
    }

    fprintf(stdout, "Sum Part 1 = %ld\n", sum);

    fprintf(stdout, "Sum Part 2 = %ld\n", sum2);

    g_ptr_array_free(data, TRUE);   
    return 0;
}
