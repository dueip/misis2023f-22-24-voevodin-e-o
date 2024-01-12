#pragma once

#ifndef OPTIONS_HPP_2023_10_27
#define OPTIONS_HPP_2023_10_27

#include <vector>
#include <string_view>
#include <filesystem>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <unordered_set>
#include <Utilities/Utilities.hpp>

namespace ve{
	class Options final {
	public:
		Options(const Options&) = delete;
		Options operator=(const Options&) = delete;


		static Options& getInstance() {
			static Options options;
			return options;
		}

		constexpr int64_t [[nodiscard]] getChunkSize() const {
			return chunk_size;
		}

		constexpr int64_t [[nodiscard]] getMaxSize() const {
			return ve::fromGigabytes(2);
		}

		void setChunkSize(int64_t new_chunk_size) {
			chunk_size = new_chunk_size;
		}

	private:
		Options() = default;
		int64_t chunk_size = ve::fromMegabytes(512);
	};
}

#endif