game: main.o player.o enemy.o gameManager.o util.o
	gcc main.o player.o enemy.o gameManager.o util.o -o game -lncurses -lm

main.o: main.c player.c enemy.c gameManager.c util.c
	gcc -c main.c player.c enemy.c gameManager.c util.c

player.o: player.c player.h util.h
	gcc -c player.c

enemy.o: enemy.c enemy.h util.h
	gcc -c enemy.c

gameManager.o: gameManager.c gameManager.h util.h
	gcc -c gameManager.c

util.o: util.c util.h
	gcc -c util.c

clean: 
	rm game
	rm -f *.o