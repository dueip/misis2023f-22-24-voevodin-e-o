#pragma once

#ifndef UTILITIES_HPP_2023_10_27
#define UTILITIES_HPP_2023_10_27

#include <vector>
#include <string_view>
#include <filesystem>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <unordered_set>
namespace ve {
	// Works only with unicode
	inline constexpr char toLower(char a) noexcept {
		if (a <= 'Z' && a >= 'A')
			return a + ('a' - 'A');
	}

	// Works only with unicode!
	constexpr inline std::string toLower(const std::string& str) {
		std::string string_temp;
		string_temp.reserve(str.size());
		std::transform(str.begin(), str.end(), string_temp.begin(), [](char a) { return toLower(a); });
		return string_temp;
	}

	enum class ErrorCodes {
		OK = 0,
		FileLargerThanAChunk,
		CannotParseImageFromFile,
		UnsupportedExtension,
		WasDirty,
		SumOfFilesLargerThanAThreshold,
		Unfiltered = ~0
	};

	struct Error {
		Error() = default;
		Error(ErrorCodes code, const std::string& message) : code(code), message(message) {}
		Error(ErrorCodes code) : code(code) {}
		const ErrorCodes code;
		const std::string message = "";
		operator bool() {
			return !static_cast<bool>(code);
		}
	};


	// TODO :all this should be in its own classes
	inline constexpr [[nodiscard]] int64_t toKilobytes(int64_t bytes) noexcept {
		return bytes >> 10;
	}

	inline constexpr [[nodiscard]] int64_t toMegabytes(int64_t bytes) noexcept {
		return ve::toKilobytes(bytes) >> 10;
	}


	inline constexpr [[nodiscard]] int64_t fromKilobytes(int64_t kilobytes) noexcept {
		return kilobytes << 10;
	}

	inline constexpr [[nodiscard]] int64_t fromMegabytes(int64_t megabytes) noexcept {
		return fromKilobytes(megabytes) << 10;
	}

	inline constexpr [[nodiscard]] int64_t fromGigabytes(int64_t gigabytes) noexcept {
		return fromMegabytes(gigabytes) << 10;
	}

	enum class DirectoryIteration {
		NORMAL = 0,
		RECURSIVE = 1
	};
	namespace {
		using Path = std::filesystem::path;
		using DirectoryEntry = std::filesystem::directory_entry;
	}

	inline std::vector<DirectoryEntry> listFiles(const Path& directory, DirectoryIteration it_type = DirectoryIteration::NORMAL) {
		using namespace std::filesystem;
		std::error_code err;

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
}

#endif