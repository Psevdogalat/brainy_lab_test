INCLUDE 		= include
OBJ				= objects

ENGINE_INCLUDE 	= engine/include
ENGINE_OBJ		= engine/objects/engine.o

INCLUDE_FLAGS 	= -I$(INCLUDE) -I$(ENGINE_INCLUDE)
LIBS_FLAGS		= -static -lstdc++ -lgdi32 -lopengl32
OPTKEYS			= -o0
#DBGFLG			= -Wl,--strip-all
DBGFLG			= -g

.PHONY: all clean $(ENGINE_OBJ)

all: game.exe

clean:
	cd engine; make clean
	rm $(OBJ)/*.o
	rm *.exe

$(OBJ)/game_module.o: game_module.cpp
	mkdir -p $(OBJ)
	gcc -x c++ $(DBGFLG) $(OPTKEYS) $< $(INCLUDE_FLAGS) -c -o $@
	
$(OBJ)/test_module.o: test_module.cpp
	mkdir -p $(OBJ)
	gcc -x c++ $(DBGFLG) $(OPTKEYS) $< $(INCLUDE_FLAGS) -c -o $@
	
$(OBJ)/gjk_test.o: gjk_test.cpp
	mkdir -p $(OBJ)
	gcc -x c++ $(DBGFLG) $(OPTKEYS) $< $(INCLUDE_FLAGS) -c -o $@

$(OBJ)/objects.o: objects.cpp  $(INCLUDE)/objects.h
	mkdir -p $(OBJ)
	gcc -x c++ $(DBGFLG) $(OPTKEYS) $< $(INCLUDE_FLAGS) -c -o $@
	
$(OBJ)/vector_utilits.o: vector_utilits.cpp  $(INCLUDE)/vector_utilits.h
	mkdir -p $(OBJ)
	gcc -x c++ $(DBGFLG) $(OPTKEYS) $< $(INCLUDE_FLAGS) -c -o $@	
	
$(OBJ)/utilits.o: utilits.cpp  $(INCLUDE)/utilits.h
	mkdir -p $(OBJ)
	gcc -x c++ $(DBGFLG) $(OPTKEYS) $< $(INCLUDE_FLAGS) -c -o $@	
	
$(OBJ)/keys.o: keys.cpp  $(INCLUDE)/keys.h
	mkdir -p $(OBJ)
	gcc -x c++ $(DBGFLG) $(OPTKEYS) $< $(INCLUDE_FLAGS) -c -o $@	

$(OBJ)/controllers.o: controllers.cpp  $(INCLUDE)/controllers.h
	mkdir -p $(OBJ)
	gcc -x c++ $(DBGFLG) $(OPTKEYS) $< $(INCLUDE_FLAGS) -c -o $@	

$(OBJ)/collisions2.o: collisions2.cpp  $(INCLUDE)/collisions2.h
	mkdir -p $(OBJ)
	gcc -x c++ $(DBGFLG) $(OPTKEYS) $< $(INCLUDE_FLAGS) -c -o $@	

$(ENGINE_OBJ): 
	cd engine; make

game.exe:  $(OBJ)/objects.o $(OBJ)/game_module.o $(OBJ)/vector_utilits.o $(OBJ)/utilits.o $(OBJ)/keys.o $(OBJ)/controllers.o $(ENGINE_OBJ)
	gcc  $^ $(LIBS_FLAGS) -o $@
	
test.exe: $(OBJ)/objects.o $(OBJ)/test_module.o $(OBJ)/vector_utilits.o $(OBJ)/utilits.o $(OBJ)/keys.o $(OBJ)/controllers.o $(OBJ)/collisions2.o $(ENGINE_OBJ) 
	gcc $(DBGFLG) $^ $(LIBS_FLAGS) -o $@


gjk_test.exe: $(OBJ)/objects.o $(OBJ)/gjk_test.o $(OBJ)/vector_utilits.o $(OBJ)/utilits.o $(OBJ)/keys.o $(OBJ)/controllers.o $(OBJ)/collisions2.o $(ENGINE_OBJ) 
	gcc $(DBGFLG) $^ $(LIBS_FLAGS) -o $@
