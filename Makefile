# Round robin makefiles

RR_I_UTILS = -I./src/utils
RR_SRC = ./src/main.c

rr:
	gcc $(RR_I_UTILS) -Wall $(RR_SRC) -o ./bin/rr

clean:
	rm -rf bin/main
