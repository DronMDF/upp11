
#pragma once
#include <functional>
#include <iostream>
#include <list>

namespace upp11 {

class TestCollection {
private:
	std::list<std::function<void ()>> tests;

	static TestCollection &getInstance() {
		static TestCollection collection;
		return collection;
	};

public:
	static void addTest(std::function<void ()> test)
	{
		TestCollection &collection = getInstance();
		collection.tests.push_back(test);
	}

	static void runAllTests()
	{
		TestCollection &collection = getInstance();
		for (auto t: collection.tests) {
			t();
		}
	}
};

template <typename T>
class TestInvokerImpl {
private:
	void invoke() {
		T test;
		test();
	}
public:
	TestInvokerImpl() {
		TestCollection::addTest(std::bind(&TestInvokerImpl::invoke, this));
	}
};

} // end of namespace upp11

#define UP_RUN() \
	upp11::TestCollection::runAllTests()

#define UP_TEST(name) \
class Test##name { \
public: \
	void operator()(); \
}; \
static upp11::TestInvokerImpl<Test##name> test##name##invoker; \
void Test##name::operator()()

#define UP_FAIL(msg) \
	std::cout << msg << std::endl;
