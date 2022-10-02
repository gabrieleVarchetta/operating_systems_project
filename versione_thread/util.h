#ifndef UTIL_H_
#define UTIL_H_

#include <curses.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include <math.h>
#include <string.h>

typedef enum{PLAYER, ROCKET, ALIEN, BOMB} ObjectType;

typedef enum{EASY = 1, MEDIUM, HARD, CUSTOM} Difficulty;

typedef enum {PLAYER_COLOR = 1, ALIEN_COLOR, ROCKET_COLOR, BOMB_COLOR, TEXT_COLOR} ObjectColor;

typedef struct {
    int x;
    int y;
}position;

/* Struttura contenente le informazioni su posizione e tipologia di oggetto */
typedef struct {
    position coords;
    ObjectType object_type;
    int object_id;
    bool alive;
    pthread_t objectTid;
    int lives;
}objectData;

typedef struct {
  double x,y,z;
} coord;

typedef struct {
    position pos;
    int id;
}initInfo;

typedef struct {
    char name[25];
    int sec;
    int score;
}Record;

/* Macro utilizzate per migliorare la leggibilità del codice */
#define MOVEMENT_RANGE 1 //dimensione del raggio di spostamento degli oggetti
#define UP_KEY 65 //freccia direzionale 
#define DOWN_KEY 66 //freccia direzionale 
#define SPACE 32 //tasto SPAZIO sulla tastiera 
#define ENTER 10 //tasto INVIO sulla tastiera
#define MAX_X 90 //dimensione finestra asse x
#define MAX_Y 24 //dimensione finestra asse y
#define READ_END 0 //interfaccia lettura pipe
#define WRITE_END 1 //interfaccia scrittura pipe
#define N_ALIEN 1 //numero di alieni
#define MIN_BOMB 10 //limite inferiore generazione bombe
#define MAX_BOMB 150 //limite superiore generazione bombe
#define ROCKET_DELAY 7500 
#define BOMB_DELAY 25000
#define NAME_LEN 8 //lunghezza massima del nome utente
#define ALIEN_SCORE 50 //punteggio ottenuto da ogni alieno colpito
#define ALIEN_LIVES 3 //vite totali dell'alieno
#define PLAYER_LIVES 3 //vite del giocatore
#define X_MOVEMENT 25 //numero di cicli in cui avviene spostamento orizzontale
#define Y_MOVEMENT 5 //numero di spostamenti verticali da fare prima di invertire direzione
#define PLAYER_EXPLOSION_SOUND "aplay ../sounds/player_explode.wav 2> log_pipe.txt &"
#define ALIEN_EXPLOSION_SOUND "aplay ../sounds/enemy_explode.wav 2> log_pipe.txt &"
#define PLAYER_ROCKET_SOUND "aplay ../sounds/player_fire.wav 2> log_pipe.txt &"
#define ALIEN_BOMB_SOUND "aplay ../sounds/enemy_fire.wav 2> log_pipe.txt &"
#define MAIN_SOUNDTRACK "aplay ../sounds/soundtrack.wav 2> ./log_pipe.txt &"
#define INTRO_SOUNDTRACK "aplay ../sounds/intro.wav 2> ./log_pipe.txt &"

#define KILL_SOUND "killall aplay" 

#define NUM_FRAMES 100
#define NUM_RANDOMCOORDS 800
#define PERSPECTIVE 50.0

/* Dimensione degli sprite */
#define PLAYERSPRITE_X 5
#define PLAYERSPRITE_Y 6
#define ALIENSPRITE_X 3
#define ALIENSPRITE_Y 3

/* Dimensione del buffer */
#define DIM_BUFFER 500 

objectData buffer[DIM_BUFFER];
objectData consumedProduct;

bool isGameEnded;

int in, out;
int nAlien, alienDelay;

pthread_mutex_t bufferMutex, printMutex, mainMutex;
sem_t fullSlots, emptySlots;

/* Functions */
void init();
void printTitle();
Difficulty chooseDifficulty();

void consume();
void produce(objectData object);
void clean_buffer();

void setAlienSpawnPos(int *posX, int *posY, int *alienSpacingX, int alienSpacingY, int i);

double prng();
void kaboom();

void selectionSort(Record *array, int size);
int compare(Record r1, Record r2);
void swap(Record *r1, Record *r2);

FILE* openFile(char *fileName, char *mode);
FILE* closeFile(FILE *fp);

#endif