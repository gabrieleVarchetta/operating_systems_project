/* Inclusione di tutte le librerie esterne utilizzate */
#include "player.h"
#include "enemy.h"
#include "gameManager.h"
#include "util.h"

int main(int argc, char const *argv[]) {   
    /* Pulizia del terminale dai vari comandi per compilazione ed esecuzione */
    system("clear");

    /* Variabili generiche */
    int i, randomBomb;

    int posX, posY = 1;
    int alienSpacingY = 5, alienSpacingX = 5;

    struct timeval start, end;

    int score = 0;
    int nAlien, alienDelay;
    Difficulty difficulty;

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

    /* Inizializzazione della pipe */
    int fd[FDS];

    if (pipe(fd) == -1) {
        perror("Error creating main pipe.");
        _exit(1);
    }

    /* Inizio del contatore per il calcolo della durata della partita */
    gettimeofday(&start, NULL);

    /* Creazione dei vari processi player, alieni e gameManager */
    pid_t enemyPid[nAlien], playerPid;

    playerPid = fork();

    switch (playerPid) {
        
        case -1:
            perror("Execution error during fork.");
            _exit(1);
            break;
    
        case 0:
            close(fd[READ_END]);
            mainPlayer(fd[WRITE_END]);
            close(fd[WRITE_END]);
            break;

        default: // padre
            for (i = 0; i < nAlien; i++) {
                setAlienSpawnPos(&posX, &posY, &alienSpacingX, alienSpacingY, i);

                enemyPid[i] = fork();
                randomBomb = rand() % (MAX_BOMB - MIN_BOMB + 1) + MIN_BOMB;

                if(enemyPid[i] == -1) {
                    perror("Execution error during fork.");
                    _exit(1);
                }

                else if(enemyPid[i] == 0) {
                    close(fd[READ_END]);
                    alien(fd[WRITE_END], i, posX, posY, randomBomb, alienDelay);
                }
            }
                
            close(fd[WRITE_END]);
                
            score = gameManager(fd[READ_END], nAlien);
            break;
    }

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

    /* Chiusura della finestra di gioco e pulizia del terminale */
    endwin();
    system("clear");

    /* Terminazione processo principale */
    return 0;
}