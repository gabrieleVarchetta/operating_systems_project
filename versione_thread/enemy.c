#include "enemy.h"

/**
 * Funzione per la gestione dei movimenti e del rilascio di bombe degli alieni.
 * Tutti gli alieni si muovono in formazione allineata, eseguendo dei movimenti verticali
 * e ad intervalli regolari anche orizzontali. 
 * @param args struttura contenente la posizione iniziale dell'alieno
 * @return void* 
 */
void* alienFunct(void *args) {
    objectData alien;
    pthread_t bombThread;

    int dx = -MOVEMENT_RANGE * 5, dy = MOVEMENT_RANGE;
    int movCounter = 0, loopCounter = 0;

    /* Casting dei parametri passati al thread */
    initInfo* alienInfo = (initInfo*) args;

    pthread_mutex_lock(&mainMutex);
        int randomBomb = rand() % (MAX_BOMB - MIN_BOMB + 1) + MIN_BOMB;
    pthread_mutex_unlock(&mainMutex);
    
    /* Sezione critica per l'accesso a dati condivisi */
    pthread_mutex_lock(&mainMutex);
        /* Setting di alcune informazioni di base dell'alieno */
        alien.coords = alienInfo->pos;
        alien.object_type = ALIEN;
        alien.object_id = alienInfo->id;
        alien.objectTid = pthread_self();
    pthread_mutex_unlock(&mainMutex);
    
    /* Inserimento delle informazioni di base nel buffer */
    produce(alien);

    while(!isGameEnded) {
        /* Rilascio delle bombe ogni randomBomb cicli */
        if(loopCounter % randomBomb == 0 && loopCounter != 0) {
            pthread_mutex_lock(&mainMutex);
                /* Creazione e allocazione della memoria per i parametri
                da passare al thread bomba
                */
                initInfo* bombInfo = malloc(sizeof(initInfo));
                bombInfo->pos = alien.coords;
                bombInfo->id = alien.object_id;
            pthread_mutex_unlock(&mainMutex);

            /* Creazione del thread bomba */
            if(pthread_create(&bombThread, NULL, &bomb, bombInfo) != 0) {
                perror("Error creating the thread");
                exit(1);
            }
            
        }

        /* Movimento dell'alieno verso il basso o verso l'alto */
        if(movCounter < Y_MOVEMENT) {
            pthread_mutex_lock(&mainMutex);
                alien.coords.y += dy;
            pthread_mutex_unlock(&mainMutex);
        }
        else {
            dy *= -1;
            movCounter = 0;
        }

        /* Ogni X_MOVEMENT cicli l'alieno si sposta a sinistra */
        if(loopCounter % X_MOVEMENT == 0 && loopCounter != 0) {
            pthread_mutex_lock(&mainMutex);
                alien.coords.x += dx;
            pthread_mutex_unlock(&mainMutex);
        }

        /* Inserimento nel buffer delle informazioni aggiornate */
        produce(alien);

        /* Velocità movimento alieno */
        usleep(alienDelay);

        pthread_mutex_lock(&mainMutex);
            movCounter++;
            loopCounter++;
        pthread_mutex_unlock(&mainMutex);
    }
    
    /* Rilascio della memoria allocata per il passaggio dei parametri */
    free(args);

    return NULL;
}

/**
 * Procedura per la generazione delle coordinate della bomba. La bomba viene rilasciata
 * ad intervalli regolari, gli alieni avranno intervalli diversi tra loro.
 * @param args struttura contenente le informazioni iniziali della bomba
 * @return void* 
 */
void* bomb(void* args) {
    objectData bomb;

    /* Casting dei parametri passati al thread */
    initInfo* bombData = (initInfo*) args;

    /* Sezione critica per l'accesso a dati condivisi */
    pthread_mutex_lock(&mainMutex);
        /* Setting di alcune informazioni di base della bomba */
        bomb.coords.x = bombData->pos.x;
        bomb.coords.y = bombData->pos.y + 1;
        bomb.object_type = BOMB;
        bomb.objectTid = pthread_self();
        bomb.object_id = bombData->id;
    pthread_mutex_unlock(&mainMutex);

    /* Avvio suono per il rilascio della bomba */
    system(ALIEN_BOMB_SOUND);
    
    /* Scrittura nel buffer delle informazioni di base della bomba */
    produce(bomb);

    do {
        pthread_mutex_lock(&mainMutex);
            /* Movimento orizzontale della bomba */
            bomb.coords.x--;
        pthread_mutex_unlock(&mainMutex);

        /* Scrittura della posizione aggiornata nel buffer */
        produce(bomb);

        /* Velocità bomba */
        usleep(BOMB_DELAY);
    }while(bomb.coords.x > 1 && !isGameEnded);

    /* Rilascio della memoria allocata per il passaggio dei parametri */
    free(args);

    /* Terminazione del processo bomba in caso non colpisca il player
       e quindi quando esce dalla finestra di gioco
    */
    return NULL;
}