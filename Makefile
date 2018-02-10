CC = g++  #-pg # -g

NAME = polaris

SOMAJOR = 1
SOMINOR = 0
LIBNAME = lib$(NAME).so.${SOMAJOR}.${SOMINOR}

PREFIX = /usr/local

CPP_FILES = $(wildcard src/*.cpp)
OBJ_FILES = $(patsubst %.cpp, %.o, $(CPP_FILES))

CFLAGS = -Wall -std=c++0x -I./include
LD_FLAGS = -lpthread -ldl

all : $(NAME) lib

$(NAME) : ./exe/$(NAME).o $(OBJ_FILES)
	@echo "linking $@"
	@$(CC) -o $@ $^ $(LD_FLAGS) 


./exe/$(NAME).o : ./exe/$(NAME).cpp
	@echo "compiling $@"
	@$(CC) $(CFLAGS) -c $^ -o $@


lib : ./lib/$(LIBNAME)
./lib/$(LIBNAME) : ${OBJ_FILES}
	@mkdir -p ./lib
	@echo "linking $@"
	@$(CC) -fPIC -shared -Wl,-soname,lib${NAME}.so.${SOMAJOR} ${OBJ_FILES} -o $@ $(LD_FLAGS) 


%.o : %.cpp
	@echo "compiling $@"
	@$(CC) -fPIC $(CFLAGS) -c $^ -o $@


install:
	@echo "installing..."
	@mkdir -p ${PREFIX}/include/$(NAME)
	@cp ./include/*.h ${PREFIX}/include/$(NAME)
	@cp ./lib/$(LIBNAME) ${PREFIX}/lib/
	@cp ./${NAME} $(PREFIX)/bin/
	@ln -sf ${PREFIX}/lib/lib${NAME}.so.${SOMAJOR} ${PREFIX}/lib/lib${NAME}.so
	@sudo ldconfig -n ${PREFIX}/lib/lib${NAME}.so


uninstall:
	rm -rf ${PREFIX}/include/${NAME}
	rm ${PREFIX}/lib/$(LIBNAME)
	rm ${PREFIX}/bin/$(NAME)


clean:
	@echo "cleaning..."
	@-rm ${OBJ_FILES} > /dev/null 2>&1
	@-rm ./exe/${NAME}.o > /dev/null 2>&1
	@-rm ./lib/${LIBNAME} > /dev/null 2>&1
	@-rm $(NAME) > /dev/null 2>&1

