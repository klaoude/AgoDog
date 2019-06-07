CC = gcc
CFLAGS = -std=c99 -g `sdl2-config --cflags`
LDFLAGS = -lpthread -lm -lwebsockets `sdl2-config --libs` -lSDL2_gfx -lSDL2_ttf
EXEC = AgoBot
TEST = Test

all : $(EXEC)

test : $(TEST)

$(EXEC) : main.o UI.o IA.o WS.o Utils.o
	$(CC) -o $(EXEC) *.o $(LDFLAGS)

$(TEST) : unitTest.o Utils.o
	$(CC) -o $(TEST) unitTest.o Utils.o $(LDFLAGS)

unitTest.o : unitTest.c
	$(CC) -o $@ -c $< $(CFLAGS)

main.o : main.c
	$(CC) -o $@ -c $< $(CFLAGS)

UI.o : UI.c	
	$(CC) -o $@ -c $< $(CFLAGS)

IA.o : IA.c	
	$(CC) -o $@ -c $< $(CFLAGS)

Utils.o : Utils.c	
	$(CC) -o $@ -c $< $(CFLAGS)

WS.o : WS.c	
	$(CC) -o $@ -c $< $(CFLAGS)

clean :
	rm -rf *.o