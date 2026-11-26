CXX ?= g++
CXXFLAGS ?= -std=c++20 -Wall -Wextra -g
CPPFLAGS += -I. -Itests
SOURCES = json_library.cpp array.cpp singly_list.cpp doubly_list.cpp stack.cpp queue.cpp hash_table.cpp avl_tree.cpp commands.cpp
HEADERS = $(wildcard *.h)
BOOST_TESTS = $(wildcard tests/boost/*.cpp)
GTEST_TESTS = $(wildcard tests/gtest/*.cpp)
CATCH_TESTS = $(wildcard tests/catch2/*.cpp)
COVDIR = .coverage-build
COV_SOURCES = $(SOURCES) main.cpp $(BOOST_TESTS)
COV_OBJS = $(patsubst %.cpp,$(COVDIR)/%.o,$(COV_SOURCES))

all: dbms

dbms: $(SOURCES) main.cpp $(HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(SOURCES) main.cpp -o $@

lint:
	clang-tidy $(filter-out json_library.cpp,$(SOURCES)) main.cpp -- -std=c++20 -I.

analyze:
	@set -e; for f in $(filter-out json_library.cpp,$(SOURCES)) main.cpp; do \
	  echo "Checking $$f"; clang++ --analyze -Xanalyzer -analyzer-output=text -std=c++20 -Wall -Wextra -I. "$$f"; \
	done

run_boost: $(SOURCES) $(BOOST_TESTS) $(HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(SOURCES) $(BOOST_TESTS) -o $@

test: test_boost

test_boost: run_boost
	./run_boost --report_level=detailed

test_gtest:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(SOURCES) $(GTEST_TESTS) -lgtest -lgtest_main -pthread -o run_gtest
	./run_gtest

test_catch2:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(SOURCES) $(CATCH_TESTS) -lCatch2Main -lCatch2 -o run_catch2
	./run_catch2

smoke: dbms
	bash scripts/smoke.sh ./dbms

# Both executables reuse the same instrumented implementation objects.
$(COVDIR)/%.o: %.cpp $(HEADERS)
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -O0 --coverage -c $< -o $@

coverage: coverage_boost

coverage_boost: $(COV_OBJS)
	find $(COVDIR) -name '*.gcda' -delete
	$(CXX) --coverage $(patsubst %.cpp,$(COVDIR)/%.o,$(SOURCES) $(BOOST_TESTS)) -o coverage_tests
	$(CXX) --coverage $(patsubst %.cpp,$(COVDIR)/%.o,$(SOURCES) main.cpp) -o coverage_dbms
	./coverage_tests --report_level=short
	bash scripts/smoke.sh ./coverage_dbms
	python3 scripts/coverage.py $(COVDIR) coverage

bench:
	$(CXX) $(CPPFLAGS) -std=c++20 -O2 $(SOURCES) benchmarks/bench_local.cpp -o run_bench
	./run_bench

bench_google:
	$(CXX) $(CPPFLAGS) -std=c++20 -O2 $(SOURCES) benchmarks/bench.cpp -lbenchmark -pthread -o run_bench
	./run_bench --benchmark_min_time=0.1s

clean:
	rm -f dbms run_boost run_gtest run_catch2 run_bench coverage_tests coverage_dbms *.gcda *.gcno
	rm -rf coverage $(COVDIR)

.PHONY: all lint test test_boost test_gtest test_catch2 smoke coverage coverage_boost bench bench_google analyze clean
