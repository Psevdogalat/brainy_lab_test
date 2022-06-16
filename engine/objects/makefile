INCLUDE = ../include
SRC		= ..

ENGINE_OBJECTS = $(OBJ)/core.o $(OBJ)/objects.o $(OBJ)/geometry.o
OPTKEYS = -o0
DBGFLG	= -g

.PHONY: all clean

all: 	engine.o

clean:	
	rm *.o
	
windows.o: $(SRC)/windows.cpp
	gcc -x c++ $(DBGFLG) $(OPTKEYS) $< -I$(INCLUDE) -c -o $@

core.o: $(SRC)/core.cpp $(INCLUDE)/engine.h
	gcc -x c++ $(DBGFLG) $(OPTKEYS) $< -I$(INCLUDE) -c -o $@
	
objects.o: $(SRC)/objects.cpp $(INCLUDE)/engine.h
	gcc -x c++ $(DBGFLG) $(OPTKEYS) $< -I$(INCLUDE) -c -o $@	

geometry.o: $(SRC)/geometry.cpp $(INCLUDE)/geometry.h
	gcc -x c++ $(DBGFLG) $(OPTKEYS) $< -I$(INCLUDE) -c -o $@
	
physics.o: $(SRC)/physics.cpp $(INCLUDE)/physics.h
	gcc -x c++ $(DBGFLG) $(OPTKEYS) $< -I$(INCLUDE) -c -o $@

graphics.o: $(SRC)/graphics.cpp $(INCLUDE)/graphics.h
	gcc -x c++ $(DBGFLG) $(OPTKEYS) $< -I$(INCLUDE) -c -o $@
	
collisions.o: $(SRC)/collisions.cpp $(INCLUDE)/collisions.h
	gcc -x c++ $(DBGFLG) $(OPTKEYS) $< -I$(INCLUDE) -c -o $@

engine.o: core.o objects.o geometry.o graphics.o physics.o collisions.o windows.o
	ld -r -o $@ $^ 