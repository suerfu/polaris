# Makefile for compiing saber library for data acquisition with polaris

CC = g++ -pg # -g

NAME = random
LIBNAME = lib$(NAME).so


CPP_FILES = $(wildcard src/*.cpp)
OBJ_FILES = $(patsubst %.cpp, %.o, $(CPP_FILES))


CFLAGS = -Wall -std=c++0x -fPIC -I./include

ROOT_FLAGS = `root-config --cflags --glibs`
PLRS_FLAGS = -I/usr/local/include/polaris
LD_POLARIS = -L/usr/local/lib -lpolaris


all : lib

lib : ./lib/$(LIBNAME)

./lib/$(LIBNAME) : $(OBJ_FILES)
	@echo "linking $@"
	@mkdir -p ./lib
	@$(CC) -fPIC -shared -Wl,-soname,$(LIBNAME) -o $@ $^ ${LD_POLARIS} ${ROOT_FLAGS}

%.o : %.cpp
	@echo "compiling $@"
	@$(CC) $(CFLAGS) $(PLRS_FLAGS) -c $^ -o $@ ${ROOT_FLAGS}

clean:
	@echo "cleaning..."
	@-rm ${OBJ_FILES} > /dev/null 2>&1
	@-rm ./lib/${LIBNAME} > /dev/null 2>&1
