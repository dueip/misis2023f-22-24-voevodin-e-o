#pragma once

#ifndef BRUH_HPP_2023_10_27
#define BRUH_HPP_2023_10_27

#include <vector>
#include <string_view>
#include <filesystem>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <unordered_set>


namespace ve {
	inline constexpr char toLower(char a) noexcept {
		if (a <= 'Z' && a >= 'A')
			return a + ('a' - 'A');
	}


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

	// Move only
	class ILoader {
	public:
		ILoader() = default;
		ILoader& operator=(const ILoader&) = delete;
		ILoader(const ILoader&) = delete;

		ILoader& operator=(ILoader&&) = default;
		ILoader(ILoader&&) = default;

		virtual ~ILoader() = default;

		

		virtual ve::Error loadFromFile(const Path& path) = 0;

		// A flag to wether underlaying data's been already moved out, thus making
		// it available for new use
		virtual bool isDirty() const noexcept = 0;

		virtual void reset() = 0;

		// It moves it out so it is not const.
		virtual [[nodiscard]] std::vector<cv::Mat>& getData() noexcept = 0;
		
		virtual [[nodiscard]] const std::vector<cv::Mat>& getData() const noexcept = 0;

		virtual [[nodiscard]] std::vector<cv::Mat> copyData() const noexcept = 0;

		virtual constexpr [[nodiscard]] bool isExtensionSupported(const std::string& extension) const noexcept = 0;
	};

	class TestLoader final : public ILoader {
	public:
		TestLoader() {
			mats_.reserve(5);
		}
		virtual ~TestLoader() = default;

		[[nodiscard]] std::vector<cv::Mat>& getData() noexcept override { is_dirty_ = true; return mats_; };

		[[nodiscard]] const std::vector<cv::Mat>& getData() const noexcept override { return mats_; };

		[[nodiscard]] std::vector<cv::Mat> copyData() const noexcept override { return mats_; };

		ve::Error loadFromFile(const Path& path) override	 { return ve::ErrorCodes::OK; };
		bool isDirty() const noexcept { return is_dirty_; };

	
	private:
		std::vector<cv::Mat> mats_;
		bool is_dirty_ = false;
	};

	class DicomLoader final : public ILoader {
	public:
		DicomLoader() = default;
		DicomLoader(const DicomLoader& other) = delete;
		DicomLoader& operator=(const DicomLoader& other) = delete;
		
		DicomLoader(DicomLoader&&) = default;
		DicomLoader& operator=(DicomLoader&&) = default;
		
		virtual ~DicomLoader() = default;

		[[nodiscard]] std::vector<cv::Mat>& getData() noexcept { is_dirty_ = true; return mats_; };
		[[nodiscard]] const std::vector<cv::Mat>& getData() const noexcept { return mats_; };
		[[nodiscard]] std::vector<cv::Mat> copyData() const noexcept { return mats_; };

		// TODO: implement this
		ve::Error loadFromFile(const Path& path) override { return ve::ErrorCodes::Unfiltered; }

		bool isDirty() const noexcept { return is_dirty_; };
		
		constexpr [[nodiscard]] bool isExtensionSupported(const std::string& extension) const noexcept override {
			return extension == ".dcm";
		}

	private:
		std::vector<cv::Mat> mats_;
		bool is_dirty_ = false;
	};

	class ImageLoader final : public ILoader {
	public:

		ImageLoader() = default;
		ImageLoader(const ImageLoader&) = delete;
		ImageLoader& operator=(const ImageLoader&) = delete;

		ImageLoader(ImageLoader&&) = default;
		ImageLoader& operator=(ImageLoader&&) = default;
		
		virtual ~ImageLoader() = default;

		[[nodiscard]] std::vector<cv::Mat>& getData() noexcept { is_dirty_ = true; return mats_; };
		[[nodiscard]] const std::vector<cv::Mat>& getData() const noexcept { return mats_; };
		[[nodiscard]] std::vector<cv::Mat> copyData() const noexcept { return mats_; };

		ve::Error loadFromFile(const Path& path) override;

		bool isDirty() const noexcept { return is_dirty_; };

		[[nodiscard]] bool isExtensionSupported(const std::string& extension) const noexcept override {
			return (extensions.find(extension) != extensions.end());
		}


		void reset() override { is_dirty_ = false; mats_.clear(); }
	private:
		std::vector<cv::Mat> mats_;
		bool is_dirty_ = false;
		// Should use OPENCV_IO_MAX_IMAGE_PIXELS here, but for some reason cannot find it?
		constexpr inline static int64_t chunk_size = fromGigabytes(1);
		// For now we'll go with only those 3 types, but we'll see.
		const inline static std::unordered_set<std::string> extensions = { ".png", ".jpg", ".jpeg" };
	};

	class DirectoryLoader final : public ILoader {
	public:
		DirectoryLoader() = default;
		virtual ~DirectoryLoader() = default;

		ve::Error loadFromFile(const Path& path) override;
		template<std::forward_iterator PathIt>
		ve::Error loadFromFiles(const PathIt& begin, const PathIt& end);


		[[nodiscard]] std::vector<cv::Mat>& getData() noexcept { throw std::exception("You can't use non-const getdata over a directory"); };
		[[nodiscard]] const std::vector<cv::Mat>& getData() const noexcept { return constructVector(); };
		[[nodiscard]] std::vector<cv::Mat> copyData() const noexcept { return constructVector(); };


		bool isDirty() const noexcept { for (const auto& el : loaders) { if (el->isDirty()) return true; } return false; };

		void reset() { for (auto& el : loaders) { el->reset(); } }

		constexpr [[nodiscard]] bool isExtensionSupported(const std::string& extension) const noexcept override {
			return false;
		}
		
	private:
		const std::vector<std::shared_ptr<ILoader>> loaders = {
			std::make_shared<ImageLoader>()
		};

		constexpr [[nodiscard]] std::vector<cv::Mat> constructVector() const {
			std::vector<cv::Mat> vec;
			for (const auto& loader : loaders) {
				const std::vector<cv::Mat>& loader_data = loader->getData();
				for (const cv::Mat& mat : loader_data) {
					vec.push_back(mat);
				}
			}
			return vec;
		}
	};

}


template<std::forward_iterator PathIt>
ve::Error ve::DirectoryLoader::loadFromFiles(const PathIt& begin, const PathIt& end) {
	if (isDirty()) {
		return ve::ErrorCodes::WasDirty;
	}

	for (auto& loader : loaders) {
		for (PathIt entry = begin; entry != end; ++entry) {	
			if (loader->isExtensionSupported(entry->extension().string())) {
				ve::Error err = loader->loadFromFile(*entry);
				if (err.code != ve::ErrorCodes::OK) return err;
			}
		}
	}
	return ve::ErrorCodes::OK;
}

#endif