#include "gameManager.h"

/* Dichiarazione e inizializzazione degli sprite degli oggetti */
const char playerSprite[PLAYERSPRITE_X][PLAYERSPRITE_Y] = {{"*\\    "}, {"* |-> "}, {"* |-->"}, {"* |-> "}, {"*/    "}};
const char enemySprite[ALIENSPRITE_X][ALIENSPRITE_Y] = {{" _ "}, {"/ \\"}, {"***"}};
const char enemySprite2[ALIENSPRITE_X][ALIENSPRITE_Y] = {{"V V"}, {"   "}, {"V V"}};

/**
 * Funzione che gestisce l'intero gioco. Si occupa di consumare dal buffer le informazioni
 * che gli altri thread inseriscono al suo interno. Gestisce la stampa di tutti gli oggetti
 * in movimento, si occupa di verificare le varie collisioni tra gli oggetti in movimento e
 * infine manda una richiesta di cancellazione ai thread con la pthread_cancel. Restituisce il punteggio 
 * totalizzato durante la partita.
 */
int gameManager() {
    /* Creazione oggetti alieni e bombe, il numero di bombe è uguale al numero di alieni */
    objectData alienTemp[nAlien], bombTemp[nAlien];

    /* Creazione oggetto player e oggetti missili */
    objectData playerTemp, rocketTemp[2];

    int alienId, bombId, rocketId, i, alienCollidedWith, score = 0;
    int maxScore = nAlien * ALIEN_SCORE * ALIEN_LIVES;

    /* Setting dei valori degli alieni non inseriti nel buffer e 
       inizializzazione della posizione fuori dalla finestra di 
       gioco
    */
    for(i = 0; i < nAlien; i++) {
        alienTemp[i].alive = true;
        alienTemp[i].lives = ALIEN_LIVES;
        alienTemp[i].coords.x = 150;
        bombTemp[i].coords.x = -10;
    }

    /* Setting dei valori della navicella non inseriti nel buffer e 
       inizializzazione della posizione fuori dalla finestra di 
       gioco
    */
    playerTemp.lives = PLAYER_LIVES;
    playerTemp.coords.x = -10;
    playerTemp.alive = true;

    /* Setting della posizione fuori dalla finestra di gioco */
    rocketTemp[0].coords.x = -10;
    rocketTemp[1].coords.x = -10;
    
    /* Stampa della cornice del gioco */
    pthread_mutex_lock(&printMutex);
        printBorder();
    pthread_mutex_unlock(&printMutex);

    /* Avvio della soundtrack principale del gioco */
    system(MAIN_SOUNDTRACK);
    
    while (!isGameEnded) {
        /* Acquisizione delle informazioni contenute nel buffer inserite dai vari thread */
        consume();
        
        /* Diverse gestioni in base al tipo di oggetto che è stato letto */
        switch(consumedProduct.object_type) {
            case ALIEN:
                /* Salvataggio dell'id dell'alieno letto su una variabile di appoggio */
                alienId = consumedProduct.object_id;

                /* Se l'oggetto si trova dentro la finestra di gioco, elimina la
                   vecchia posizione
                */
                if(alienTemp[alienId].coords.x >= 0) {
                    /* Creazione della sezione critica per la stampa e chiusura di tale sezione */
                    pthread_mutex_lock(&printMutex);
                        deleteSprite(ALIENSPRITE_X, ALIENSPRITE_Y, alienTemp[alienId].coords);
                    pthread_mutex_unlock(&printMutex);
                }

                /* Salvataggio delle nuove coordinate e del thread id dell'alieno letto dal buffer */
                alienTemp[alienId].coords = consumedProduct.coords;
                alienTemp[alienId].objectTid = consumedProduct.objectTid;

              
                /* Creazione della sezione critica per la stampa e chiusura di tale sezione */
                pthread_mutex_lock(&printMutex);   
                    /* Se l'alieno ha più di 2 vite, sarà nella "fase 1", quindi stampo il primo sprite,
                       altrimenti sarà nella seconda fase quindi stampa il secondo sprite
                    */             
                    if(alienTemp[alienId].lives > 2)
                        printSprite(ALIENSPRITE_X, ALIENSPRITE_Y, enemySprite, alienTemp[alienId].coords, ALIEN_COLOR);
                    else
                        printSprite(ALIENSPRITE_X, ALIENSPRITE_Y, enemySprite2, alienTemp[alienId].coords, ALIEN_COLOR);                  
                pthread_mutex_unlock(&printMutex);

                /* Se uno degli alieni collide con la navicella oppure se uno degli alieni arriva 
                   alla sinistra della finestra, uccidi la navicella e termina il gioco 
                */
                if(checkAlienPlayerCollision(playerTemp.coords, nAlien, alienTemp) || alienTemp[alienId].coords.x <= 0) {
                    playerTemp.alive = false;

                    pthread_mutex_lock(&printMutex);
                        deleteSprite(PLAYERSPRITE_X, PLAYERSPRITE_Y, playerTemp.coords);
                    pthread_mutex_unlock(&printMutex);

                    pthread_cancel(playerTemp.objectTid);  
                }

                /* Se l'alieno ha 0 o meno vite sarà morto, quindi cancella lo sprite, imposta
                   il campo "alive" della struttura objectData a false e invia una richiesta
                   di cancellazione al thread dell'alieno che è morto
                */
                if(alienTemp[alienId].lives <= 0) {
                    /* Creazione della sezione critica per la stampa e chiusura di tale sezione */
                    pthread_mutex_lock(&printMutex);
                        deleteSprite(ALIENSPRITE_X, ALIENSPRITE_Y, alienTemp[alienId].coords);
                    pthread_mutex_unlock(&printMutex);

                    alienTemp[alienId].alive = false;
                    pthread_cancel(alienTemp[alienId].objectTid);
                }
                
                /* Creazione della sezione critica per il refresh e chiusura di tale sezione */
                pthread_mutex_lock(&printMutex);
                    refresh();
                pthread_mutex_unlock(&printMutex);

                break;
            
            case PLAYER:
                /* Se l'oggetto si trova dentro la finestra di gioco, elimina la
                   vecchia posizione
                */
                if(playerTemp.coords.x >= 0) {
                    pthread_mutex_lock(&printMutex);
                        deleteSprite(PLAYERSPRITE_X, PLAYERSPRITE_Y, playerTemp.coords);
                    pthread_mutex_unlock(&printMutex);
                }

                /* Salvataggio delle nuove coordinate e del thread id della navicella letti dal buffer */
                playerTemp.coords = consumedProduct.coords;
                playerTemp.objectTid = consumedProduct.objectTid;

                pthread_mutex_lock(&printMutex);
                /* Se la navicella è viva, stampa il suo sprite */
                if(playerTemp.alive)
                    printSprite(PLAYERSPRITE_X, PLAYERSPRITE_Y, playerSprite, playerTemp.coords, PLAYER_COLOR);
                pthread_mutex_unlock(&printMutex);

                /* Se la navicella ha 0 o meno vite sarà morta, quindi cancella lo sprite, imposta
                   il campo "alive" della struttura objectData a false e invia una richiesta di cancellazione
                   al thread della navicella
                */
                if(playerTemp.lives <= 0) {
                    pthread_mutex_lock(&printMutex);
                        printSprite(PLAYERSPRITE_X, PLAYERSPRITE_Y, playerSprite, playerTemp.coords, PLAYER_COLOR);
                    pthread_mutex_unlock(&printMutex);

                    playerTemp.alive = false;
                    pthread_cancel(playerTemp.objectTid);  
                }

                pthread_mutex_lock(&printMutex);
                    refresh();
                pthread_mutex_unlock(&printMutex);

                break;

            case BOMB:
                /* Salvataggio dell'id della bomba letta su una variabile di appoggio */
                bombId = consumedProduct.object_id;

                /* Se l'alieno che ha lanciato la bomba è vivo, stampa la bomba
                   ed effettua i controlli sulle collisioni
                */
                if(bombTemp[bombId].coords.x >= 0) {
                    pthread_mutex_lock(&printMutex);
                        mvaddch(bombTemp[bombId].coords.y, bombTemp[bombId].coords.x, ' ');
                    pthread_mutex_unlock(&printMutex);
                }

                /* Salvataggio delle informazioni della bomba letta dal buffer */
                bombTemp[bombId] = consumedProduct;
                
                /* Stampa della bomba */
                pthread_mutex_lock(&printMutex);
                    attron(COLOR_PAIR(BOMB_COLOR));
                    mvaddch(bombTemp[bombId].coords.y, bombTemp[bombId].coords.x, '<');
                    attroff(COLOR_PAIR(BOMB_COLOR));
                pthread_mutex_unlock(&printMutex);

                /* Verifica di eventuali collisioni tra la bomba e la navicella */
                if(checkPlayerCollision(bombTemp[bombId].coords, playerTemp.coords)) {
                    /* Cancella lo sprite della navicella e la bomba, per creare una sorta di 
                        effetto "lampeggio" sulla navicella 
                    */
                    pthread_mutex_lock(&printMutex);
                        deleteSprite(PLAYERSPRITE_X, PLAYERSPRITE_Y, playerTemp.coords);
                        mvaddch(bombTemp[bombId].coords.y, bombTemp[bombId].coords.x, ' ');
                        refresh();
                    pthread_mutex_unlock(&printMutex);

                    /* Stampa di nuovo la navicella */
                    pthread_mutex_lock(&printMutex);
                        printSprite(PLAYERSPRITE_X, PLAYERSPRITE_Y, playerSprite, playerTemp.coords, PLAYER_COLOR);
                        refresh();
                    pthread_mutex_unlock(&printMutex);

                    /* Aggiorna le vite della navicella */
                    playerTemp.lives--;

                    /* Avvio del suono per l'esplosione della navicella */
                    system(PLAYER_EXPLOSION_SOUND);

                    /* Richiesta di cancellazione del thread bomba */
                    pthread_cancel(bombTemp[bombId].objectTid);
                }

                /* Se la bomba non è più all'interno della finestra di gioco, cancella il carattere */
                if(bombTemp[bombId].coords.x <= 1) {
                    pthread_mutex_lock(&printMutex);
                        mvaddch(bombTemp[bombId].coords.y, bombTemp[bombId].coords.x, ' ');
                    pthread_mutex_unlock(&printMutex);
                }
            
                pthread_mutex_lock(&printMutex);
                    refresh();
                pthread_mutex_unlock(&printMutex);

                break;
            
            case ROCKET:
                /* Salvataggio dell'id del missile letto su una variabile di appoggio */
                rocketId = consumedProduct.object_id;

                /* Se l'oggetto si trova dentro la finestra di gioco, elimina la
                   vecchia posizione
                */
                if(rocketTemp[rocketId].coords.x >= 0) {
                    pthread_mutex_lock(&printMutex);
                        mvaddch(rocketTemp[rocketId].coords.y, rocketTemp[rocketId].coords.x, ' ');
                    pthread_mutex_unlock(&printMutex);
                }

                /* Salvataggio delle informazioni del missile letto dal buffer */
                rocketTemp[rocketId] = consumedProduct;

                /* Stampa del missile */
                pthread_mutex_lock(&printMutex);
                    attron(COLOR_PAIR(ROCKET_COLOR));
                    mvaddch(rocketTemp[rocketId].coords.y, rocketTemp[rocketId].coords.x, '*');
                    attroff(COLOR_PAIR(ROCKET_COLOR));
                pthread_mutex_unlock(&printMutex);
                
                /* Verifica di eventuali collisioni tra il missile e uno degli alieni.
                   L'eventuale id dell'alieno colpito viene salvato in una variabile 
                   di appoggio
                */
                alienCollidedWith = checRocketCollision(rocketTemp[rocketId].coords, nAlien, alienTemp);
                
                /* Se il missile ha colliso con un alieno */
                if(alienCollidedWith != -1) {
                    /* Cancella lo sprite dell'alieno colpito e il missile */
                    pthread_mutex_lock(&printMutex);
                        deleteSprite(ALIENSPRITE_X, ALIENSPRITE_Y, alienTemp[alienCollidedWith].coords);
                        mvaddch(rocketTemp[rocketId].coords.y, rocketTemp[rocketId].coords.x, ' ');
                        refresh();
                    pthread_mutex_unlock(&printMutex);

                    /* Aggiornamento delle vite dell'alieno colpito */
                    alienTemp[alienCollidedWith].lives--;

                    /* Aggiornamento del punteggio */
                    score += ALIEN_SCORE;

                    /* Avvio del suono per l'esplosione dell'alieno colpito */
                    system(ALIEN_EXPLOSION_SOUND);

                    /* Richiesta di cancellazione del thread missile */
                    pthread_cancel(rocketTemp[rocketId].objectTid);
                }
                
                /* Se il missile non è più all'interno della finestra di gioco, cancella il carattere */
                if(rocketTemp[rocketId].coords.y <= 1 || rocketTemp[rocketId].coords.y >= MAX_Y - 1 || rocketTemp[rocketId].coords.x >= MAX_X - 1) {
                    pthread_mutex_lock(&printMutex);
                        mvaddch(rocketTemp[rocketId].coords.y, rocketTemp[rocketId].coords.x, ' ');
                    pthread_mutex_unlock(&printMutex);
                } 
                    
                pthread_mutex_lock(&printMutex);
                    refresh();
                pthread_mutex_unlock(&printMutex);

                break;          
        }
        
        /* Se la navicella finisce le vite o vengono uccisi gli alieni, il gioco termina */
        if(!playerTemp.alive || score >= maxScore)
            isGameEnded = true;

        /* Stampa delle statistiche */
        pthread_mutex_lock(&printMutex);
            attron(COLOR_PAIR(TEXT_COLOR));
            mvprintw(1,1, "Lives: %d", playerTemp.lives);
            mvprintw(1,MAX_X-15, "Score: %d", score);
            attroff(COLOR_PAIR(TEXT_COLOR));
            curs_set(0);
            refresh();
        pthread_mutex_unlock(&printMutex);    
    }

    /* Terminazione di eventuali thread ancora attivi in caso di vittoria o sconfitta */
    if(playerTemp.alive)
        pthread_cancel(playerTemp.objectTid);
    
    for(i = 0; i < nAlien; i++) {
        if(alienTemp[i].alive)
            pthread_cancel(alienTemp[i].objectTid);
    }

    return score;
}

/**
 * Procedura per la stampa di un generico sprite
 * @param rows numero di righe dello sprite
 * @param columns numero di colonne dello sprite
 * @param sprite sprite da stampare
 * @param pos posizione dell'oggetto di cui stampare lo sprite
 * @param objectColor colore dello sprite da stampare
 */
void printSprite(int rows, int columns, const char sprite[rows][columns], position pos, ObjectColor objectColor) {
    int i, j;

    /* Impostazione del colore dello sprite */
    attron(COLOR_PAIR(objectColor));

    /* Scorre le righe e le colonne, stampando carattere per carattere nelle posizioni corrette */
    for(i = 0; i < rows; i++)
        for(j = 0; j < columns; j++)
            mvaddch(pos.y + i, pos.x + j, sprite[i][j]);
    
    attroff(COLOR_PAIR(objectColor));
}

/**
 * Procedura per la cancellazione di un generico sprite
 * @param rows numero di righe dello sprite
 * @param columns numero di colonne dello sprite
 * @param pos posizione dell'oggetto di cui cancellare lo sprite
 */
void deleteSprite(int rows, int columns, position pos) {
    int i, j;

    /* Scorre le righe e le colonne, cancellando carattere per carattere nelle posizioni corrette */
    for(i = 0; i < rows; i++)
        for(j = 0; j < columns; j++)
            mvaddch(pos.y + i, pos.x + j, ' ');
}

/** 
 * Procedura che stampa una cornice lungo i bordi della finestra di gioco
 * @param rows 
 * @param columns 
 */
void printBorder() {
    int i;

    mvaddch(0,0, ACS_ULCORNER);
    mvaddch(MAX_Y, 0, ACS_LLCORNER);

    mvaddch(0, MAX_X, ACS_URCORNER);
    mvaddch(MAX_Y, MAX_X, ACS_LRCORNER);

    for(i = 1; i < MAX_X; i++) {
        mvaddch(0, i, ACS_HLINE);
        mvaddch(MAX_Y, i, ACS_HLINE);
    }

    for(i = 1; i < MAX_Y; i++) {
        mvaddch(i, 0, ACS_VLINE);
        mvaddch(i, MAX_X, ACS_VLINE);
    }
}

/**
 * Funzione che verifica le collisioni tra i missili e gli alieni. Scorre la lista
 * di alieni vivi, scorre ogni alieno carattere per carattere e se la posizione del 
 * missile coincide con la posizione di uno dei caratteri dell'alieno, restituisce
 * l'indice dell'alieno con cui ha colliso. Se non c'è stata nessuna collisione, 
 * restituisce -1
 * @param rocketPos posizione del misisle
 * @param nAlien numero di alieni in base alla difficoltà scelta
 * @param alienList lista di alieni
 * @return int indice dell'alieno colpito oppure -1
 */
int checRocketCollision(position rocketPos, int nAlien, objectData alienList[nAlien]) {
    int i, j, k;

    for(i = 0; i < nAlien; i++) {
        for(j = 0; j < ALIENSPRITE_X; j++) {
            for(k = 0; k < ALIENSPRITE_Y; k++) {
                if(alienList[i].alive) {
                    if((rocketPos.x == alienList[i].coords.x + j) && (rocketPos.y == alienList[i].coords.y + k))
                        return i; //id dell'alieno con cui il missile ha colliso
                }
            }
        }
    }
        
    return -1;
}

/**
 * Funzione che verifica le collisioni tra alieni e navicella. Scorre il vettore di alieni e 
 * dopo aver controllato se l'alieno i-esimo è vivo, scorre lo sprite della navicella carattere
 * per carattere e controlla se la posizione dell'alieno coincide con uno dei caratteri della 
 * navicella, se coincidono restituisce true, altrimenti false
 * @param playerPos posizione della navicella
 * @param nAlien numero di alieni in base alla difficoltà
 * @param alienList vettore di alieni
 * @return true 
 * @return false 
 */
bool checkAlienPlayerCollision(position playerPos, int nAlien, objectData alienList[nAlien]) {
    int i, j, k;

    for(i = 0; i < nAlien; i++) {
        if(alienList[i].alive) {
            for(j = 0; j < PLAYERSPRITE_X; j++)
                for(k = 0; k < PLAYERSPRITE_Y; k++)
                    if(alienList[i].coords.x == playerPos.x + k && alienList[i].coords.y == playerPos.y + j)
                        return true;
        }
    }

    return false;
}

/**
 * Funzione che verifica le collisioni tra le bombe e la navicella. Scorre l'intero
 * sprite della navicella carattere per carattere e controlla se la posizione della 
 * bomba coincide con la posizione di uno dei caratteri della navicella, restituisce
 * true, altrimenti false
 * @param bombPos posizione della bomba
 * @param playerPos posizione della navicella
 * @return true 
 * @return false 
 */
bool checkPlayerCollision(position bombPos, position playerPos){
    int i, j;

    for(i = 0; i < PLAYERSPRITE_X; i++)
        for(j = 0; j < PLAYERSPRITE_Y; j++)
            if(bombPos.x == playerPos.x + j && bombPos.y == playerPos.y + i)
                return true;

    return false;            
}