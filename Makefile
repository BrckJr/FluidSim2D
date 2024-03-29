build: ./src/*.cpp
	# Include all files from the source directory and make sure the compiler translates to ARM64
	g++ -std=c++17 -c ./src/*.cpp -I /opt/homebrew/Cellar/sfml/2.6.1/include/ -L/opt/homebrew/Cellar/sfml/2.6.1/lib -lsfml-graphics -lsfml-window -lsfml-system -arch arm64
	
	mkdir -p obj && mv ./*.o ./obj
	g++ ./obj/*.o -o main -I /opt/homebrew/Cellar/sfml/2.6.1/include/ -L/opt/homebrew/Cellar/sfml/2.6.1/lib -lsfml-graphics -lsfml-window -lsfml-system -arch arm64
	chmod 755 ./main
	./main