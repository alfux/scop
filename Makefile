#Scop 42-project Makefile by @lfux

DSRC	:= ./src

DHDR	:= ./hdr

DOBJ	:= ./obj

SRC		:= main.cpp Error.cpp SDL2pp.cpp

HDR		:= main.hpp Error.hpp SDL2pp.hpp

OBJ		:= $(SRC:%.cpp=$(DOBJ)/%.o)

SRC		:= $(SRC:%.cpp=$(DSRC)/%.cpp)

HDR		:= $(HDR:%.hpp=$(DHDR)/%.hpp)

SDLL	:= $(shell sdl2-config --libs)

SDLI	:= $(shell sdl2-config --cflags)

CFLAGS	+= -Wall -Wextra -Werror -Wshadow -g -std=c++2b -I$(DHDR)

CC		:= c++

NAME	:= scop

all			:	$(NAME)

$(NAME)		:	$(OBJ)
				$(CC) $(CFLAGS) $(SDLL) $^ -o $@

$(DOBJ)/%.o	:	$(DSRC)/%.cpp $(DHDR)/%.hpp | $(DOBJ)
				$(CC) $(CFLAGS) $(SDLI) -c $< -o $@

$(DOBJ)		:
				mkdir $@

clean		:
				rm -rf $(DOBJ)

fclean		:	clean
				rm -rf $(NAME)

re			:	fclean all

.PHONY		:	all clean fclean re
