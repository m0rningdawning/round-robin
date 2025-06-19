all:
	sudo gcc -Wall src/main.c -o bin/main

clean:
	rm -rf bin/main