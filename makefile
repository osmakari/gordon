bin/gordon: src/*.c src/*.h bin
	gcc src/*.c -lncursesw -o bin/gordon

bin:
	mkdir bin