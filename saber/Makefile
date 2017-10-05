# Makefile for compiing saber library for data acquisition with polaris

CC = g++ # -g

NAME = saber
LIBNAME = lib$(NAME).so

PREFIX = /usr/local


CPP_FILES = $(wildcard src/*.cpp)
OBJ_FILES = $(patsubst %.cpp, %.o, $(CPP_FILES))


CFLAGS = -Wall -std=c++0x -fPIC -I./include


PLRS_FLAGS = -I/usr/local/include/polaris


CAEN_FLAGS = -I/usr/include/CAEN
LD_CAEN = -lCAENVME



all : lib

lib : ./lib/$(LIBNAME)

./lib/$(LIBNAME) : $(OBJ_FILES)
	@echo "linking $@"
	@mkdir -p ./lib
	@$(CC) -fPIC -shared -Wl,-soname,$(LIBNAME) $^ -o $@ $(LD_CAEN) 

%.o : %.cpp
	@echo "compiling $@"
	@$(CC) $(CFLAGS) $(PLRS_FLAGS) $(CAEN_FLAGS) -c $^ -o $@

clean:
	@echo "cleaning..."
	@-rm ${OBJ_FILES} > /dev/null 2>&1
	@-rm ./lib/${LIBNAME} > /dev/null 2>&1
