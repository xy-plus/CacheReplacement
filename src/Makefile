CXX = g++
CFLAG = #-g -O2 -std=c++0x -Wno-deprecated  #-Wall
LIBS  = #-lm -lstdc++ 
EXCUTABLE_FILE = test_main.out

# TEST_SUBDIR=./test_subdir/
# SOURCE=$(wildcard ./*cpp $(TEST_SUBDIR)*.cpp)
SOURCE=$(wildcard ./*cpp)
INCLUDE = #-I ./test_subdir/ -I ./

OBJECTS=$(patsubst %.cpp,%.o,$(SOURCE))
$(EXCUTABLE_FILE): $(OBJECTS)
	$(CXX) $(INCLUDE) $(CFLAG) $^ -o $@ $(LIBS)
$(OBJECTS):%.o : %.cpp
	$(CXX) $(INCLUDE) -c $(CFLAG) $< -o $@ $(LIBS)

.PHONY:clean
clean:
	rm -rf $(OBJECTS) $(EXCUTABLE_FILE)
