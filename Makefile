CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
LDFLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf

main: main.cpp
	$(CXX) src/main.cpp -o main $(CXXFLAGS) $(LDFLAGS)

run: main
	./main

clean:
	rm -f main
