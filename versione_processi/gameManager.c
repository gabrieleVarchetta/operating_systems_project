#include "gameManager.h"

/* Dichiarazione e inizializzazione degli sprite degli oggetti */
const char playerSprite[PLAYERSPRITE_X][PLAYERSPRITE_Y] = {{"*\\    "}, {"* |-> "}, {"* |-->"}, {"* |-> "}, {"*/    "}};
const char enemySprite[ALIENSPRITE_X][ALIENSPRITE_Y] = {{" _ "}, {"/ \\"}, {"***"}};
const char enemySprite2[ALIENSPRITE_X][ALIENSPRITE_Y] = {{"V V"}, {"   "}, {"V V"}};

/**
 * Funzione che gestisce l'intero gioco. Si occupa di leggere dalla pipe su cui gli altri 
 * processi scrivono le informazioni a loro relative. Gestisce la stampa di tutti gli oggetti
 * in movimento, si occupa di verificare le varie collisioni tra gli oggetti in movimento e
 * infine termina i processi che non servono più tramite una kill. Restituisce il punteggio 
 * totalizzato durante la partita.
 * @param pipeIn pipe da cui leggere le informazioni relative i vari processi
 * @param nAlien numero di alieni in base alla difficoltà scelta
 * @return int punteggio totalizzato
 */
int gameManager(int pipeIn, int nAlien) {
    /* Creazione oggetti alieni e bombe, il numero di bombe è uguale al numero di alieni */
    objectData aliens[nAlien], bombs[nAlien];

    /* Creazione oggetto player e oggetti missili */
    objectData player, rockets[2];

    /* Creazione oggetto di appoggio su cui salvare momentaneamente i dati letti dalla pipe */
    objectData pipeValue;

    int alienId, rocketId, bombId, i, alienCollidedWith, score = 0;
    bool flag = true;
    int maxScore = nAlien * ALIEN_SCORE * ALIEN_LIVES;

    /* Setting dei valori degli alieni non passati tramite pipe e 
       inizializzazione della posizione fuori dalla finestra di 
       gioco
    */
    for (i = 0; i < nAlien; i++) {
        aliens[i].coord.x = 150;
        aliens[i].lives = ALIEN_LIVES;
        aliens[i].alive = true;
        bombs[i].coord.x = -10;
    }

    /* Setting della posizione fuori dalla finestra di gioco */
    for(i = 0; i < 2; i++)
        rockets[i].coord.x = -10;
    
    /* Setting dei valori della navicella non passati tramite pipe e
       inizializzazione della posizione fuori dalla finestra di gioco
    */
    player.coord.x = -10;
    player.lives = PLAYER_LIVES;
    player.alive = true;

    /* Procedura che stampa la cornice del gioco */
    printBorder();

    /* Avvio della soundtrack principale del gioco */
    system(MAIN_SOUNDTRACK);

    do {
        /* Lettura dalla pipe delle informazioni scritte dai vari processi */
        read(pipeIn, &pipeValue, sizeof(pipeValue));

        /* Diverse gestioni in base al tipo di oggetto che è stato letto */
        switch (pipeValue.object_type) {
            case ALIEN:
                /* Salvataggio dell'id dell'alieno letto su una variabile di appoggio */
                alienId = pipeValue.object_id;

                /* Se l'oggetto si trova dentro la finestra di gioco, elimina la
                   vecchia posizione
                */
                if (aliens[alienId].coord.x >= 0)
                    deleteSprite(ALIENSPRITE_X, ALIENSPRITE_Y, aliens[alienId].coord);

                /* Salvataggio delle nuove coordinate e del pid dell'alieno letto dalla pipe */
                aliens[alienId].coord = pipeValue.coord;
                aliens[alienId].objectPid = pipeValue.objectPid;

                /* Se l'alieno ha più di 2 vite, sarà nella "fase 1", quindi stampo il primo sprite,
                   altrimenti sarà nella seconda fase quindi stampa il secondo sprite
                */
                if(aliens[alienId].lives > 2)
                    printSprite(ALIENSPRITE_X, ALIENSPRITE_Y, enemySprite, aliens[alienId].coord, ALIEN_COLOR);
                else
                    printSprite(ALIENSPRITE_X, ALIENSPRITE_Y, enemySprite2, aliens[alienId].coord, ALIEN_COLOR);

                /* Se uno degli alieni collide con la navicella oppure se uno degli alieni arriva 
                   alla sinistra della finestra, uccidi la navicella e termina il gioco 
                */
                if(checkAlienPlayerCollision(player.coord, nAlien, aliens) || aliens[alienId].coord.x <= 0) {
                    player.alive = false;
                    deleteSprite(PLAYERSPRITE_X, PLAYERSPRITE_Y, player.coord);
                    kill(player.objectPid, SIGKILL);
                }

                /* Se l'alieno ha 0 o meno vite sarà morto, quindi cancella lo sprite, imposta
                   il campo "alive" della struttura objectData a false e termina il processo
                   dell'alieno che è morto
                */
                if(aliens[alienId].lives <= 0) {
                    deleteSprite(ALIENSPRITE_X, ALIENSPRITE_Y, aliens[alienId].coord);
                    aliens[alienId].alive = false;
                    kill(aliens[alienId].objectPid, SIGKILL);
                }
               
                refresh();

                break;
            
            case PLAYER:
                /* Se l'oggetto si trova dentro la finestra di gioco, elimina la
                   vecchia posizione
                */
                if(player.coord.x >= 0)
                    deleteSprite(PLAYERSPRITE_X, PLAYERSPRITE_Y, player.coord);
                
                /* Salvataggio delle nuove coordinate e del pid della navicella letti dalla pipe */
                player.coord = pipeValue.coord;
                player.objectPid = pipeValue.objectPid;

                /* Se la navicella è viva, stampa il suo sprite */
                if(player.alive)
                    printSprite(PLAYERSPRITE_X, PLAYERSPRITE_Y, playerSprite, player.coord, PLAYER_COLOR);

                /* Se la navicella ha 0 o meno vite sarà morta, quindi cancella lo sprite, imposta
                   il campo "alive" della struttura objectData a false e termina il processo
                   della navicella
                */
                if(player.lives <= 0) {
                    deleteSprite(PLAYERSPRITE_X, PLAYERSPRITE_Y, player.coord);
                    player.alive = false;
                    kill(player.objectPid, SIGKILL);
                }

                refresh();

                break;
            
            case ROCKET:
                /* Salvataggio dell'id del missile letto su una variabile di appoggio */
                rocketId = pipeValue.object_id;
                
                /* Se l'oggetto si trova dentro la finestra di gioco, elimina la
                   vecchia posizione
                */
                if(rockets[rocketId].coord.x >= 0)
                    mvaddch(rockets[rocketId].coord.y, rockets[rocketId].coord.x, ' ');

                /* Salvataggio delle informazioni del missile letto dalla pipe */
                rockets[rocketId] = pipeValue;

                /* Impostazione del colore del missile e stampa del missile */      
                attron(COLOR_PAIR(ROCKET_COLOR));
                mvaddch(rockets[rocketId].coord.y, rockets[rocketId].coord.x, '*');
                attroff(COLOR_PAIR(ROCKET_COLOR));
                /* Verifica di eventuali collisioni tra il missile e uno degli alieni.
                   L'eventuale id dell'alieno colpito viene salvato in una variabile 
                   di appoggio
                */
                alienCollidedWith = checRocketCollision(rockets[rocketId].coord, nAlien, aliens);

                /* Se il missile ha colliso con un alieno */
                if(alienCollidedWith != -1) {
                    /* Cancella lo sprite dell'alieno colpito */
                    deleteSprite(ALIENSPRITE_X, ALIENSPRITE_Y, aliens[alienCollidedWith].coord);
                    refresh();

                    /* Cancella il carattere del missile */
                    mvaddch(rockets[rocketId].coord.y, rockets[rocketId].coord.x, ' ');

                    /* Aggiornamento delle vite dell'alieno colpito */
                    aliens[alienCollidedWith].lives--;

                    /* Aggiornamento del punteggio */
                    score += ALIEN_SCORE;

                    /* Avvio del suono per l'esplosione dell'alieno colpito */
                    system(ALIEN_EXPLOSION_SOUND);

                    /* Terminazione del processo missile */
                    kill(rockets[rocketId].objectPid, SIGKILL);
                }

                /* Se il missile non è più all'interno della finestra di gioco, cancella il carattere */
                if(rockets[rocketId].coord.y <= 1 || rockets[rocketId].coord.y >= MAX_Y-1 || rockets[rocketId].coord.x >= MAX_X - 1)
                    mvaddch(rockets[rocketId].coord.y, rockets[rocketId].coord.x, ' ');

                refresh();

                break;

            case BOMB:
                /* Salvataggio dell'id della bomba letta su una variabile di appoggio */
                bombId = pipeValue.object_id;

                /* Se l'oggetto si trova dentro la finestra di gioco, elimina la
                   vecchia posizione
                */
                if(bombs[bombId].coord.x >= 0)
                    mvaddch(bombs[bombId].coord.y, bombs[bombId].coord.x, ' ');

                /* Salvataggio delle informazioni della bomba letta dalla pipe */
                bombs[bombId] = pipeValue;

                /* Impostazione del colore della bomba e stampa della bomba */
                attron(COLOR_PAIR(BOMB_COLOR));
                mvaddch(bombs[bombId].coord.y, bombs[bombId].coord.x, '<');
                attroff(COLOR_PAIR(BOMB_COLOR));
                /* Verifica di eventuali collisioni tra la bomba e la navicella */
                if(checkPlayerCollision(bombs[bombId].coord, player.coord)) {
                    /* Cancella lo sprite della navicella, per creare una sorta di 
                       effetto "lampeggio" 
                    */
                    deleteSprite(PLAYERSPRITE_X, PLAYERSPRITE_Y, player.coord);
                    refresh();

                    /* Cancella il carattere della bomba */
                    mvaddch(bombs[bombId].coord.y, bombs[bombId].coord.x, ' ');

                    /* Aggiorna le vite della navicella */
                    player.lives--;

                    /* Avvio del suono per l'esplosione della navicella */
                    system(PLAYER_EXPLOSION_SOUND);

                    /* Terminazione del processo bomba */
                    kill(bombs[bombId].objectPid, SIGKILL);
                }

                /* Se la bomba non è più all'interno della finestra di gioco, cancella il carattere */
                if(bombs[bombId].coord.x <= 1)
                    mvaddch(bombs[bombId].coord.y, bombs[bombId].coord.x, ' ');

                refresh();
                break;
        }

        /* Impostazione colore per la stampa delle statistiche e stampa delle statistiche */
        attron(COLOR_PAIR(TEXT_COLOR));
        mvprintw(1,1, "Lives: %d", player.lives);
        mvprintw(1,MAX_X-15, "Score: %d", score);
        attroff(COLOR_PAIR(TEXT_COLOR));
        curs_set(0);
        refresh();
    }while(player.alive && score < maxScore);

    /* Terminazione di eventuali processi ancora attivi in caso di vittoria o sconfitta */
    if(player.alive)
        kill(player.objectPid, SIGKILL);
    
    for(i = 0; i < nAlien; i++) 
        if(aliens[i].alive)
            kill(aliens[i].objectPid, SIGKILL);
            
    /* Pulizia della finestra di gioco */
    clear();
    
    return score;
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
                    if((rocketPos.x == alienList[i].coord.x + j) && (rocketPos.y == alienList[i].coord.y + k))
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
                    if(alienList[i].coord.x == playerPos.x + k && alienList[i].coord.y == playerPos.y + j)
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