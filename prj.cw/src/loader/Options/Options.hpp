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
			return ve::fromMegabytes(512);
		}

		constexpr int64_t [[nodiscard]] getMaxSize() const {
			return ve::fromGigabytes(1);
		}

	private:
		Options() = default;
	};
}

#endif