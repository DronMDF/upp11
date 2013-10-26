
#pragma once
#include <list>
#include <iostream>

namespace upp11 {

class TestInvoker {
public:
	virtual void invoke() = 0;
};

class TestCollection {
private:
	std::list<TestInvoker *> tests;

	static TestCollection &getInstance() {
		static TestCollection collection;
		return collection;
	};

public:
	static void addTest(class TestInvoker *test)
	{
		TestCollection &collection = getInstance();
		collection.tests.push_back(test);
	}

	static void runAllTests()
	{
		TestCollection &collection = getInstance();
		for (auto *t: collection.tests) {
			t->invoke();
		}
	}
};

template <typename T>
class TestInvokerImpl : public TestInvoker {
public:
	TestInvokerImpl() {
		TestCollection::addTest(this);
	}

private:
	void invoke() override {
		T test;
		test();
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
