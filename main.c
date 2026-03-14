/*TERRAIN GENERATOR PROGRAM
This program is made to be exported in my upcoming game
this is being tested and made better day by day.
I am using multi-threading for better performance for larger maps.
I will pair it with another program to load the terrain near the coordinates given.
If you end up using my program please let me know, just for fun.
If it is bad documented tell me, i am not the best documenter.
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <string.h>

#define MAXWIDTH 20000
#define MAXLENGTH 20000
#define MAXTHREADS 2
#define KEY 6767
#define VIEW_DISTANCE 50

int MATRIX[MAXWIDTH][MAXLENGTH];
int p[512];

typedef struct {
    int start_x, end_x;
    int start_y, end_y;
} thread_range;

//noise functions
float fade(float t) { return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f); }
float lerp(float t, float a, float b) { return a + t * (b - a); }
float grad(int hash, float x, float y) {
    switch(hash & 3) {
        case 0: return x + y;
        case 1: return -x + y;
        case 2: return x - y;
        case 3: return -x - y;
        default: return 0;
    }
}

//functions
float noiseGenerator(float x, float y);
void *terrainGeneration(void *arg);


//---------------------------------------------------main
int main() {
    pthread_t threads[MAXTHREADS];
    thread_range t_args[MAXTHREADS];
    int player_x = VIEW_DISTANCE / 2;
    int player_y = VIEW_DISTANCE / 2;

    int loaded_x_min = -1, loaded_x_max = -1;
    int loaded_y_min = -1, loaded_y_max = -1;

    char input_str[50];

    srand(KEY);
    for (int i = 0; i < 256; i++) p[i] = i;
    for (int i = 0; i < 256; i++) {
        int j = rand() % 256;
        int tmp = p[i]; p[i] = p[j]; p[j] = tmp;
        p[i + 256] = p[i];
    }

    while (1) {
        printf("\ninserisci coordinate oppure digita exit per uscire\n");
        
        if (fgets(input_str, sizeof(input_str), stdin) == NULL) break;

        input_str[strcspn(input_str, "\n")] = 0;

        if (strcmp(input_str, "exit") == 0) {
            break;
        }

        int input_x, input_y;
        if (sscanf(input_str, "%d %d", &input_x, &input_y) != 2) {
            printf("\nusa delle coordinate numeri\n");
            continue;
        }

        player_x += input_x;
        player_y += input_y;

        int x_min = (player_x - VIEW_DISTANCE < 0) ? 0 : player_x - VIEW_DISTANCE;
        int x_max = (player_x + VIEW_DISTANCE >= MAXWIDTH) ? MAXWIDTH : player_x + VIEW_DISTANCE;
        int y_min = (player_y - VIEW_DISTANCE < 0) ? 0 : player_y - VIEW_DISTANCE;
        int y_max = (player_y + VIEW_DISTANCE >= MAXLENGTH) ? MAXLENGTH : player_y + VIEW_DISTANCE;

        if (loaded_x_min == -1 || x_min < loaded_x_min || x_max > loaded_x_max || y_min < loaded_y_min || y_max > loaded_y_max) {
            
            int total_width = x_max - x_min;
            int chunk_size = total_width / MAXTHREADS;

            for (int i = 0; i < MAXTHREADS; i++) {
                t_args[i].start_x = x_min + (i * chunk_size);
                t_args[i].end_x = (i == MAXTHREADS - 1) ? x_max : t_args[i].start_x + chunk_size;
                t_args[i].start_y = y_min;
                t_args[i].end_y = y_max;
                pthread_create(&threads[i], NULL, terrainGeneration, &t_args[i]);
            }

            for (int i = 0; i < MAXTHREADS; i++) {
                pthread_join(threads[i], NULL);
            }

            loaded_x_min = x_min;
            loaded_x_max = x_max;
            loaded_y_min = y_min;
            loaded_y_max = y_max;

            printf("\nmappa attorno al giocatore (%d - %d):\n", player_x, player_y);


            for (int i = x_min; i < x_max; i++) {
                for (int j = y_min; j < y_max; j++) {
                    if (i == player_x && j == player_y) printf("P ");
                    else printf("%d ", MATRIX[i][j]);
                }
                printf("\n");
            }


        }


    }

    return 0;
}
//---------------------------------------------------------------


//--------------------------------------------------noise generator
float noiseGenerator(float x, float y) {
    int X = (int)floor(x) & 255;

    int Y = (int)floor(y) & 255;

    x -= floor(x); y -= floor(y);

    float u = fade(x), v = fade(y);

    int A = (p[X] + Y) & 511;

    int B = (p[X + 1] + Y) & 511;

    return lerp(v, lerp(u, grad(p[A], x, y), grad(p[B], x-1, y)),lerp(u, grad(p[A+1], x, y-1), grad(p[B+1], x-1, y-1)));
}
//-------------------------------------------------------------


//----------------------------------------------------terrain generator
void *terrainGeneration(void *arg) {
    thread_range *range = (thread_range *)arg;
    float scala = 0.1f;

    for (int i = range->start_x; i < range->end_x; i++) {

        for (int j = range->start_y; j < range->end_y; j++) {
            
            float noise = noiseGenerator(i * scala, j * scala);
            float val = (noise + 1.0f) / 2.0f;

            if (val < 0.25f)      MATRIX[i][j] = 0;
            else if (val < 0.3f) MATRIX[i][j] = 1;
            else if (val < 0.4f) MATRIX[i][j] = 2;
            else if (val < 0.5f) MATRIX[i][j] = 3;
            else if (val < 0.6f) MATRIX[i][j] = 4;
            else if (val < 0.7f) MATRIX[i][j] = 5;
            else if (val < 0.8f) MATRIX[i][j] = 6;
            else if (val < 0.9f) MATRIX[i][j] = 7;
            else if (val < 0.95f) MATRIX[i][j] = 8;
            else                  MATRIX[i][j] = 9;
        }
    }
    pthread_exit(NULL);
}
//---------------------------------------------------------