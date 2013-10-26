
check: test000
	./test000

test000: test/test000a.cpp test/test000b.cpp upp11.h
	gcc -std=c++11 -o test000 -I. test/test000b.cpp test/test000b.cpp -lstdc++

clean:
	rm test000
