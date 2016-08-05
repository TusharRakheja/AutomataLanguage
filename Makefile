# Makes the interpreter for Autolang Version 2.

# MACROS

CXX = g++
SOURCE = Files/Source\ Files/
HEADER = Files/Header\ Files/
FLAGS = -std=c++14 -w
OBJS = Interpreter.o Auto.o ExpressionTree.o Map.o Set.o Tuple.o AbstractMap.o AbstractSet.o ProgramVars.o

# TARGETS

auto : $(OBJS)
	$(CXX) -o auto $(OBJS)
	
AbstractMap.o : $(SOURCE)AbstractMap.cpp $(HEADER)AbstractMap.h
	$(CXX) -c $(FLAGS) $(SOURCE)AbstractMap.cpp 

AbstractSet.o : $(SOURCE)AbstractSet.cpp $(HEADER)AbstractSet.h
	$(CXX) -c $(FLAGS) $(SOURCE)AbstractSet.cpp
	
Set.o : $(SOURCE)Set.cpp $(HEADER)Set.h $(HEADER)ExpressionTree.h 
	$(CXX) -c $(FLAGS) $(SOURCE)Set.cpp

Tuple.o : $(SOURCE)Tuple.cpp $(HEADER)Tuple.h $(HEADER)Set.h $(HEADER)ExpressionTree.h
	$(CXX) -c $(FLAGS) $(SOURCE)Tuple.cpp

Map.o : $(SOURCE)Map.cpp $(HEADER)Map.h
	$(CXX) -c $(FLAGS) $(SOURCE)Map.cpp

Auto.o : $(SOURCE)Auto.cpp $(HEADER)Auto.h
	$(CXX) -c $(FLAGS) $(SOURCE)Auto.cpp

ProgramVars.o : $(SOURCE)ProgramVars.cpp $(HEADER)ProgramVars.h
	$(CXX) -c $(FLAGS) $(SOURCE)ProgramVars.cpp

ExpressionTree.o : $(SOURCE)ExpressionTree.cpp $(HEADER)ExpressionTree.h
	$(CXX) -c $(FLAGS) $(SOURCE)ExpressionTree.cpp

Interpreter.o : $(SOURCE)Interpreter.cpp $(HEADER)ExpressionTree.h
	$(CXX) -c $(FLAGS) $(SOURCE)Interpreter.cpp

clean : 
	rm $(OBJS)
