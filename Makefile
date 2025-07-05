# Round robin makefiles

RR_I_UTILS = -I./src/utils -I./src/rr
RR_SRC = ./src/main.c ./src/rr/rr.c ./src/rr/proc.c ./src/rr/queue.c ./src/utils/utils.c ./src/utils/log.c ./src/utils/file.c ./src/utils/randomize.c

rr:
	gcc $(RR_I_UTILS) -g -Wall $(RR_SRC) -o ./bin/rr

clean:
	rm -rf bin/rr
