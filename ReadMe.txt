Progetto SO (Sistemi Operativi)

Autori:

-> Argiolas Diego 60/61/66047
-> Varchetta Gabriele 60/61/65976

How to build & play:

-> How to build:
Entrare manualmente da terminale dentro la cartella PROGETTO_SO e selezionare una delle due versioni
disponibili:
	
	Versione Processi
	Versione Threads

Dopo essersi recati nella cartella desiderata eseguire il comando "make" per compilare, e successivamente
scrivere ./game, se si vogliono effettuare delle modifiche, assicurarsi di eseguire il comando "make clean"
e successivamente make, seguito da ./game. Assicurarsi che il file "record.txt" contenga solo uno 0 nella prima
riga, quindi che non vi sia nessun record presente.

All'interno della cartella è presente anche un file "log_pipe.txt" in cui vengono reindirizzati gli output
di alcuni comandi.

-> How to play:

Impostare la dimensione della finestra del terminale a 91x25.

Una volta premuto enter ci sarà una schermata per la scelta della difficoltà, scegliere la difficoltà con
le frecce direzionali su e giu', e poi premere invio per selezionare la difficoltà desiderata.
Dopo aver selezionato la difficoltà comincerà il gioco, premere le frecce direzionali su e giu' per muovere
la navicella e premere spazio per sparare i missili. Il gioco termina quando vengono uccisi tutti gli alieni,
quando muore la navicella (3 vite), o quando un alieno supera la colonna della navicella.

Struttura:

-> Versione processi: l'architettura è basata su N produttori e un consumatore, i produttori scrivono le loro
informazioni aggiornate su una pipe, che verranno lette dal consumatore.

-> Versione threads: l'architettura è basata su N produttori e un consumatore, i produttori inseriscono le loro
informazioni aggiornate su un buffer condiviso, che verranno acquisite dal consumatore.

Problemi:

Il gioco presenta due tipi di bug estetici:

	1) in entrambe le versioni, quando l'alieno arriva nella stessa colonna (x) del player, non viene stampato 
	   l'ultimo movimento dell'alieno verso la parte sinistra del terminale, perchè appunto il gioco termina 
	   prima che questo movimento venga stampato.

	2) nella versione threads, quando si aumenta il numero di CPU della VM, si verificano dei bug estetici,
           stampe di caratteri random nel terminale.

Nuove funzionalità:

	1) Musica intro e soundtrack durante il gameplay (WARNING: abbassare il volume della VM a 25/30).
	2) Stampa iniziale del titolo con animazioni.
	3) Menu' interattivo per la scelta della difficoltà (easy, medium, hard, custom).
	4) Suoni custom per i missili, le bombe, le collisioni, e infine le esplosioni (alieni o navicella).
	5) Leaderboard dei top 10 giocatori.
	6) Stampa di punteggio e vite.
	






















	 