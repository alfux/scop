#Scop 42-project Makefile by @lfux

DSRC	:= ./src

DHDR	:= ./hdr

DOBJ	:= ./obj

DSHADER	:= ./shaders

SRC		:= main.cpp Error.cpp SDL2pp.cpp Scop.cpp

HDR		:= main.hpp Error.hpp SDL2pp.hpp Scop.hpp

OBJ		:= $(SRC:%.cpp=$(DOBJ)/%.o)

SHADERS	:= vert.spv frag.spv

SRC		:= $(SRC:%.cpp=$(DSRC)/%.cpp)

HDR		:= $(HDR:%.hpp=$(DHDR)/%.hpp)

SHADERS := $(SHADERS:%.spv=$(DSHADER)/%.spv)

SDLL	:= $(shell sdl2-config --libs)

SDLI	:= $(shell sdl2-config --cflags)

VULKAND	:= /Users/alfux/VulkanSDK/1.3.290.0/macOS

VULKANL := -L$(VULKAND)/lib -lvulkan -rpath $(VULKAND)/lib

VULKANI := -I$(VULKAND)/include

CFLAGS	+= -Wall -Wextra -Werror -g -std=c++2b -I$(DHDR)

CC		:= g++

NAME	:= scop

all			:	$(NAME) $(SHADERS)

$(NAME)		:	$(OBJ)
				$(CC) $(CFLAGS) $(SDLL) $(VULKANL) $^ -o $@

ifeq ($(debug), true)
$(DOBJ)/%.o	:	$(DSRC)/%.cpp $(DHDR)/%.hpp | $(DOBJ)
				$(CC) $(CFLAGS) $(SDLI) $(VULKANI) -c $< -o $@
else
$(DOBJ)/%.o	:	$(DSRC)/%.cpp $(DHDR)/%.hpp | $(DOBJ)
				$(CC) $(CFLAGS) -D NDEBUG $(SDLI) $(VULKANI) -c $< -o $@
endif

$(DSHADER)/%.spv : $(DSHADER)/shader.%
	$(VULKAND)/bin/glslc $< -o $@

$(DOBJ)		:
				mkdir $@

debug		:
				$(MAKE) debug=true

clean		:
				rm -rf $(DOBJ)

fclean		:	clean
				rm -rf $(NAME)
				rm -rf $(DSHADER)/*.spv

re			:	fclean all

redebug		:	fclean debug

.PHONY		:	all clean fclean re debug redebug
