
check: test000 test001 test002
	#./test000
	@./test001 > test001.actual
	@diff -du test001.actual test/test001.expected
	@echo test001 SUCCESS
	@./test002 > test002.actual
	@diff -du test002.actual test/test002.expected
	@echo test002 SUCCESS

test000: test/test000a.cpp test/test000b.cpp upp11.h
	gcc -std=c++11 -o test000 -I. test/test000a.cpp test/test000b.cpp -lstdc++

test001: test/test001.cpp upp11.h
	gcc -std=c++11 -o test001 -I. test/test001.cpp -lstdc++

test002: test/test002.cpp upp11.h
	gcc -std=c++11 -o test002 -I. test/test002.cpp -lstdc++

clean:
	rm test000
