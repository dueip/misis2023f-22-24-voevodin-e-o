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

	class MatButCooler final {
	public:
		MatButCooler(const std::filesystem::path& path) {
			path_to_the_mat = path;
		}

		operator cv::Mat() {
			if (mat.empty()) {
				mat = cv::imread(path_to_the_mat.string());
			}
			return mat;
		}

		void markReadyToDelete() {
			// Just dropss the mat???
			int refCount = (mat.u) ? ((mat.u)->refcount) : 0;
			if (refCount > 1) {
				throw std::exception("WHAT IS HAPPPENING THIS SHOULD HAVE NEVER BEEN THE CASE");
			}
			mat.release();

			should_be_deleted = false;
		}

	private:
		bool should_be_deleted = true;
		cv::Mat mat;
		std::filesystem::path path_to_the_mat;
	};

	/*!
		Класс, который предоставляет функции для сохранения масок в определенную папку, а также дальнейших их подгрузок.
	*/
	class ImageWriter {
	public:
		ImageWriter() = delete;
		ImageWriter(const std::filesystem::path& path) {
			setPath(path);
		}
		ImageWriter(ImageWriter&&) = default;
		ImageWriter& operator=(ImageWriter&&) = default;

		ImageWriter& operator=(const ImageWriter&) = delete;
		ImageWriter(const ImageWriter&) = delete;

		~ImageWriter() = default;


		enum class Extensions {
			JPG,
			PNG
		};

		void setPath(const std::filesystem::path& path_to_add) noexcept {
			// Если путь -- директория и при этом её не существует, то создаём его
			if (!std::filesystem::exists(path_to_add)){
				std::filesystem::create_directory(path_to_add);
			}
			cached_path_ = path_to_add;
		}

		/*!  \todo Добавить поддержку  нескольких расширений сразу
		* Сохраняет маски, переданные в эту функцию, в кэшированную папку, заданную в конструкторе.
		*
		* \param [in] begin Итератор, указывающий на начало контейнера с масками
		* \param [in] end Итератор, указывающий на конец контейнера с масками
		* \param [in] extension Сохранять с каким расширением (см enum Extensions)
		* \return OK если не было ошибки. UnsupportedExtension, если расширение по какой-либо причине не поддерживается. CannotParseImageFromFile, если по какой-то бэкэнд не может сохранить картинку.
		*/
		template<std::forward_iterator ForwardIt>
		ve::Error saveMasks(const ForwardIt begin, const  ForwardIt end, Extensions extension = Extensions::JPG) const   {
			return saveMasks(begin, end, cached_path_, extension);
		}


		/*!  \todo Добавить поддержку  нескольких расширений сразу
		* Сохраняет маски, переданные в эту функцию, в целевую папку
		* 
		* \param [in] begin Итератор, указывающий на начало контейнера с масками
		* \param [in] end Итератор, указывающий на конец контейнера с масками
		* \param [in] path Целевая папка
		* \param [in] extension Сохранять с каким расширением (см enum Extensions)
		* \return OK если не было ошибки. UnsupportedExtension, если расширение по какой-либо причине не поддерживается. CannotParseImageFromFile, если по какой-то бэкэнд не может сохранить картинку.
		*/
		template<std::forward_iterator ForwardIt>
		static ve::Error saveMasks(const ForwardIt begin, const ForwardIt end, const std::filesystem::path& path, Extensions extension = Extensions::JPG) {
			for (ForwardIt it = begin; it != end; ++it) {
				std::string string_extension;
				switch (extension) {
				case Extensions::JPG:
					string_extension = ".jpg";
					break;
				case Extensions::PNG:
					string_extension = ".png";
					break;
				default:
					return ve::ErrorCodes::UnsupportedExtension;
				}
				
				std::string where_to_save = path.string() + "/mask_" + std::to_string(masks_saved_count) + string_extension;
				if (!cv::imwrite(where_to_save, *it)) {
					return ve::ErrorCodes::CannotParseImageFromFile;
				}
				++masks_saved_count;

			}
			return ve::ErrorCodes::OK;
		}
 
	private:
		std::filesystem::path cached_path_;
		inline static int masks_saved_count = 0;
	};

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
		ve::Error loadFromFile(const Path& path) override {
			//if (isExtensionSupported(path.extension().string())) {
			//	return ve::ErrorCodes::UnsupportedExtension;
			//}
			//DicomImage image(path.string().c_str());
			//if (image.getStatus() != EIS_Normal) {
			//	//! \todo Add support for  different reasons why it's not possible to read the dicom.
			//	return ve::ErrorCodes::CannotParseImageFromFile;
			//}

			//cv::Mat mat(int(image.getWidth()), int(image.getHeight()), CV_8U, (uchar*)image.getOutputData(8));
			//mats_.push_back(mat)
			return ve::ErrorCodes::CannotParseImageFromFile;
		}

		bool isDirty() const noexcept { return is_dirty_; };
		
		constexpr [[nodiscard]] bool isExtensionSupported(const std::string& extension) const noexcept override {
			return extension == ".dcm";
		}

		void reset() override { is_dirty_ = false; mats_.clear(); }

		constexpr [[nodiscard]] int64_t getMaxChunkSize() const noexcept override { return Options::getInstance().getChunkSize(); }
		[[nodiscard]] int64_t getCurrentSize() const noexcept override {
			int64_t size_sum = 0;
			for (const auto& el : mats_) {
				size_sum += el.elemSize() * el.total();
			}
			return size_sum;
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


	class ImageStorage final {
	public:
	private:

	};
	

	class DirectoryLoader final  {
	public:
		DirectoryLoader() = default;
		virtual ~DirectoryLoader() = default;

		ve::Error loadFromDirectory(const Path& path);

		template<std::forward_iterator PathIt>
		ve::Error loadFromFiles(const PathIt& begin, const PathIt& end);


		[[nodiscard]] const std::vector<cv::Mat>& getData() const noexcept { return cached_vector_; };
		[[nodiscard]] std::vector<cv::Mat> copyData() const noexcept { return cached_vector_; };


		bool isDirty() const noexcept { for (const auto& el : loaders) { if (el->isDirty()) return true; } return false; };

		void reset() { cached_vector_.clear(); for (auto& el : loaders) { el->reset(); } }

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


		std::vector<cv::Mat> cached_vector_;

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
// Also should smack here some concept since it accepts only path and not directory entry(?weird?)
template<std::forward_iterator PathIt>
ve::Error ve::DirectoryLoader::loadFromFiles(const PathIt& begin, const PathIt& end) {	

	if (isDirty()) {
		return ve::ErrorCodes::WasDirty;
	}

	for (auto& loader : loaders) {
		for (PathIt entry = begin; entry != end; ++entry) {	
			if (loader->isExtensionSupported(entry->extension().string())) {
				ve::Error err = loader->loadFromFile(*entry);
				if (getCurrentSizeOfBlocks() > Options::getInstance().getMaxSize()) {
					throw std::exception("Dont load so much, geez");
				}
				if (err.code != ve::ErrorCodes::OK) return err;
			}
		}
	}
	cached_vector_ = constructVector();
	return ve::ErrorCodes::OK;
}

#endif