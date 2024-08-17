#OBJS specifies which files to compile as part of the project
OBJS = main.c

SOURCES := $(wildcard *.c) $(wildcard Engine/*.c) 

#CC specifies which compiler we're using
CC = emcc

#COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
COMPILER_FLAGS = -w

#INCLUDES
INCLUDES = -I./Sounds -I./Art -I./Engine -I./Fonts

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -s WASM=1 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["png"]' -s USE_SDL_TTF=2 -s USE_SDL_MIXER=2 -s SDL2_MIXER_FORMATS='["wav"]' --preload-file Art --preload-file Sounds 

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = play

#This is the target that compiles our executable
all : $(SOURCES)
	$(CC) $(SOURCES) $(COMPILER_FLAGS) $(INCLUDES) $(LINKER_FLAGS) -o $(OBJ_NAME)
