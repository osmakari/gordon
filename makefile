bin/gordon: src/*.c src/*.h bin
	gcc src/*.c -lncurses -o bin/gordon

bin:
	mkdir bin