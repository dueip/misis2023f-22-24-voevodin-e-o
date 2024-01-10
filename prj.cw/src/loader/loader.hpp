#pragma once

#ifndef BRUH_HPP_2023_10_27
#define BRUH_HPP_2023_10_27

#include <vector>
#include <string_view>
#include <filesystem>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <unordered_set>
#include <Options/Options.hpp>


namespace ve {

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

		virtual constexpr [[nodiscard]] int64_t getMaxChunkSize() const noexcept = 0;
		
		virtual [[nodiscard]] int64_t getCurrentSize() const noexcept = 0;
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
			std::string fixed_extension;
			fixed_extension.reserve(fixed_extension.size() + 1);
			if (extension.size() > 0 && extension[0] != '.') {
				fixed_extension = '.' + extension;
			}
			else {
				fixed_extension = extension;
			}
			return (supported_extensions.find(fixed_extension) != supported_extensions.end());
		}


		void reset() override { is_dirty_ = false; mats_.clear(); }

		constexpr [[nodiscard]] int64_t getMaxChunkSize() const noexcept override { return chunk_size; }
		[[nodiscard]] int64_t getCurrentSize() const noexcept override;
	private:
		std::vector<cv::Mat> mats_;
		bool is_dirty_ = false;

		const inline static int64_t chunk_size = Options::getInstance().getChunkSize();
		// For now we'll go with only those 3 types, but we'll see.
		const inline static std::unordered_set<std::string> supported_extensions = { ".png", ".jpg", ".jpeg" };
	};

	class DirectoryLoader final  {
	public:
		DirectoryLoader() = default;
		virtual ~DirectoryLoader() = default;

		ve::Error loadFromDirectory(const Path& path);

		template<std::forward_iterator PathIt>
		ve::Error loadFromFiles(const PathIt& begin, const PathIt& end);


		[[nodiscard]] const std::vector<cv::Mat>& getData() const noexcept { return constructVector(); };
		[[nodiscard]] std::vector<cv::Mat> copyData() const noexcept { return constructVector(); };


		bool isDirty() const noexcept { for (const auto& el : loaders) { if (el->isDirty()) return true; } return false; };

		void reset() { for (auto& el : loaders) { el->reset(); } }

		constexpr [[nodiscard]] bool isExtensionSupported(const std::string& extension) const noexcept {
			for (const auto& el : loaders) { if (el->isExtensionSupported(extension)) return true; } return false;
		}
		
		constexpr [[nodiscard]] int64_t getMaxChunkSize() const noexcept {
			return Options::getInstance().getChunkSize();
		}
		int64_t getCurrentSizeOfBlocks() const noexcept {
			// potentially can overflow if we get big enough numbers
			// but for now shouldn't worry since there's no way any picture file will outgrow 2^63 (~9 000 000 000) gigabytes
			int64_t sum_of_sizes = 0;
			for (const auto& loader : loaders) {
				sum_of_sizes += loader->getCurrentSize();
			}
			return sum_of_sizes;
		}
	private:

		// Mutable since we do want our getChunkSize function to be available anywhere
		// -1 means that we don't have it cached just yet
		mutable int64_t chunk_size_cached_ = -1;
		

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

// wtf is this
// TOOD: should probably be the basic implementation for ever loadFromDirectory, but oh well??
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