#pragma once

#ifndef BRUH_HPP_2023_10_27
#define BRUH_HPP_2023_10_27

#include <vector>
#include <string_view>
#include <filesystem>


namespace ve {
	enum class DirectoryIteration {
		NORMAL = 0,
		RECURSIVE = 1
	};
	using Path = std::filesystem::path;
	using DirectoryEntry = std::filesystem::directory_entry;
	
	
	std::vector<DirectoryEntry> listFiles(const Path& directory, DirectoryIteration it_type = DirectoryIteration::NORMAL) {
		using namespace std::filesystem;
		std::error_code err;
		
		//// Если путь не является директорией, то кидаем ошибку
		//if (!is_directory(status(directory, err))) {
		//	throw std::exception(err.message().c_str());
		//}
		//
		std::vector<DirectoryEntry> all_files;
		
		switch (it_type) {
		case DirectoryIteration::NORMAL: 
			for (auto& el : directory_iterator(directory)) {
				all_files.push_back(el);
			}
			break;
		case DirectoryIteration::RECURSIVE: 
			for (auto& el : recursive_directory_iterator(directory)) {
				all_files.push_back(el);
			}
			break;
		}

		return all_files;
		
	}

	// Move only
	class ILoader {
	public:
		ILoader() = default;
		ILoader& operator=(const ILoader&) = delete;
		ILoader(const ILoader&) = delete;

		ILoader& operator=(ILoader&&) = default;
		ILoader(ILoader&&) = default;

		virtual ~ILoader() = default;


		virtual constexpr bool loadFromFile(const std::filesystem::path& path) const = 0;
	};

	class TestLoader final : public ILoader {
	public:
		TestLoader() = default;
		virtual constexpr bool loadFromFile(const std::filesystem::path& path) const override { return true; };
		virtual ~TestLoader() = default;
	};
}

#endif