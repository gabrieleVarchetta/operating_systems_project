#include "util.h"

/**
 * Procedura di inizializzazione degli sprite, della finestra di gioco e dei colori.
 */
void init() {
    srand(time(NULL));
    initscr();
    noecho();
    curs_set(0);
    srand(time(NULL));
    start_color();

    init_pair(PLAYER_COLOR, COLOR_CYAN, COLOR_BLACK);
    init_pair(ALIEN_COLOR, COLOR_GREEN, COLOR_BLACK);
    init_pair(ROCKET_COLOR, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(BOMB_COLOR, COLOR_YELLOW, COLOR_BLACK);
    init_pair(TEXT_COLOR, COLOR_WHITE, COLOR_BLACK);
}

/**
 * Procedura per la stampa di un'esplosione iniziale e del titolo del gioco.
 */
void printTitle(){
    int i = 0;
    char introString[21] = "Press ENTER to play!";
    bool flag = true;

    /* Avvio della musica per l'intro del gioco */
    system(INTRO_SOUNDTRACK);

    /* Procedura per la stampa di un'esplosione */
    kaboom();

    /* Stampa di 100 stelle per simulare un effetto "spazio" */
    for(i = 0; i < 100; i++){
        attron(COLOR_PAIR(rand() % 6));
        mvaddch(rand() % MAX_Y - 1, rand() % MAX_X - 1, '*');
        refresh();
        usleep(1000);
    }   

    /* Selezione colore del titolo del gioco */
    attron(COLOR_PAIR(TEXT_COLOR));

    /* Stampa del titolo del gioco */
    mvprintw(4, MAX_X / 3-22, "   _____                          ____       ____               __         \n");
    printBorder(MAX_Y, MAX_X);
    usleep(500000);
    refresh();
    mvprintw(5, MAX_X / 3-22, "  / ___/____  ____ _________     / __ \\___  / __/__  ____  ____/ /__  _____\n");
    printBorder(MAX_Y, MAX_X);
    usleep(500000);
    refresh();
    mvprintw(6, MAX_X / 3-22, "  \\__ \\/ __ \\/ __ `/ ___/ _ \\   / / / / _ \\/ /_/ _ \\/ __ \\/ __  / _ \\/ ___/\n");
    printBorder(MAX_Y, MAX_X);
    usleep(500000);
    refresh();
    mvprintw(7, MAX_X / 3-22, " ___/ / /_/ / /_/ / /__/  __/  / /_/ /  __/ __/  __/ / / / /_/ /  __/ /    \n");
    printBorder(MAX_Y, MAX_X);
    usleep(500000);
    refresh();
    mvprintw(8, MAX_X / 3-22, "/____/ .___/\\__,_/\\___/\\___/  /_____/\\___/_/  \\___/_/ /_/\\__,_/\\___/_/     \n");
    printBorder(MAX_Y, MAX_X);
    usleep(500000);
    refresh();
    mvprintw(9, MAX_X / 3-22, "    /_/                                                                    \n");
    printBorder(MAX_Y, MAX_X);
    usleep(500000);
    refresh();

    sleep(1);

    refresh();

    /* Stampa carattere per carattere della intro string */
    for(i = 0; i < 20; i++){
        mvaddch(15, MAX_X / 2 - 10 + i, introString[i]);
        printBorder(MAX_Y, MAX_X);
        refresh();
        usleep(75000);
    }

    /* Effetto "lampeggio" della intro string */
    do{
        timeout(1);

        char keyPressed = getch();

        switch(keyPressed){
            case ENTER:
                flag = false;
                break;
          
            default:
                break;
        }

        mvprintw(15, MAX_X / 2 - 10, "                     ");
        printBorder(MAX_Y, MAX_X);
        refresh();
        usleep(500000);
        mvprintw(15, MAX_X / 2 - 10,"%s", introString);
        printBorder(MAX_Y, MAX_X);
        refresh();
        usleep(500000);
        
        mvprintw(4, MAX_X / 3-22, "   _____                          ____       ____               __         \n");
        mvprintw(5, MAX_X / 3-22, "  / ___/____  ____ _________     / __ \\___  / __/__  ____  ____/ /__  _____\n");
        mvprintw(6, MAX_X / 3-22, "  \\__ \\/ __ \\/ __ `/ ___/ _ \\   / / / / _ \\/ /_/ _ \\/ __ \\/ __  / _ \\/ ___/\n");
        mvprintw(7, MAX_X / 3-22, " ___/ / /_/ / /_/ / /__/  __/  / /_/ /  __/ __/  __/ / / / /_/ /  __/ /    \n");
        mvprintw(8, MAX_X / 3-22, "/____/ .___/\\__,_/\\___/\\___/  /_____/\\___/_/  \\___/_/ /_/\\__,_/\\___/_/     \n");
        mvprintw(9, MAX_X / 3-22, "    /_/                                                                    \n");
        
        refresh();
     }while(flag); 
}

/**
 * Funzione per la scelta della difficoltà del gioco, in base al valore scelto
 * verranno creati un numero di alieni diversi con velocità diverse. La funzione
 * simula una scelta interattiva, in cui l'utente premendo i tasti freccia su e
 * freccia giù della tastiera, si sposta tra le varie voci del menù a schermo,
 * selezionando la voce desiderata premendo il tasto invio sulla tastiera.
 * @return int restituisce il livello scelto
 */
Difficulty chooseDifficulty(){
    int y = MAX_Y / 2 - 8;
    Difficulty difficulty;
    int prevY = y;
    bool flag = true;

    do{
        /* Movimento del cursore */
        char keyPressed = getch();

        switch (keyPressed){
            case UP_KEY:
                if(y > MAX_Y / 2 - 8)
                    y -= 5;
                break;
        
            case DOWN_KEY:
                if(y < MAX_Y / 2 + 5)
                    y += 5;
                break;

            case ENTER:
                if(y == MAX_Y / 2 - 8)
                    difficulty = EASY;

                else if(y == MAX_Y / 2 - 3)
                    difficulty = MEDIUM;

                else if(y == MAX_Y / 2 + 2)
                    difficulty = HARD;
                    
                else
                    difficulty = CUSTOM;

                flag = false;
                break;

            default:
                break;
        }

        printBorder(MAX_Y, MAX_X);

        /* Stampa dei livelli di difficoltà in colonna */
        mvprintw(MAX_Y / 2 - 8, MAX_X / 2, "Easy");
        mvprintw(MAX_Y / 2 - 3, MAX_X / 2, "Medium");
        mvprintw(MAX_Y / 2 + 2, MAX_X / 2, "Hard");
        mvprintw(MAX_Y / 2 + 7, MAX_X / 2, "Custom");

        /* Stampa del cursore */
        mvaddch(y, MAX_X / 2 - 5, '>');

        /* Se il cursore è stato mosso, cancello la sua vecchia posizione */
        if(y != prevY)
          mvaddch(prevY, MAX_X / 2 - 5, ' ');

        refresh();

        prevY = y;
      }while(flag);

    attroff(COLOR_PAIR(TEXT_COLOR));
    /* Terminazione della musica di intro */
    system(KILL_SOUND);

    return difficulty;
}

/**
 * Procedura per il "produttore". Prende come parametro un oggetto di tipo
 * objectData e lo inserisce in un buffer circolare. L'accesso al buffer è
 * gestito tramite un semaforo contatore e un mutex. Il semaforo contatore 
 * conta il numero di posti liberi nel buffer, se i posti liberi sono maggiori
 * di 0 continua l'esecuzione, altrimenti aspetta. Dopo che il semaforo da
 * accesso viene bloccata la sezione critica (se non precedentemente bloccata
 * da altri thread) e inserito l'elemento nel buffer. Alla fine viene effettuata
 * una sem_post al semaforo fullSlots, per incrementare il numero di posti occupati
 * nel buffer.
 * @param object oggetto da inserire nel buffer
 */
void produce(objectData object) {
    sem_wait(&emptySlots);
    pthread_mutex_lock(&bufferMutex);
        buffer[in] = object;
        in = (in + 1) % DIM_BUFFER;
    pthread_mutex_unlock(&bufferMutex);
    sem_post(&fullSlots);
}

/**
 * Procedura per il "consumatore". Prende un oggetto dal buffer circolare e lo 
 * inserisce in una variabile globale di appoggio, che verrà usata dal gameManager.
 * L'accesso al buffer è gestito tramite un semafoto contatore e un mutex. Il semaforo
 * contatore conta il numero di posti pieni nel buffer, se i posti pieni sono maggiori
 * di 0 continua l'esecuzione, altrimenti aspetta. Dopo che il semaforo da accesso
 * viene bloccata la sezione critica (se non precedentemente bloccata da altri
 * thread) e prelevato l'elemento dal buffer. Alla fine viene effettuata una sem_pos
 * al semaforo emptySlots, per incrementare il numero di posti liberi nel buffer.
 */
void consume() {
    sem_wait(&fullSlots);
    pthread_mutex_lock(&bufferMutex);
        consumedProduct = buffer[out];
        out = (out + 1) % DIM_BUFFER;
    pthread_mutex_unlock(&bufferMutex);
    sem_post(&emptySlots);
}

/**
 * Procedura per la pulizia di ogni elemento del buffer da elementi indesiderati.
 */
void clean_buffer() {
    int i;

    for(i = 0; i < DIM_BUFFER; i++) {
        buffer[i].coords.x = -1;
        buffer[i].coords.y = -1;
        buffer[i].lives = -1;
        buffer[i].object_id = -1;
        buffer[i].object_type = -1;
        buffer[i].objectTid = -1;
        buffer[i].alive = true;
    }
}

/* Procedura che setta le posizioni in colonna per gli alieni, posizionando 
   4 alieni per colonna
*/
void setAlienSpawnPos(int *posX, int *posY, int *alienSpacingX, int alienSpacingY, int i) {
    /* Se stiamo generando le posizioni del primo alieno, lasciamo la y = 1 e impostiamo
       l'asse x
    */
    if(i == 0)
        *posX = MAX_X - *alienSpacingX;
    /* Altrimenti ogni 4 alieni cambia la colonna */
    else if(i % 4 != 0) {
        *posX = MAX_X - *alienSpacingX;
        *posY += alienSpacingY;
    } 
    else {
        *alienSpacingX += 10;
        *posX = MAX_X - *alienSpacingX;
        *posY = 1;
    }  
}

/* Funzione di supporto per generare valori random per l'esplosione */
double prng() {
  static long long s=1;
  s = s * 1488248101 + 981577151;
  return ((s % 65536) - 32768) / 32768.0;
}

/* Procedura per stampare l'esplosione iniziale
   Fonte: https://codegolf.stackexchange.com/questions/24462/display-the-explosion-of-a-star-in-ascii-art
*/
void kaboom() {

    char *frames[NUM_FRAMES], *p;
    int i,j,x,y,z,v,rows,cols,ith,i0;
    int maxx,minx,maxy,miny,delay=10000;
    double bx,by,bz,br,r,th,t;
    coord *randomCoords;

    cols = 91;
    rows = 25;

    // Valori preimpostati per la finestra del terminale
    minx = -cols / 2;
    maxx = cols+minx-1;
    miny = -rows / 2;
    maxy = rows+miny-1;


    // Genera coordinate random
    randomCoords = malloc(NUM_RANDOMCOORDS * sizeof(coord));
    for (i=0; i<NUM_RANDOMCOORDS; i++) {
    bx = prng();
    by = prng();
    bz = prng();
    br = sqrt(bx*bx + by*by + bz*bz);
    randomCoords[i].x = (bx / br) * (1.3 + 0.2 * prng());
    randomCoords[i].y = (0.5 * by / br) * (1.3 + 0.2 * prng());;
    randomCoords[i].z = (bz / br) * (1.3 + 0.2 * prng());;
  }
  
  // Generazione animazione con i frame
  for (i=0; i<NUM_FRAMES; i++) {
    t = (1. * i) / NUM_FRAMES;
    p = frames[i] = malloc(cols * rows + 1);
    for (y=miny; y<=maxy; y++) {
      for (x=minx; x<=maxx; x++) {
        
        // '*' nel primo frame
        if (i==0) {
          *p++ = (x==0 && y==0) ? '*' : ' ';
          continue;
        }
        
        // Stella crescente nei 7 prossimi frame
        if (i<8) {
          r = sqrt(x*x + 4*y*y);
          *p++ = (r < i*2) ? '@' : ' ';
          continue;
        }
        
        // E poi l'esplosione della stella
        r = sqrt(x*x + 4*y*y) * (0.5 + (prng()/3.0)*cos(16.*atan2(y*2.+0.01,x+0.01))*.3);
        ith = 32 + th * 32. * M_1_PI;
        v = i - r - 7;
        if (v<0) *p++ = (i<19)?"%@W#H=+~-:."[i-8]:' ';
        else if (v<20) *p++ = " .:!HIOMW#%$&@08O=+-"[v];
        else *p++=' ';
      }
    }
    
    // Terminazione
    *p = '\0';
  }
  
  // Stampa di tutta l'animazione sotto forma di string
  curs_set(0);
  for (i=0; i<NUM_FRAMES; i++) {
    erase();
    attron(COLOR_PAIR(rand() % 7));
    mvaddstr(0,0,frames[i]);
    refresh();
    usleep(60000); // Velocità esplosione
  }
}

/* Funzioni utili all'ordinamento dei record del file di testo, ordina in base al punteggio, a parità di punteggio si 
   confronta la durata della partita:
   Algoritmo di ordinamento Selection Sort
   Funzione di comparazione degli elementi del record
   Procedura per lo scambio degli elementi nel Selection Sort
*/
void selectionSort(Record *array, int size){
    int i, j, min;

    for(i = 0; i <= size - 2; i++){
        min = i;
        for(j = i + 1; j <= size - 1; j++){
            if (compare(array[j], array[min]) == 0)
                min = j;
        }
        swap(&array[i], &array[min]);
    }
}

int compare(Record r1, Record r2){
    if (r1.score > r2.score)
        return 0;

    else if (r1.score == r2.score && r1.sec < r2.sec)
        return 0;

    else
        return 1;
}

void swap(Record *r1, Record *r2){
    Record temp;

    temp = *r1;
    *r1 = *r2;
    *r2 = temp;
}

/**
 * Funzione per l'apertura del file di testo contenente i dati di tutti i giocatori.
 * @param fileName nome del file da aprire
 * @param mode modalità di apertura del file
 * @return FILE* puntatore al file aperto
 */
FILE* openFile(char *fileName, char *mode){
    FILE *fp = fopen(fileName, mode);

    if(fp == NULL){
        perror("Error opening file");
        exit(1);
    }

    return fp;
}

/**
 * Funzione per la chiusura del file di testo contenente i dati di tutti i giocatori.
 * @param fp puntatore al file da chiudere
 * @return FILE* 
 */
FILE* closeFile(FILE *fp){
    if(fp != NULL)
        fclose(fp);

    return NULL;
}