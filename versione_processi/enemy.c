#include "enemy.h"

/**
 * Procedura per la gestione dei movimenti e del rilascio di bombe degli alieni.
 * Tutti gli alieni si muovono in formazione allineata, eseguendo dei movimenti verticali
 * e ad intervalli regolari anche orizzontali.
 * @param pipeOut pipe su cui scrivere le informazioni di ogni alieno al gameManager
 * @param id codice identificativo diverso per ogni alieno
 * @param x posizione di spawn sull'asse x
 * @param y posizione di spawn sull'asse y
 * @param randomBomb numero di cicli dopo cui rilasciare una bomba (diverso per ogni alieno)
 * @param alienDelay velocità degli alieni in base alla difficoltà scelta
 */
void alien(int pipeOut, int id, int x, int y, int randomBomb, int alienDelay) {
    objectData alien;
    pid_t bombPid;

    int dx = -MOVEMENT_RANGE * 5, dy = MOVEMENT_RANGE;
    int loopCounter = 0, movCounter = 0;

    /* Setting di alcune informazioni di base dell'alieno */
    alien.coord.x = x;
    alien.coord.y = y;
    alien.object_type = ALIEN;
    alien.object_id = id;
    alien.objectPid = getpid();

    /* Scrittura delle informazioni di base sulla pipe */
    write(pipeOut, &alien, sizeof(alien));

    while(true) {
        /* Rilascio delle bombe ogni randomBomb cicli */
        if(loopCounter % randomBomb == 0 && loopCounter != 0) {
            /* Creazione del processo bomba */
            bombPid = fork();

            if(bombPid == -1) {
                perror("Errore nell'esecuzione della fork");
                _exit(1);
            }
            else if(bombPid == 0) 
                /* Procedura per la generazione delle coordinate della bomba */
                bomb(pipeOut, alien.coord, alien.object_id);
        }   

        /* Movimento dell'alieno verso il basso o verso l'alto */
        if(movCounter < Y_MOVEMENT)
            alien.coord.y += dy;
        else {
            dy *= -1;
            movCounter = 0;
        }

        /* Ogni X_MOVEMENT cicli l'alieno si sposta a sinistra */
        if(loopCounter % X_MOVEMENT == 0 && loopCounter != 0)
            alien.coord.x += dx;
        
        /* Scrittura delle informazioni aggiornate sulla pipe */
        write(pipeOut, &alien, sizeof(alien));

        /* Velocità movimento alieno */
        usleep(alienDelay);

        movCounter++;
        loopCounter++;
    }
}

/**
 * Procedura per la generazione delle coordinate della bomba. La bomba viene rilasciata
 * ad intervalli regolari, gli alieni avranno intervalli diversi tra loro.
 * @param pipeOut pipe su cui il processo bomba scrive le sue informazioni
 * @param alienPos posizione in cui si trova l'alieno che rilascia la bomba
 * @param bombId codice identificativo della bomba, uguale a quello dell'alieno che la rilascia
 */
void bomb(int pipeOut, position alienPos, int bombId) {
    objectData bomb;

    /* Setting di alcune informazioni di base della bomba */
    bomb.objectPid = getpid();
    bomb.coord.x = alienPos.x - 1;
    bomb.coord.y = alienPos.y;
    bomb.object_type = BOMB;
    bomb.object_id = bombId;

    /* Avvio suono per il rilascio della bomba */
    system(ALIEN_BOMB_SOUND);

    /* Scrittura sulla pipe delle informazioni di base della bomba */
    write(pipeOut, &bomb, sizeof(bomb));

    do {
        /* Movimento orizzontale della bomba */
        bomb.coord.x--;

        /* Scrittura della posizione aggiornata sulla pipe */
        write(pipeOut, &bomb, sizeof(bomb));

        /* Velocità bomba */
        usleep(BOMB_DELAY);
    }while(bomb.coord.x > 1);

    /* Terminazione del processo bomba in caso non colpisca il player
       e quindi quando esce dalla finestra di gioco
    */
    _exit(0);
}