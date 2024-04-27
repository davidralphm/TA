CC := gcc
CFLAGS := -c -O0 -Wall

INCPATH := -I./include -IC:/SDL2/include -IC:/SDL2_Image/include -IC:/SDL2_Mixer_New/include
LIBPATH := -LC:/SDL2/lib -LC:/SDL2_Image/lib -LC:/SDL2_Mixer_New/lib

LIBS := -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer

OBJ :=	\
		./obj/button.o								\
		./obj/enemy.o								\
		./obj/enemyspawner.o						\
		./obj/decoration.o							\
		./obj/fireball.o							\
		./obj/font.o								\
		./obj/game.o								\
		./obj/item.o								\
		./obj/main.o								\
		./obj/map.o									\
		./obj/minimap.o								\
		./obj/player.o								\
		./obj/raycaster.o							\
		./obj/sounds.o								\
		./obj/sprite.o								\
		./obj/teleportexit.o						\
		./obj/textures.o							\
		./obj/ui.o									\

./obj/%.o: ./src/%.c
	$(CC) $(CFLAGS) $< -o $@ $(INCPATH)

main.exe: $(OBJ)
	$(CC) $(OBJ) -o $@ $(LIBPATH) $(LIBS)

clean:
	rm ./obj/*.o