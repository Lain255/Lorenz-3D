LIBS	:= -lSDL2main -lSDL2 -lm
SOURCE	:= main.c
FLAGS	:= -O3 -Wall

run:
	gcc $(FLAGS) $(SOURCE) $(LIBS)
	./a.out
	rm a.out
