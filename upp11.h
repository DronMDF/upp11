
#pragma once
#include <list>
#include <iostream>

namespace upp11 {

class Test {
public:
	virtual void operator()() = 0;
};

class TestCollection {
	std::list<Test *> tests;

	static TestCollection &getInstance() {
		static TestCollection collection;
		return collection;
	};

public:
	static void addTest(class Test *test)
	{
		TestCollection &collection = getInstance();
		collection.tests.push_back(test);
	}

	static void runAllTests()
	{
		TestCollection &collection = getInstance();
		for (auto *t: collection.tests) {
			(*t)();
		}
	}
};

class TestRegistrar : public Test {
protected:
	TestRegistrar() {
		TestCollection::addTest(this); \
	}
};

} // end of namespace upp11

#define UP_RUN() \
	upp11::TestCollection::runAllTests()

#define UP_TEST(name) \
class Test##name : public upp11::TestRegistrar { \
	void operator()() override; \
}; \
static Test##name test##name##instance; \
void Test##name::operator()()

#define UP_FAIL(msg) \
	std::cout << msg << std::endl;
