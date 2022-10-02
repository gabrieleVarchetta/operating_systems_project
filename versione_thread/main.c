#include "player.h"
#include "enemy.h"
#include "gameManager.h"
#include "util.h"

int main(int argc, char const *argv[]) {
    /* Pulizia del terminale dai vari comandi per compilazione ed esecuzione */
    system("clear");
    
    /* Variabili generiche */
    int i, alienSpacingY = 5, alienSpacingX = 5, posX, posY = 1;

    struct timeval start, end;

    int score;
    Difficulty difficulty;

    /* Inizializzazione degli indici del buffer e del flag globale */
    in = out = 0;
    isGameEnded = false;

    /* Inizializzazione dei file di testo per la leaderboard */
    FILE *fp;
    int nRecords, leaderboardPlayers;

    fp = openFile("./record.txt", "r");

    fscanf(fp, "%d\n", &nRecords);

    Record records[nRecords+1];

    for (i = 0; i < nRecords; i++)
       fscanf(fp, "%s %d %d\n", records[i].name, &records[i].sec, &records[i].score);
    
    fp = closeFile(fp);

    /* Inserimento del nome del giocatore corrente per la leaderboard */
    Record newRecord;
    
    do {
        printf("Enter your name: ");
        scanf("%s", newRecord.name);

        if(strlen(newRecord.name) > NAME_LEN) {
            printf("Name too long, try again (must be less than 8 characters)\n");
            sleep(2);
            system("clear");
            refresh();
        }
    }while(strlen(newRecord.name) > NAME_LEN);

    /* Inizializzazione di semafori e mutex */
    pthread_mutex_init(&bufferMutex, NULL);
    pthread_mutex_init(&printMutex, NULL);
    pthread_mutex_init(&mainMutex, NULL);
    sem_init(&fullSlots, 0, 0);
    sem_init(&emptySlots, 0, DIM_BUFFER);

    /* Inizializzazione del buffer */
    clean_buffer();
    
    /* Inizializzazione della finestra di gioco e dei colori */
    init();
    
    /* Esplosione iniziale e stampa del nome del gioco */
    printTitle();

    clear();
    refresh();

    /* Selezione della difficoltà del gioco */
    difficulty = chooseDifficulty();

    clear();
    refresh();

    switch (difficulty) {

        case EASY:
            nAlien = 12;
            alienDelay = 250000;
            break;
    
        case MEDIUM:
            nAlien = 16;
            alienDelay = 200000;
            break;
    
        case HARD:
            nAlien = 20;
            alienDelay = 150000;
            break;
        case CUSTOM:
            /* Parametri modificabili a piacere */
            nAlien = N_ALIEN;
            alienDelay = 250000;

        default:
            break;
    }

    /* Creazione dei thread id */
    pthread_t playerThread, alienThread[nAlien];

    /* Inizio del contatore per il calcolo della durata della partita */
    gettimeofday(&start, NULL);

    /* Creazione dei thread player e alieni */
    if(pthread_create(&playerThread, NULL, &mainPlayer, NULL) != 0) {
        perror("Error creating the thread.");
        exit(1);
    }

    for(i = 0; i < nAlien; i++) {
        setAlienSpawnPos(&posX, &posY, &alienSpacingX, alienSpacingY, i);

        /* Creazione e allocazione della memoria per i parametri da passare agli alieni */
        initInfo* alienInit = malloc(sizeof(initInfo));

        alienInit->pos.x = posX;
        alienInit->pos.y = posY;
        alienInit->id = i;

        if(pthread_create(&alienThread[i], NULL, &alienFunct, alienInit) != 0) {
            perror("Error creating the thread.");
            exit(1);
        }
    }

    /* Il thread padre eseguirà il gameManager */
    score = gameManager();
    
    /* Terminazione del contatore per la durata della partita */
    gettimeofday(&end, NULL);

    /* Pulizia della finestra di gioco */
    clear();

    /* Salvataggio delle statistiche del giocatore attuale */
    newRecord.sec = (end.tv_sec - start.tv_sec);
    newRecord.score = score;
    records[nRecords] = newRecord;
    nRecords++;

    fp = openFile("./record.txt", "w");

    fprintf(fp, "%d\n", nRecords);

    for(i = 0; i < nRecords; i++)
        fprintf(fp, "%s %d %d\n", records[i].name, records[i].sec, records[i].score);
    
    fp = closeFile(fp);

    /* Stampa del messaggio di vittoria o sconfitta in base al punteggio totalizzato */
    if(score == ALIEN_SCORE * nAlien * ALIEN_LIVES) {
        printBorder(MAX_X, MAX_Y);
        mvprintw(MAX_Y/2, MAX_X/2 - 7,"You have won!");
        refresh();
    } 
    else {
        printBorder(MAX_X, MAX_Y);
        mvprintw(MAX_Y/2, MAX_X/2 - 7,"You have lost!");
        refresh();
    }
 
    sleep(2); 
    clear();

    /* Ordinamento in base a punteggio e tempo dei record presenti nel file tramite algoritmo Selection Sort */
    selectionSort(records, nRecords);

    /* Stampa della top 10 dei giocatori presenti nel file */
    mvprintw(MAX_Y / 2 - 8, MAX_X / 2 - 13, "Top 10 Player Leaderboard");
    mvprintw(MAX_Y / 2 - 5, MAX_X / 2 - 20, "Rank");
    mvprintw(MAX_Y / 2 - 5, MAX_X / 2 - 8, "Name");
    mvprintw(MAX_Y / 2 - 5, MAX_X / 2 + 4, "Score");
    mvprintw(MAX_Y / 2 - 5, MAX_X / 2 + 16, "Time");

    refresh();

    if (nRecords < 10)
        leaderboardPlayers = nRecords;
    else
        leaderboardPlayers = 10;


    for (i = 0; i < leaderboardPlayers; i++) {
        mvprintw(MAX_Y / 2 + i - 2, MAX_X / 2 - 20, "%d", i + 1);
        mvprintw(MAX_Y / 2 + i - 2, MAX_X / 2 - 8, "%s", records[i].name);
        mvprintw(MAX_Y / 2 + i - 2, MAX_X / 2 + 4, "%d", records[i].score);
        mvprintw(MAX_Y / 2 + i - 2, MAX_X / 2 + 16, "%d seconds", records[i].sec);

        refresh();
    }
    
    sleep(5);

    /* Terminazione di tutti i suoni utilizzati nel gioco */
    system(KILL_SOUND);

    /* Distruzione dei mutex e semafori utilizzati */
    pthread_mutex_destroy(&bufferMutex);
    pthread_mutex_destroy(&printMutex);
    pthread_mutex_destroy(&mainMutex);
    sem_destroy(&fullSlots);
    sem_destroy(&emptySlots);

    /* Chiusura della finestra di gioco e pulizia del terminale */
    endwin();
    system("clear");
    
    /* Terminazione del thread principale */
    return 0;
}