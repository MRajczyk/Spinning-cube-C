#include <curses.h>
#include <windows.h>
#include <math.h>
#include <stdint.h>

#define CUBE_WIDTH 20
#define PROJECTION_WIDTH 160
#define PROJECTION_HEIGHT 44

float A = 0;
float B = 0;
float C = 0;

float coordThreshold = 0.5;
int distanceFromCamera = 100;
int scalingFactor = 40;

char zBuffer[PROJECTION_WIDTH * PROJECTION_HEIGHT] = {' '};
float zBufferValues[PROJECTION_WIDTH * PROJECTION_HEIGHT] = {0};

float calculateXcoord(float x, float y, float z) {
    return y * sin(A) * sin(B) * cos(C) - z * cos(A) * sin(B) * cos(C) +
         y * cos(A) * sin(C) + z * sin(A) * sin(C) + x * cos(B) * cos(C);
}

float calculateYcoord(float x, float y, float z) {
    return y * cos(A) * cos(C) + z * sin(A) * cos(C) -
         y * sin(A) * sin(B) * sin(C) + z * cos(A) * sin(B) * sin(C) -
         x * cos(B) * sin(C);
}

float calculateZcoord(float x, float y, float z) {
    return z * cos(A) * cos(B) - y * sin(A) * cos(B) + x * sin(B);
}

void calculateZbufferForPoint(float x, float y, float z, char charToPrint) {
    float x_updated = calculateXcoord(x, y, z);
    float y_updated = calculateYcoord(x, y, z);
    float z_updated = calculateZcoord(x, y, z) + distanceFromCamera;

    float z_inversed = 1 / z_updated;

    int projectionX = (int)(PROJECTION_WIDTH / 2 + scalingFactor * z_inversed * x_updated * 2);
    int projectionY = (int)(PROJECTION_HEIGHT / 2 + scalingFactor * z_inversed * y_updated);
    int idx = projectionX + projectionY * PROJECTION_WIDTH;

    if (idx >= 0 && idx < PROJECTION_WIDTH * PROJECTION_HEIGHT) {
        if (z_inversed > zBufferValues[idx]) {
            zBufferValues[idx] = z_inversed;
            zBuffer[idx] = charToPrint;
        }
    }
}

int main() {
    initscr();

    while (1) {
        memset(zBuffer, ' ', PROJECTION_WIDTH * PROJECTION_HEIGHT);
        memset(zBufferValues, 0, PROJECTION_WIDTH * PROJECTION_HEIGHT * sizeof(float));

        for(int i = 0; i < PROJECTION_HEIGHT; ++i) {
            for(int j = 0; j < PROJECTION_WIDTH; ++j) {
                mvprintw(i, j, "%c", zBuffer[j + i * PROJECTION_WIDTH]);
            }
        }
        refresh();

        for (float cubeX = -CUBE_WIDTH; cubeX < CUBE_WIDTH; cubeX += coordThreshold) {
            for (float cubeY = -CUBE_WIDTH; cubeY < CUBE_WIDTH; cubeY += coordThreshold) {
                calculateZbufferForPoint(cubeX, cubeY, -CUBE_WIDTH, '#');
                calculateZbufferForPoint(CUBE_WIDTH, cubeY, cubeX, '<');
                calculateZbufferForPoint(-CUBE_WIDTH, cubeY, -cubeX, '~');
                calculateZbufferForPoint(-cubeX, cubeY, CUBE_WIDTH, '%');
                calculateZbufferForPoint(cubeX, -CUBE_WIDTH, -cubeY, ';');
                calculateZbufferForPoint(cubeX, CUBE_WIDTH, cubeY, '+');
            }
        }

        for(int i = 0; i < PROJECTION_HEIGHT; ++i) {
            for(int j = 0; j < PROJECTION_WIDTH; ++j) {
                mvprintw(i, j, "%c", zBuffer[j + i * PROJECTION_WIDTH]);
            }
        }
        refresh();

        A += 0.01;
        B += 0.02;
        // C += 0.02;
        Sleep(10);
    }

    getch();
    endwin();
    return 0;
}

//gcc main.c -o main.exe -lpdcurses