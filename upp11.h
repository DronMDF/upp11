
#pragma once
#include <algorithm>
#include <cstdlib>
#include <chrono>
#include <functional>
#include <iostream>
#include <iterator>
#include <list>
#include <memory>
#include <sstream>
#include <vector>
#include <getopt.h>

namespace upp11 {

class TestCollection {
private:
	typedef std::pair<std::string, std::function<bool ()>> test_pair_t;
	std::vector<test_pair_t> tests;
	std::vector<std::string> suites;

	static TestCollection &getInstance() {
		static TestCollection collection;
		return collection;
	};

public:
	static void beginSuite(const std::string &name)
	{
		TestCollection &collection = getInstance();
		collection.suites.push_back(name);
	}

	static void endSuite()
	{
		TestCollection &collection = getInstance();
		collection.suites.pop_back();
	}

	static void addTest(const std::string &name, std::function<bool ()> test)
	{
		TestCollection &collection = getInstance();
		std::string path;
		for (auto s: collection.suites) {
			path += s + "/";
		}
		collection.tests.push_back(std::make_pair(path + name, test));
	}

	static bool runAllTests(unsigned seed, bool quiet, bool timestamp)
	{
		TestCollection &collection = getInstance();
		// Отсортируем все по именам, чтобы не зависело от порядка линковки
		std::sort(collection.tests.begin(), collection.tests.end(),
			[](const test_pair_t &A, const test_pair_t &B){ return A.first < B.first; });
		if (seed != 0) {
			if (!quiet) {
				std::cout << "random seed: " << seed << std::endl;
			}
			std::default_random_engine r(seed);
			std::shuffle(collection.tests.begin(), collection.tests.end(), r);
		}
		bool failure = false;
		for (auto t: collection.tests) {
			using namespace std::chrono;
			const high_resolution_clock::time_point st = high_resolution_clock::now();
			const bool success = t.second();
			const high_resolution_clock::time_point et = high_resolution_clock::now();
			const unsigned us = duration_cast<microseconds>(et - st).count();
			if (!quiet || !success) {
				std::cout << t.first;
				if (timestamp) {
					std::cout << " (" << us << "us)";
				}
				std::cout << ": " << (success ? "SUCCESS" : "FAIL") << std::endl;
			}
			failure |= !success;
		}
		return !failure;
	}
};

class TestSuiteBegin {
public:
	TestSuiteBegin(const std::string &name) {
		TestCollection::beginSuite(name);
	}
};

class TestSuiteEnd {
public:
	TestSuiteEnd() {
		TestCollection::endSuite();
	}
};

class TestException {};

template <typename T>
class TestInvoker {
public:
	bool invoke(std::function<void (T *)> test_function) const {
		std::shared_ptr<T> instance;
		try {
			instance = std::make_shared<T>();
		} catch (const TestException &) {
			return false;
		} catch (const std::exception &e) {
			std::cout << "exception from test ctor: " << e.what() << std::endl;
			return false;
		} catch (...) {
			std::cout << "unknown exception from test ctor" << std::endl;
			return false;
		}

		try {
			test_function(instance.get());
		} catch (const TestException &) {
			return false;
		} catch (const std::exception &e) {
			std::cout << "exception from test run: " << e.what() << std::endl;
			return false;
		} catch (...) {
			std::cout << "unknown exception from test run" << std::endl;
			return false;
		}

		return true;
	}
};

template <typename T>
class TestInvokerTrivial : public TestInvoker<T> {
private:
	bool invoke() {
		return TestInvoker<T>::invoke(std::bind(&T::run, std::placeholders::_1));
	}
public:
	TestInvokerTrivial(const std::string &name) {
		TestCollection::addTest(name, std::bind(&TestInvokerTrivial::invoke, this));
	}
};

template <typename T, typename ...V>
class TestInvokerParametrized : public TestInvoker<T> {
private:
	bool invoke(const std::tuple<V...> &params) {
		return TestInvoker<T>::invoke(std::bind(&T::run, std::placeholders::_1, params));
	}
public:
	TestInvokerParametrized(const std::string &name,
		const std::initializer_list<const std::tuple<V...>> &params)
	{
		for (const auto v: params) {
			TestCollection::addTest(name,
				std::bind(&TestInvokerParametrized::invoke, this, v));
		}
	}
};

template <typename T>
struct TestValue {
	const std::vector<T> value;
	const bool agregate;

	TestValue(const std::initializer_list<T> &iv)
		: value(iv.begin(), iv.end()), agregate(true) {}
	TestValue(const std::vector<T> &iv)
		: value(iv), agregate(true) {}
	TestValue(const std::list<T> &iv)
		: value(iv.begin(), iv.end()), agregate(true) {}
	template<std::size_t sz> TestValue(const std::array<T, sz> &iv)
		: value(iv.begin(), iv.end()), agregate(true) {}
	template<std::size_t sz> TestValue(T (&iv)[sz])
		: value(iv, iv + sz), agregate(true) {}
	TestValue(const T &iv)
		: value(1, iv), agregate(false) {}

	bool operator == (const TestValue<T> &b) const {
		return value == b.value && agregate == b.agregate;
	}
};

template <>
struct TestValue<const char *> {
	const std::vector<std::string> value;
	const bool agregate;

	TestValue(const std::initializer_list<const char *> &iv)
		: value(iv.begin(), iv.end()), agregate(true) {}
//	TestValue(const std::vector<const char *> &iv)
//		: value(iv.begin(), iv.end()), agregate(true) {}
//	TestValue(const std::list<const char *> &iv)
//		: value(iv.begin(), iv.end()), agregate(true) {}
//	template<std::size_t sz> TestValue(const std::array<const char *, sz> &iv)
//		: value(iv.begin(), iv.end()), agregate(true) {}
//	template<std::size_t sz> TestValue(const char (*iv)[sz])
//		: value(iv, iv + sz), agregate(true) {}
//	template<std::size_t sz> TestValue(const char (&iv)[sz])
//		: value(1, &iv), agregate(false) {}
	TestValue(const char *iv)
		: value(1, iv), agregate(false) {}

	bool operator == (const TestValue<std::string> &b) const {
		return value == b.value && agregate == b.agregate;
	}
	bool operator == (const TestValue<const char *> &b) const {
		return value == b.value && agregate == b.agregate;
	}
};

template <typename T>
std::ostream &operator << (std::ostream &os, const TestValue<T> &t)
{
	if (t.aggregate) { os << "{ "; }
	std::copy(t.value.begin(), t.value.end(), std::ostream_iterator<T>(os, ", "));
	if (t.aggregate) { os << " }"; }
	return os;
}

struct TestBase {
	template <typename T>
	bool isEqual(const TestValue<T> &a, const TestValue<T> &b) const {
		return a == b;
	}
	template <typename A, typename B>
	bool isEqual(const TestValue<A> &a, const TestValue<B> &b) const {
		return a == b;
	}

	template <typename T>
	std::string asPrintable(const TestValue<T> &a, const TestValue<T> &b) const {
		std::ostringstream os;
		os << a.str();
		if (a.aggregate || b.aggregate) { os << std::endl << "\t"; } else { os << " vs "; }
		os << b.str();
		return os.str();
	}
};

class TestMain {
public:
	int main(int argc, char **argv) {
		bool quiet = false;
		bool timestamp = false;
		int seed = time(0);
		while (true) {
			int opt = getopt(argc, argv, "qts:");
			if (opt == -1) { break; }
			if (opt == 'q') { quiet = true; }
			if (opt == 't') { timestamp = true; }
			if (opt == 's') { seed = std::atoi(optarg); }
		};
		return TestCollection::runAllTests(seed, quiet, timestamp) ? 0 : -1;
	}
};

} // end of namespace upp11

#define UP_MAIN() \
int main(int argc, char **argv) { \
	return upp11::TestMain().main(argc, argv); \
}

#define UP_RUN() \
upp11::TestCollection::runAllTests(0, false, false)
#define UP_RUN_SHUFFLED(seed) \
upp11::TestCollection::runAllTests(seed, false, false)

#define UP_SUITE_BEGIN(name) \
namespace name { \
	static upp11::TestSuiteBegin suite_begin(#name);

#define UP_SUITE_END() \
	static upp11::TestSuiteEnd suite_end; \
}

#define UP_TEST(name) \
struct Test##name : private upp11::TestBase { \
	void run(); \
}; \
static upp11::TestInvokerTrivial<Test##name> test##name##invoker(#name); \
void Test##name::run()

#define UP_FIXTURE_TEST(name, fixture) \
struct Test##name : private upp11::TestBase, public fixture { \
	void run(); \
}; \
static upp11::TestInvokerTrivial<Test##name> test##name##invoker(#name); \
void Test##name::run()

#define UP_PARAMETRIZED_TEST(name, params, ...) \
struct Test##name : private upp11::TestBase { \
	void run(const tuple<__VA_ARGS__> &params); \
}; \
static upp11::TestInvokerParametrized<Test##name, __VA_ARGS__> test##name##invoker(#name, params); \
void Test##name::run(const tuple<__VA_ARGS__> &params)

#define UP_FIXTURE_PARAMETRIZED_TEST(name, fixture, params, ...) \
struct Test##name : private upp11::TestBase, public fixture { \
	void run(const tuple<__VA_ARGS__> &params); \
}; \
static upp11::TestInvokerParametrized<Test##name, __VA_ARGS__> test##name##invoker(#name, params); \
void Test##name::run(const tuple<__VA_ARGS__> &params)

#define UP_ASSERT(expr) \
if (!(expr)) { \
	std::cout << __FILE__ "(" << __LINE__ << "): check " #expr " failed" << std::endl; \
	throw TestException(); \
}

#define UP_ASSERT_EQUAL(...) \
if (!isEqual(__VA_ARGS__)) { \
	std::cout << __FILE__ "(" << __LINE__ << "): check equal (" #__VA_ARGS__ ") failed" << std::endl; \
	std::cout << "\t" << asPrintable(__VA_ARGS__) << std::endl; \
	throw TestException(); \
}

#define UP_ASSERT_NE(...) \
if (isEqual(__VA_ARGS__)) { \
	std::cout << __FILE__ "(" << __LINE__ << "): check not equal (" #__VA_ARGS__ ") failed" << std::endl; \
	std::cout << "\t" << asPrintable(__VA_ARGS__) << std::endl; \
	throw TestException(); \
}
