
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

public:
	static TestCollection &getInstance() {
		static TestCollection collection;
		return collection;
	}

	void beginSuite(const std::string &name) {
		suites.push_back(name);
	}

	void endSuite() {
		suites.pop_back();
	}

	void addTest(const std::string &name, std::function<bool ()> test) {
		std::string path;
		for (auto s: suites) {
			path += s + "/";
		}
		tests.push_back(std::make_pair(path + name, test));
	}

	bool runAllTests(unsigned seed, bool quiet, bool timestamp) {
		// Отсортируем все по именам, чтобы не зависело от порядка линковки
		std::sort(tests.begin(), tests.end(),
			[](const test_pair_t &A, const test_pair_t &B){ return A.first < B.first; });
		if (seed != 0) {
			if (!quiet) {
				std::cout << "random seed: " << seed << std::endl;
			}
			std::default_random_engine r(seed);
			std::shuffle(tests.begin(), tests.end(), r);
		}
		bool failure = false;
		for (auto t: tests) {
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
		TestCollection::getInstance().beginSuite(name);
	}
};

class TestSuiteEnd {
public:
	TestSuiteEnd() {
		TestCollection::getInstance().endSuite();
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
		TestCollection::getInstance().addTest(name,
			std::bind(&TestInvokerTrivial::invoke, this));
	}
};

template <typename T, typename C>
class TestInvokerParametrized : public TestInvoker<T> {
private:
	bool invoke(const typename C::value_type &params) {
		return TestInvoker<T>::invoke(std::bind(&T::run, std::placeholders::_1, params));
	}
public:
	TestInvokerParametrized(const std::string &name, const C &params)
	{
		for (const auto v: params) {
			TestCollection::getInstance().addTest(name,
				std::bind(&TestInvokerParametrized::invoke, this, v));
		}
	}
};

template <typename T>
struct TestValue {
	const std::vector<T> value;
	const bool agregate;

	TestValue(const T &iv)
		: value(1, iv), agregate(false) {}
	template<typename I>
	TestValue(const I &begin, const I &end)
		: value(begin, end), agregate(true) {}
};

template <typename T>
std::ostream &operator << (std::ostream &os, const TestValue<T> &t)
{
	if (t.agregate) { os << "{ "; }
	std::copy(t.value.begin(), t.value.end(), std::ostream_iterator<T>(os, ", "));
	if (t.agregate) { os << " }"; }
	return os;
}

class TestBase {
	template <typename T>
	TestValue<T> createTestValue(const T &t) const {
		return TestValue<T>(t);
	}
	template<typename T>
	TestValue<T> createTestValue(const std::initializer_list<T> &t) const {
		return TestValue<T>(t.begin(), t.end());
	}
	template<typename T>
	TestValue<T> createTestValue(const std::initializer_list<const T> &t) const {
		return TestValue<T>(t.begin(), t.end());
	}
	template<typename T>
	TestValue<T> createTestValue(const std::list<T> &t) const {
		return TestValue<T>(t.begin(), t.end());
	}
	template<typename T>
	TestValue<T> createTestValue(const std::vector<T> &t) const {
		return TestValue<T>(t.begin(), t.end());
	}
	template<typename T, std::size_t size>
	TestValue<T> createTestValue(const std::array<T, size> &t) const {
		return TestValue<T>(t.begin(), t.end());
	}
	template<typename T, std::size_t size>
	TestValue<T> createTestValue(const T (&t)[size]) const {
		return TestValue<T>(&t[0], &t[size]);
	}
	TestValue<std::string> createTestValue(const char *t) const {
		return TestValue<std::string>(t);
	}

	template <typename A, typename B>
	bool isEqualValue(const A &, const B &) const {
		return false;
	}
	template <typename T>
	bool isEqualValue(const TestValue<T> &ta, const TestValue<T> &tb) const {
		return ta.agregate == tb.agregate && ta.value == tb.value;
	}
public:
	template <typename A, typename B>
	bool isEqual(const A &a, const B &b) const {
		const auto ta = createTestValue(a);
		const auto tb = createTestValue(b);
		return isEqualValue(ta, tb);
	}

	template <typename A, typename B>
	std::string asPrintable(const A &a, const B &b) const {
		const auto ta = createTestValue(a);
		const auto tb = createTestValue(b);
		std::ostringstream os;
		os << ta << " vs " << tb;
		return os.str();
	}
};

template <typename E>
struct TestExceptionChecker {
	const std::string location;
	const std::string extype;

	TestExceptionChecker(const std::string &location, const std::string &extype)
		: location(location), extype(extype) {}

	void check(const std::function<void ()> &f) {
		try {
			f();
		} catch (const E &e) {
			return;
		} catch (...) {
		}
		std::cout << location << ": expected exception "
			<< extype << " not throw" << std::endl;
		throw upp11::TestException();
	}

	void check(const std::string &message, const std::function<void ()> &f) {
		if (!std::is_convertible<E, std::exception>::value) {
			std::cout << location << ": expected exception "
				<< extype << " is not child of std::exception" << std::endl;
			throw upp11::TestException();
		}
		bool catched = false;
		try {
			try {
				f();
			} catch (const E &) {
				catched = true;
				throw;
			}
		} catch (const std::exception &e) {
			if (catched) {
				if (e.what() == message) { return; }
				std::cout << location << ": check exception "
					<< extype << "(\"" << message << "\") failed" << std::endl;
				std::cout << "\tcatched exception: \"" << e.what() << "\"" << std::endl;
				throw upp11::TestException();
			}
		} catch (...) {
		}
		std::cout << location << ": expected exception "
			<< extype << "(\"" << message << "\") not throw" << std::endl;
		throw upp11::TestException();
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
		return TestCollection::getInstance().runAllTests(seed, quiet, timestamp) ? 0 : -1;
	}
};

} // end of namespace upp11

#define T_(x) #x
#define T(x) T_(x)
#define LOCATION __FILE__ "(" T(__LINE__) ")"

#define UP_MAIN() \
int main(int argc, char **argv) { \
	return upp11::TestMain().main(argc, argv); \
}

#define UP_RUN() \
upp11::TestCollection::getInstance().runAllTests(0, false, false)

#define UP_SUITE_BEGIN(name) \
namespace name { \
	static upp11::TestSuiteBegin suite_begin(#name);

#define UP_SUITE_END() \
	static upp11::TestSuiteEnd suite_end; \
}

#define UP_TEST(testname) \
struct testname { \
	void run(); \
}; \
static upp11::TestInvokerTrivial<testname> testname##_invoker(#testname); \
void testname::run()

#define UP_FIXTURE_TEST(testname, fixture) \
struct testname : public fixture { \
	void run(); \
}; \
static upp11::TestInvokerTrivial<testname> testname##_invoker(#testname); \
void testname::run()

#define UP_PARAMETRIZED_TEST(testname, params) \
struct testname { \
	void run(const decltype(params)::value_type &params); \
}; \
static upp11::TestInvokerParametrized<testname, decltype(params)> \
	testname##_invoker(#testname, params); \
void testname::run(const decltype(params)::value_type &params)

#define UP_FIXTURE_PARAMETRIZED_TEST(testname, fixture, params) \
struct testname : public fixture { \
	void run(const decltype(params)::value_type &params); \
}; \
static upp11::TestInvokerParametrized<testname, decltype(params)> \
	testname##_invoker(#testname, params); \
void testname::run(const decltype(params)::value_type &params)

#define UP_ASSERT(expr) \
if (!(expr)) { \
	std::cout << __FILE__ "(" << __LINE__ << "): check " #expr " failed" << std::endl; \
	throw upp11::TestException(); \
}

#define UP_ASSERT_EQUAL(...) \
if (!upp11::TestBase().isEqual(__VA_ARGS__)) { \
	std::cout << LOCATION ": check equal (" #__VA_ARGS__ ") failed" << std::endl; \
	std::cout << "\t" << upp11::TestBase().asPrintable(__VA_ARGS__) << std::endl; \
	throw upp11::TestException(); \
}

#define UP_ASSERT_NE(...) \
if (upp11::TestBase().isEqual(__VA_ARGS__)) { \
	std::cout << LOCATION ": check not equal (" #__VA_ARGS__ ") failed" << std::endl; \
	std::cout << "\t" << upp11::TestBase().asPrintable(__VA_ARGS__) << std::endl; \
	throw upp11::TestException(); \
}

#define UP_ASSERT_EXCEPTION(extype, ...) \
upp11::TestExceptionChecker<extype>(LOCATION, #extype).check(__VA_ARGS__)
