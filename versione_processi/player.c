#include "player.h"

/**
 * Procedura per la gestione dei movimenti e dei missili del giocatore.
 * @param pipeOut pipe su cui scrivere le informazioni relative il giocatore al gameManager
 */
void mainPlayer(int pipeOut) {
    objectData player;

    struct timeval start, end;
    bool canShoot = false;
    int fireDelay, minDelay = ROCKET_DELAY * MAX_X; //minDelay è il tempo che impiega il missile ad arrivare a destra

    /* Setting di alcuni valore di base del player */
    player.coord.x = 1;
    player.coord.y = MAX_Y / 2;
    player.object_type = PLAYER;
    player.objectPid = getpid();

    /* Per il primo ciclo si imposta il tempo trascorso dall'ultimo sparo uguale a minDelay + 1 */
    fireDelay = minDelay + 1; 

    /* Scrittura delle prime informazioni di base sulla pipe */
    write(pipeOut, &player, sizeof(player));

    while (true) {
        /* Movimento della navicella e lancio dei missili */
        char keyPressed;

        switch (keyPressed = getch()) {
        case UP_KEY:
            if (player.coord.y > 2)
                player.coord.y -= MOVEMENT_RANGE;

            break;
        
        case DOWN_KEY:
            if (player.coord.y <= MAX_Y - PLAYERSPRITE_Y)
                player.coord.y += MOVEMENT_RANGE;

            break;
        
        case SPACE:
            /* Calcolo il tempo trascorso dall'ultimo lancio di missili */
            if(canShoot) {
                /* Terminazione del contatore per il tempo trascorso */
                gettimeofday(&end, NULL);
                fireDelay = ((end.tv_sec - start.tv_sec) * 1000000) + (end.tv_usec - start.tv_usec);
            }

            /* Se il tempo trascorso è maggiore del delay prefissato posso sparare di nuovo */
            if(fireDelay > minDelay) {
                /* Procedura per la generazione dei missili */
                rockets(pipeOut, player.coord.y, player.coord.x);
                
                /* Avvio del contatore per il tempo trascorso */
                gettimeofday(&start, NULL);
                canShoot = true;
            }
            break;

        default:
            break;
        }

        /* Scrittura sulla pipe di tutte le nuove informazioni del player */
        write(pipeOut, &player, sizeof(player));
    }
}

void rockets(int pipeOut, int pos_y, int pos_x) {
    pid_t rocketUpPid, rocketDownPid;
    objectData rocketUp, rocketDown;
    int loopCounter = 0, loopCounter1 = 0;

    /* Impostazione delle informazioni base dei missili */
    rocketUp.object_type = ROCKET;
    rocketUp.object_id = 0;

    rocketDown.object_type = ROCKET;
    rocketDown.object_id = 1;

    /* Avvio suono per il lancio dei missili */
    system(PLAYER_ROCKET_SOUND);

    /* Creazione processi missili e generazione coordinate */
    rocketUpPid = fork();

    if(rocketUpPid == 0) {
        /* Posizione da cui parte il missile */
        rocketUp.coord.y = pos_y + 2;
        rocketUp.coord.x = pos_x + 4;

        /* Acquisizione del pid del processo missile per killarlo in seguito */
        rocketUp.objectPid = getpid();

        do {
            /* Generazione coordinate */
            rocketUp.coord.x++;

            /* Ogni 7 cicli aggiorna il movimento anche sull'asse y */
            if(loopCounter % 7 == 0 && loopCounter != 0)
                rocketUp.coord.y--;

            /* Scrittura delle informazioni aggiornate del missile sulla pipe */
            write(pipeOut, &rocketUp, sizeof(rocketUp));
 
            /* Velocita' missile */
            usleep(ROCKET_DELAY);

            loopCounter++;
        }while(rocketUp.coord.y > 1 && rocketUp.coord.x < MAX_X - 1);

        /* Terminazione del processo missile se non colpisce nessun alieno
           e quindi quando esce dalla finestra di gioco
        */
        _exit(0);
    }
    else {
        rocketDownPid = fork();
 
        if(rocketDownPid == 0) {
            /* Posizione da cui parte il missile */
            rocketDown.coord.y = pos_y + 2;
            rocketDown.coord.x = pos_x + 4;
            
            /* Acquisizione del pid del processo missile per killarlo in seguito */
            rocketDown.objectPid = getpid();

            do { 
                /* Generazione coordinate */
                rocketDown.coord.x++;

                /* Ogni 7 cicli aggiorna il movimento anche sull'asse y */
                if(loopCounter1 % 7 == 0 && loopCounter1 != 0)
                    rocketDown.coord.y++;


                /* Scrittura delle informazioni aggiornate del missile sulla pipe */
                write(pipeOut, &rocketDown, sizeof(rocketDown));
 
                /* Velocita' missile */
                usleep(ROCKET_DELAY);

                loopCounter1++;
            }while(rocketDown.coord.y < MAX_Y-1 && rocketDown.coord.x < MAX_X - 1);

        /* Terminazione del processo missile se non colpisce nessun alieno
           e quindi quando esce dalla finestra di gioco
        */
        _exit(0);
        }
    }
}