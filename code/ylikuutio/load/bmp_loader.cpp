#include "bmp_loader.hpp"
#include "code/ylikuutio/file/file_loader.hpp"
#include "code/ylikuutio/memory/memory_templates.hpp"

// Include standard headers
#include <algorithm> // std::copy
#include <cstddef>  // std::size_t
#include <cstdio>   // std::FILE, std::fclose, std::fopen, std::fread, std::getchar, std::printf etc.
#include <iostream> // std::cout, std::cin, std::cerr
#include <limits>   // std::numeric_limits
#include <stdint.h> // uint32_t etc.
#include <string>   // std::string
#include <vector>   // std::vector

namespace yli
{
    namespace load
    {
        uint8_t* load_BMP_file(
                const std::string& bmp_filename,
                std::size_t& image_width,
                std::size_t& image_height,
                std::size_t& image_size)
        {
            std::cout << "Loading BMP file " << bmp_filename << " ...\n";

            // Open the file
            const std::vector<uint8_t> file_content = yli::file::binary_slurp(bmp_filename);

            if (file_content.empty())
            {
                std::cerr << bmp_filename << " could not be opened, or the file is empty.\n";
                return nullptr;
            }

            const std::size_t header_size = 54;

            if (file_content.size() < header_size)
            {
                // BMP header size is 54 bytes.
                std::cerr << bmp_filename << " is not a correct BMP file.\n";
                return nullptr;
            }

            if (file_content[0] != 'B' || file_content[1] != 'M')
            {
                // BMP begins always with "BM".
                std::cerr << bmp_filename << " is not a correct BMP file.\n";
                return nullptr;
            }

            uint8_t* file_content_uint8_t = (uint8_t*) file_content.data();

            // The start offset of pixel array in file.
            uint32_t pixel_array_start_offset = yli::memory::read_nonaligned_32_bit<uint8_t, uint32_t>(file_content_uint8_t, 0x0a);

            if (pixel_array_start_offset >= file_content.size())
            {
                std::cerr << "is not a correct BMP file.\n";
                return nullptr;
            }

            // Make sure this is a 24bpp file
            uint32_t bits_per_pixel = yli::memory::read_nonaligned_32_bit<uint8_t, uint32_t>(file_content_uint8_t, 0x1c);

            if (bits_per_pixel != 24)
            {
                std::cerr << "not a correct 24-bit BMP file.\n";
                return nullptr;
            }

            uint32_t compression_type = yli::memory::read_nonaligned_32_bit<uint8_t, uint32_t>(file_content_uint8_t, 0x1e);

            if (compression_type != 0)
            {
                std::cerr << "not a correct uncompressed 24-bit BMP file.\n";
                return nullptr;
            }

            // Read the information about the image
            uint32_t image_size_uint32_t = yli::memory::read_nonaligned_32_bit<uint8_t, uint32_t>(file_content_uint8_t, 0x22);

            if (image_size_uint32_t > 2147483647)
            {
                std::cerr << "BMP file is too big, size: " << image_size_uint32_t << " bytes.\n";
                return nullptr;
            }

            image_size = static_cast<std::size_t>(image_size_uint32_t);
            std::cout << "image size is " << image_size << " bytes.\n";

            int32_t temp_image_width = yli::memory::read_nonaligned_32_bit<uint8_t, int32_t>(file_content_uint8_t, 0x12);

            if (temp_image_width < 0)
            {
                std::cerr << "image width is negative: " << temp_image_width << "\n";
                return nullptr;
            }

            image_width = static_cast<std::size_t>(temp_image_width);
            std::cout << "image width is " << image_width << " pixels.\n";

            int32_t temp_image_height = yli::memory::read_nonaligned_32_bit<uint8_t, int32_t>(file_content_uint8_t, 0x16);

            if (temp_image_height < 0)
            {
                std::cerr << "image height is negative: " << temp_image_height << "\n";
                return nullptr;
            }

            image_height = static_cast<std::size_t>(temp_image_height);
            std::cout << "image height is " << image_height << " pixels.\n";

            // Some BMP files are misformatted, guess missing information
            if (image_size == 0)
            {
                std::size_t number_of_pixels = image_width * image_height;
                std::cout << "image contains " << number_of_pixels << " pixels.\n";

                if (number_of_pixels > std::numeric_limits<std::size_t>::max() / 4)
                {
                    std::cerr << "BMP file is too big, number of pixels: " << number_of_pixels << "\n";
                    return nullptr;
                }

                image_size = number_of_pixels * 3; // 3 : one byte for each Red, Green and Blue component
            }

            // Create a buffer.
            uint8_t* image_data = new uint8_t[image_size];

            if (image_data == nullptr)
            {
                std::cerr << "Reserving memory for image data failed.\n";
                return nullptr;
            }

            std::cout << "copying image data ...\n";
            std::copy(file_content.begin() + pixel_array_start_offset, file_content.end(), image_data);
            std::cout << "image data copied.\n";

            return image_data;
        }
    }
}