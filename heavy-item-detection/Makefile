EXEC += main_hitter main_changer 
all: $(EXEC)

CFLAGS = -Wall -std=c++11 -O3
HEADER += hash.h datatypes.hpp util.h adaptor.hpp 
SRC += hash.c adaptor.cpp
SKETCHHEADER += StableSketch.hpp
SKETCHSRC += StableSketch.cpp
LIBS= -lpcap 

main_changer: main_changer.cpp $(SRC) $(HEADER) $(SKETCHHEADER)
	g++ $(CFLAGS) $(INCLUDES) -o $@ $< $(SRC) $(SKETCHSRC) $(LIBS) -w

main_hitter: main_hitter.cpp $(SRC) $(HEADER) $(SKETCHHEADER) 
	g++ $(CFLAGS) $(INCLUDES) -o $@ $< $(SRC) $(SKETCHSRC) $(LIBS) -w

clean:
	rm -rf $(EXEC)
	rm -rf *log*
	rm -rf *out*
