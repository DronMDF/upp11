
check: testupp testfailures
	@./testupp -q
	@echo Check units SUCCESS

	-@./testfailures -s 0 > testfailures.actual
	@diff -du test/testfailures.expected testfailures.actual
	@echo Check test failures SUCCESS

testupp: test/testupp.cpp test/testuppdetail.cpp test/testuppstatic.cpp upp11.h
	${CXX} -Wall -Wextra -Weffc++ -Werror -std=c++11 -o testupp -I. \
		test/testupp.cpp test/testuppdetail.cpp test/testuppstatic.cpp -lstdc++

testfailures: test/testfailures.cpp upp11.h
	${CXX} -Wall -Wextra -Weffc++ -Werror -std=c++11 -o testfailures -I. \
		test/testfailures.cpp -lstdc++

clean:
	rm testupp
	rm testfailures
	rm testfailures.actual
