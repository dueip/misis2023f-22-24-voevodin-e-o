#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <bruh.hpp>


TEST_CASE("a") {
	CHECK(1 == 1);
	ve::TestLoader tl;
	CHECK(tl.loadFromFile(""));
}
