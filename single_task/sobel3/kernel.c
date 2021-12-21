#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    float x;
    float y;
    float z;
} Dot;

void P6sobel3(Dot* input, Dot* output, int* n) {
    // printf("sobel3 size=%d\n", *n);
    int size = *n;
    const float kernel[] = {1, 0, -1, 2, 0, -2, 1, 0, -1};
    int radius = 3;
    for (int i = 0; i < size; i++) {
        int x = i % size;
        int y = i / size;
        Dot Gx, Gy;
        for (int x_shift = 0; x_shift < 3; x_shift++)
            for (int y_shift = 0; y_shift < 3; y_shift++) {
                int xs = x + x_shift - 1;
                int ys = y + y_shift - 1;
                if (x == xs && y == ys)
                    continue;
                if (xs < 0 || xs >= size || ys < 0 || ys >= size)
                    continue;
                Dot sample = input[xs + ys * size];
                int offset_x = x_shift + y_shift * radius;
                int offset_y = y_shift + x_shift * radius;
                float conv_x = kernel[offset_x];
                Dot conv4_x = {conv_x, conv_x, conv_x};
                Gx.x += conv4_x.x * sample.x;
                Gx.y += conv4_x.y * sample.y;
                Gx.z += conv4_x.z * sample.z;
                float conv_y = kernel[offset_y];
                Dot conv4_y = {conv_y, conv_y, conv_y};
                Gy.x += conv4_y.x * sample.x;
                Gy.y += conv4_y.y * sample.y;
                Gy.z += conv4_y.z * sample.z;
            }
        Dot color = {hypot(Gx.x, Gy.x), hypot(Gx.y, Gy.y), hypot(Gx.y, Gy.y)};
        Dot minval = {0.0, 0.0, 0.0};
        Dot maxval = {1.0, 1.0, 1.0};
        if (color.x < minval.x) {
            output[i] = minval;
        } else if (color.x > maxval.x) {
            output[i] = maxval;
        } else {
            output[i] = color;
        }
    }
}
