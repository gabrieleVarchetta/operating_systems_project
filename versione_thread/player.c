#include "player.h"

/**
 * Funzione per la gestione dei movimenti e dei missili del giocatore.
 * @return void* 
 */
void* mainPlayer() {
    objectData player;
    pthread_t rocketUpThread, rocketDownThread;

    struct timeval start, end;
    bool canShoot = false;
    int fireDelay, minDelay = 7500 * MAX_X; //minDelay è il tempo che impiega il missile ad arrivare a destra
    
    /* Setting di alcuni valore di base del player */
    player.coords.x = 1;
    player.coords.y = MAX_Y / 2;
    player.object_type = PLAYER;
    player.objectTid = pthread_self();

    /* Per il primo ciclo si imposta il tempo trascorso dall'ultimo sparo uguale a minDelay + 1 */
    fireDelay = minDelay + 1; 

    /* Scrittura delle prime informazioni di base nel buffer */
    produce(player);

    while(!isGameEnded) {
        /* Movimento della navicella e lancio dei missili */
        char keyPressed;

        switch(keyPressed = getch()) {
            case UP_KEY:
                if(player.coords.y > 2)
                    player.coords.y -= MOVEMENT_RANGE;
                    
                break;
            
            case DOWN_KEY:
                if(player.coords.y <= MAX_Y - PLAYERSPRITE_Y) 
                    player.coords.y += MOVEMENT_RANGE;

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
                    /* Creazione e allocazione della memoria per i parametri
                       da passare al thread missile
                    */
                    initInfo* rocketUpInfo = malloc(sizeof(initInfo));
                    rocketUpInfo->pos = player.coords;
                    rocketUpInfo->id = 0;

                    /* Creazione del thread missile */
                    if(pthread_create(&rocketUpThread, NULL, &rockets, rocketUpInfo) != 0) {
                        perror("Error creating the thread.");
                        exit(1);
                    }

                    /* Creazione e allocazione della memoria per i parametri
                       da passare al thread missile
                    */
                    initInfo* rocketDownInfo = malloc(sizeof(initInfo));
                    rocketDownInfo->pos = player.coords;
                    rocketDownInfo->id = 1;

                    /* Creazione del thread missile */
                    if(pthread_create(&rocketDownThread, NULL, &rockets, rocketDownInfo) != 0) {
                        perror("Error creating the thread.");
                        exit(1);
                    }

                    /* Avvio del contatore per il tempo trascorso */
                    gettimeofday(&start, NULL);
                    canShoot = true;
                }
            
            break;
            
            default:
                break;
        }
        /* Inserimento nel buffer di tutte le nuove informazioni del player */
        produce(player);
    }
    
    return NULL;
}

void* rockets(void *args) {
    objectData rocketUp, rocketDown;
    int loopCounter = 0, loopCounter1 = 0;

    /* Casting dei parametri passati al thread */
    initInfo* rocketInfo = (initInfo*) args;
    
    /* Impostazione delle informazioni base dei missili */
    rocketUp.object_type = ROCKET;
    rocketUp.object_id = 0;

    rocketDown.object_type = ROCKET;
    rocketDown.object_id = 1;

    /* Avvio suono per il lancio dei missili */
    system(PLAYER_ROCKET_SOUND);

    if(rocketInfo->id == 0) {
        /* Posizione da cui parte il missile e id del thread missile */
        rocketUp.coords.y = rocketInfo->pos.y + 1;
        rocketUp.coords.x = rocketInfo->pos.x + 4;
        rocketUp.objectTid = pthread_self();

        do {
            /* Generazione coordinate */
            rocketUp.coords.x++;
        
            /* Ogni 7 cicli aggiorna il movimento anche sull'asse y */
            if(loopCounter % 7 == 0 && loopCounter != 0)
                rocketUp.coords.y--;

            /* Scrittura delle informazioni aggiornate del missile nel buffer */
            produce(rocketUp);

            /* Velocita' missile */
            usleep(ROCKET_DELAY);

            loopCounter++;
        }while(rocketUp.coords.y > 1 && rocketUp.coords.x < MAX_X - 1 && !isGameEnded);
    } 
    else {
        /* Posizione da cui parte il missile e id del thread missile */
        rocketDown.coords.y = rocketInfo->pos.y + 3;
        rocketDown.coords.x = rocketInfo->pos.x + 4;
        rocketDown.objectTid = pthread_self();

        do {   
            /* Generazione coordinate */
            rocketDown.coords.x++;

            /* Ogni 7 cicli aggiorna il movimento anche sull'asse y */
            if(loopCounter1 % 7 == 0 && loopCounter1 != 0) 
                rocketDown.coords.y++;

            /* Scrittura delle informazioni aggiornate del missile nel buffer */
            produce(rocketDown);

            /* Velocita' missile */
            usleep(ROCKET_DELAY);
          
            loopCounter1++;
        }while(rocketDown.coords.y < MAX_Y-1 && rocketDown.coords.x < MAX_X - 1 && !isGameEnded);
    }

    /* Rilascio della memoria allocata per i parametri dei thread */
    free(args);

    /* Terminazione dei thread in caso non collidano con nessun alieno */
    return NULL;
}