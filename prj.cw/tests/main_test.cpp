﻿#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <loader/loader.hpp>
#include <algorithm>
#include <map>

namespace {
	auto check_if_everything_was_found_correctly = []
	(const std::vector<ve::DirectoryEntry>& whats_being_checked, const std::vector<ve::Path>& to_check_with,
		const ve::Path& original_path
		) -> bool {
			for (auto& el : whats_being_checked) {
				auto relative_path = std::filesystem::relative(el, original_path);
				if (std::find(to_check_with.begin(), to_check_with.end(), relative_path) == to_check_with.end()) {
					return false;
				}
			}
			return true;
		};

	auto check_list_files = []
	(const std::vector<ve::Path>& answers, const std::filesystem::path& rel_test_path,
		ve::DirectoryIteration it = ve::DirectoryIteration::NORMAL) -> bool
		{
			const std::filesystem::path test_dir_path = std::filesystem::path(CMAKE_TEST_PATH).string() + rel_test_path.string();
			auto test_dir_entires = ve::listFiles(test_dir_path, it);
			return check_if_everything_was_found_correctly(test_dir_entires, answers, test_dir_path);
		};
}

TEST_CASE("List files") {
	
	SUBCASE("Normal") {
		const std::vector<ve::Path> normal_entries = { "test_rec_dir", "file.txt" };
		const std::vector<ve::Path> empty = { };

		const std::map<std::string, std::vector<ve::Path>> cases =
		{
			{"test_dir", normal_entries},
			{"test_dir_empty", empty}
		};

		REQUIRE_NOTHROW(ve::listFiles(CMAKE_TEST_PATH));

		for (const auto& test_case : cases) {
			CHECK(check_list_files(test_case.second, test_case.first));
		}
	
	}

	SUBCASE("Recursive") {
		REQUIRE_NOTHROW(ve::listFiles(CMAKE_TEST_PATH));
		CHECK(check_list_files({"file.txt","test_rec_dir", "test_rec_dir/file2.txt"}, "test_dir", ve::DirectoryIteration::RECURSIVE));
	}
}

TEST_CASE("Translation units") {
	CHECK(ve::fromKilobytes(1) == 1024);
	CHECK(ve::fromMegabytes(1) == 1024 * 1024);
	// Проверяем, равен ли 1 мегабайт 1024 килобайтам
	CHECK(ve::fromMegabytes(1) == ve::fromKilobytes(1024));
	CHECK(ve::fromGigabytes(1) == 1024 * 1024 * 1024);
	CHECK(ve::fromKilobytes(1) + 512 == 1536);

	CHECK(ve::toKilobytes(1024) == 1);
	CHECK(ve::toMegabytes(ve::fromMegabytes(1)) == 1);
}

TEST_CASE("Image loader") {
	ve::ImageLoader image_loader;
	CHECK(image_loader.isDirty() == false);
	
	SUBCASE("Check if everything loads correctly") {
		CHECK_FALSE(image_loader.loadFromFile("./"));
		CHECK(image_loader.loadFromFile("./").code == ve::ErrorCodes::UnsupportedExtension);
		const std::vector<std::filesystem::path> files{
			std::string(CMAKE_TEST_PATH) + "images/1.jpg",
			std::string(CMAKE_TEST_PATH) + "images/3.png"
		};
		for (const auto& el : files) {
			CHECK(image_loader.loadFromFile(el));
		}
		CHECK(image_loader.getLoadedSize() == 2);
		CHECK(image_loader.getLoadedSize() == image_loader.getData().size());
		cv::Mat mat1 = image_loader.at(1);
		CHECK(!mat1.empty());
		CHECK(image_loader.getLoadedSize() == 2);
		cv::Mat mat = image_loader.at(0);
		CHECK(!mat.empty());
		CHECK(image_loader.getLoadedSize() == 2);
		
		image_loader.reset();
		
	}
}

TEST_CASE("Directory Loader") {
	ve::DirectoryLoader dir_loader;

	SUBCASE("DirectoryLoading, [loading]") {
		
		CHECK(dir_loader.loadFromDirectory("./"));
		dir_loader.reset();
		CHECK(dir_loader.loadFromDirectory(CMAKE_TEST_PATH));
		CHECK(dir_loader.copyData().size() == 0);
		dir_loader.reset();
		const std::vector<std::filesystem::path> files{
			std::string(CMAKE_TEST_PATH) + "images/1.jpg",
			std::string(CMAKE_TEST_PATH) + "images/3.png"
		};

		CHECK(dir_loader.loadFromFiles(files.cbegin(), files.cend()));
		CHECK(dir_loader.copyData().size() == 2);
		dir_loader.reset();
		CHECK(dir_loader.loadFromDirectory(std::string(CMAKE_TEST_PATH) + "images/"));
		CHECK(dir_loader.copyData().size() == 2);
		dir_loader.reset();
		ve::Error err = dir_loader.loadFromDirectory(std::string(CMAKE_TEST_PATH) + "images/bugged_images");
		CHECK(!err);
		CHECK(err.message == std::string(CMAKE_TEST_PATH) + "images/bugged_images\\1.jpg");
		CHECK(dir_loader.copyData().size() == 0);
		dir_loader.reset();

		SUBCASE("DirectoryLoading, [Load a big amount of data]") {
			CHECK(dir_loader.loadFromFiles(files.cbegin(), files.cend()));
			const std::string big_data_path = std::string(CMAKE_TEST_PATH) + "/big_data/";
			const auto& data = dir_loader.getData();
			ve::generateTestData(big_data_path, data[0]);
			dir_loader.reset();
			dir_loader.loadFromDirectory(big_data_path);
			CHECK(dir_loader.getData().size() == ceil(ve::fromMegabytes(512) / static_cast<float>(ve::calculateMatSize(dir_loader.getData()[0]))));
		}
	}
}

TEST_CASE("ImageWriter") {
	ve::ImageWriter writer(std::string(CMAKE_TEST_PATH) + "/test_masks/");
	
	ve::DirectoryLoader dir_loader;
	
	SUBCASE("Image Writer, [writing 1 cv::Mat to tests/masks]") {
		const std::vector<std::filesystem::path> files{
			std::string(CMAKE_TEST_PATH) + "images/1.jpg"
		};
		dir_loader.loadFromFiles(files.cbegin(), files.cend());
		CHECK(writer.saveMasks(dir_loader.getData().cbegin(), dir_loader.getData().cend()));
	}
	SUBCASE("Image Writer, [writing 2 cv::Mat to tests/masks]") {
		const std::vector<std::filesystem::path> files{
			std::string(CMAKE_TEST_PATH) + "images/1.jpg",
			std::string(CMAKE_TEST_PATH) + "images/3.png"
		};
		dir_loader.loadFromFiles(files.cbegin(), files.cend());
		const auto& data = dir_loader.getData();
		CHECK(writer.saveMasks(data.cbegin(), data.cend()));
	}
}




TEST_CASE("Dicom loader") {
	ve::DicomLoader loader;

	SUBCASE("Dicom loader, [Unable to read even 1 dcm file because this stupid library wouldnt work :)]") {
		CHECK_FALSE(loader.loadFromFile(std::string(CMAKE_TEST_PATH) + "/sample_dicom/MRBRAIN.DCM"));
	}
	
}