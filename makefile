bin/gordon: src/*.c src/*.h
	gcc src/*.c -lncurses -o bin/gordon