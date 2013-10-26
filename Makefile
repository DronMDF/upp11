
check: test000
	./test000

test000: test/test000a.cpp test/test000b.cpp
	gcc -std=c++11 -o test000 -I. $^ -lstdc++
