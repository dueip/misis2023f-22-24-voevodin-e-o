#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <bruh.hpp>
#include <algorithm>


auto check_if_everything_was_found_correctly = []
(const std::vector<ve::DirectoryEntry>& whats_being_checked, const std::vector<ve::Path>& to_check_with) -> bool {
	for (auto& el : whats_being_checked) {
		if (std::find(to_check_with.begin(), to_check_with.end(), el.path()) == to_check_with.end()) {
			return false;
		}
	}
	return true;
};
	

TEST_CASE("List files") {
	
	SUBCASE("Normal") {
		std::filesystem::path test_dir_path = "test_dir";
		std::vector<ve::Path> normal_entries = { "test_rec_dir", "file.txt" };

		REQUIRE_NOTHROW(ve::listFiles(test_dir_path));
		auto test_dir_entires = ve::listFiles(test_dir_path);
		CHECK(check_if_everything_was_found_correctly(test_dir_entires, normal_entries));

	
	}

	SUBCASE("Recursive") {

	}
}
