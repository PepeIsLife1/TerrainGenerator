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

#define MAXWIDTH 1000
#define MAXLENGTH 1000
#define MAXTHREADS 4
#define KEY 6767

int MATRIX[MAXWIDTH][MAXLENGTH];
int p[512];

typedef struct {
    int id;
} thread_args;

//functions 
float noiseGenerator(float x, float y);
void *generazioneTerreno(void *arg);


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

//---------------------------------------------------main
int main() {
    pthread_t threads[MAXTHREADS];
    thread_args t_args[MAXTHREADS];
    FILE *pf;

    srand(KEY);

    for (int i = 0; i < 256; i++) p[i] = i;
    for (int i = 0; i < 256; i++) {
        int j = rand() % 256;
        int tmp = p[i]; p[i] = p[j]; p[j] = tmp;
        p[i + 256] = p[i];
    }
//----------------------------------------------------

//------------------------------------------------thread creation
    for (int i = 0; i < MAXTHREADS; i++) {
        t_args[i].id = i;
        pthread_create(&threads[i], NULL, terrainGeneration, &t_args[i]);
    }

    for (int i = 0; i < MAXTHREADS; i++) {
        pthread_join(threads[i], NULL);
    }
//----------------------------------------------

//------------------------------------------------file
    pf = fopen("mappa.txt", "wb");
    if (pf == NULL) {
        perror("errore apertura file");
        return 1;
    }

    printf("mappa generata:\n");
    for (int i = 0; i < MAXWIDTH; i++) {
        for (int j = 0; j < MAXLENGTH; j++) {
            printf("%d ", MATRIX[i][j]);
            fwrite(&MATRIX[i][j], sizeof(int), 1, pf);
        }
        printf("\n");
    }

    fclose(pf);
    printf("file salvato con successo\n");
    return 0;
}

//---------------------------------------------------

float noiseGenerator(float x, float y) {
    int X = (int)floor(x) & 255;
    int Y = (int)floor(y) & 255;
    x -= floor(x); y -= floor(y);
    float u = fade(x), v = fade(y);
    
    int A = (p[X] + Y) & 511;
    int B = (p[X + 1] + Y) & 511;

    return lerp(v, lerp(u, grad(p[A], x, y), grad(p[B], x-1, y)),
                   lerp(u, grad(p[A+1], x, y-1), grad(p[B+1], x-1, y-1)));
}


//--------------------------------------thread function
void *terrainGeneration(void *arg) {
    thread_args *data = (thread_args *)arg;
    int id = data->id;
    
    int rows_per_thread = MAXWIDTH / MAXTHREADS;
    int start_row = id * rows_per_thread;
    int end_row = (id == MAXTHREADS - 1) ? MAXWIDTH : (id + 1) * rows_per_thread;

    float scala = 0.1f;

    for (int i = start_row; i < end_row; i++) {
        for (int j = 0; j < MAXLENGTH; j++) {
            float noise = perlin2d(i * scala, j * scala);
            float val = (noise + 1.0f) / 2.0f;

            if (val < 0.10f)      MATRIX[i][j] = 0;
            else if (val < 0.20f) MATRIX[i][j] = 1;
            else if (val < 0.30f) MATRIX[i][j] = 2;
            else if (val < 0.40f) MATRIX[i][j] = 3;
            else if (val < 0.50f) MATRIX[i][j] = 4;
            else if (val < 0.60f) MATRIX[i][j] = 5;
            else if (val < 0.70f) MATRIX[i][j] = 6;
            else if (val < 0.80f) MATRIX[i][j] = 7;
            else if (val < 0.90f) MATRIX[i][j] = 8;
            else if (val < 1.00f) MATRIX[i][j] = 9;
        }
    }
    pthread_exit(NULL);
}