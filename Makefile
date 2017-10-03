CC = g++ # -g

NAME = polaris
EXENAME = polaris
LIBNAME = lib$(NAME).so

PREFIX = /usr/local

CPP_FILES = $(wildcard src/*.cpp)
OBJ_FILES = $(patsubst %.cpp, %.o, $(CPP_FILES))

CFLAGS = -Wall -std=c++0x -fPIC -I./include
LD_FLAGS = -lpthread -ldl

# LD_PLOT = -lplot -lXaw -lXmu -lXt -lXext -lX11 -lpng -lz -lm

#CAEN_FLAGS = -I/usr/include/CAEN/
#LD_CAEN = $(INCLUDE_CAEN) -lCAENVME

all : $(EXENAME) lib

$(EXENAME) : ./exe/$(EXENAME).o $(OBJ_FILES)
#	@$(CC) -L./lib -l$(NAME) $(LD_FLAGS) $(LD_PLOT) $< -o $@
	@echo "linking $@"
	@$(CC) $^ $(LD_FLAGS) $(LD_PLOT) $(LD_CAEN) -o $@


./exe/$(EXENAME).o : ./exe/$(EXENAME).cpp
	@echo "compiling $@"
	@$(CC) $(CFLAGS) -c $^ -o $@

lib : ./lib/$(LIBNAME)
	@mkdir -p ./lib

./lib/$(LIBNAME) : $(OBJ_FILES)
	@echo "linking $@"
	@$(CC) -fPIC -shared -Wl,-soname,$(LIBNAME) $(LD_FLAGS) $(LD_PLOT) $(LD_CAEN) $^ -o $@

%.o : %.cpp
	@echo "compiling $@"
	@$(CC) $(CFLAGS) $(CAEN_FLAGS) -c $^ -o $@

install:
	@mkdir -p ${PREFIX}/include/$(NAME)
	@cp ./include/*.h ${PREFIX}/include/$(NAME)
	@cp ./lib/$(LIBNAME) ${PREFIX}/lib/
	@cp ./${NAME} $(PREFIX)/bin/

uninstall:
	rm -rf ${PREFIX}/include/${NAME}
	rm ${PREFIX}/lib/$(LIBNAME)
	rm ${PREFIX}/bin/$(NAME)

clean:
	@echo "cleaning..."
	@-rm ${OBJ_FILES} > /dev/null 2>&1
	@-rm ./lib/*.so > /dev/null 2>&1
	@-rm $(EXENAME) > /dev/null 2>&1

