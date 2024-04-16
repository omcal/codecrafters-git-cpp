#include <iostream>
#include <cstdlib>
#include <cstring>
#include <zlib.h>
#include <sstream>
#include <stdexcept>
#include "zlib_implement.h"

#define CHUNK 16384 //16KB

int decompress(FILE* input, FILE* output) {
    // initialize decompression stream
    //std::cout << "Initializing decompression stream.\n";
    z_stream stream = {0};
    if (inflateInit(&stream) != Z_OK) {
        std::cerr << "Failed to initialize decompression stream.\n";
        return EXIT_FAILURE;
    }

    // initialize decompression variables
    char in[CHUNK];
    char out[CHUNK];
    bool haveHeader = false;
    char header[64];
    int ret;

    do {
        stream.avail_in = fread(in, 1, CHUNK, input); // read from input file
        stream.next_in = reinterpret_cast<unsigned char*>(in); // set input stream
        if (ferror(input)) {
            std::cerr << "Failed to read from input file.\n";
            return EXIT_FAILURE;
        }
        if (stream.avail_in == 0) {
            break;
        }

        do {
            stream.avail_out = CHUNK; // set output buffer size
            stream.next_out = reinterpret_cast<unsigned char*>(out); // set output stream
            ret = inflate(&stream, Z_NO_FLUSH); // decompress
            if (ret == Z_NEED_DICT || ret == Z_DATA_ERROR || ret == Z_MEM_ERROR) {
                std::cerr << "Failed to decompress file.\n";
                return EXIT_FAILURE;
            }

            // write header to output file
            unsigned headerLen = 0, dataLen = 0;
            if (!haveHeader) {
                sscanf(out, "%s %u", header, &dataLen);
                haveHeader = true;
                headerLen = strlen(out) + 1;
            }
            // write decompressed data to output file
            if (dataLen > 0) {
                if(fwrite(out + headerLen, 1, dataLen, output) != dataLen) {
                    std::cerr << "Failed to write to output file.\n";
                    return EXIT_FAILURE;
                }
            }
        } while (stream.avail_out == 0);
        
    } while (ret != Z_STREAM_END);

    return inflateEnd(&stream) == Z_OK ? EXIT_SUCCESS : EXIT_FAILURE;
}

int compress(FILE* input, FILE* output) {
    // Initialize compression stream.
    //std::cout << "Initializing compression stream.\n";
    z_stream stream = {0};
    if (deflateInit(&stream, Z_DEFAULT_COMPRESSION) != Z_OK) {
        std::cerr << "Failed to initialize compression stream.\n";
        return EXIT_FAILURE;
    }

    char in[CHUNK];
    char out[CHUNK];
    int ret;
    int flush;

    do {
        stream.avail_in = fread(in, 1, CHUNK, input);
        stream.next_in = reinterpret_cast<unsigned char*>(in);
        if (ferror(input)) {
            (void)deflateEnd(&stream);  // Free memory
            std::cerr << "Failed to read from input file.\n";
            return EXIT_FAILURE;
        }
        flush = feof(input) ? Z_FINISH : Z_NO_FLUSH;

        do {
            stream.avail_out = CHUNK;
            stream.next_out = reinterpret_cast<unsigned char*>(out);
            ret = deflate(&stream, flush);
            if (ret == Z_NEED_DICT || ret == Z_DATA_ERROR || ret == Z_MEM_ERROR) {
                (void)deflateEnd(&stream);  // Free memory
                std::cerr << "Failed to compress file.\n";
                return EXIT_FAILURE;
            }
            size_t have = CHUNK - stream.avail_out;
            if (fwrite(out, 1, have, output) != have || ferror(output)) {
                (void)deflateEnd(&stream);  // Free memory
                std::cerr << "Failed to write to output file.\n";
                return EXIT_FAILURE;
            }
        } while (stream.avail_out == 0);
    } while (flush != Z_FINISH);

    // Clean up and check for errors
    if (deflateEnd(&stream) != Z_OK) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

std::string decompress_string (const std::string& compressed_str) {
    z_stream d_stream;
    memset(&d_stream, 0, sizeof(d_stream));

    if (inflateInit(&d_stream) != Z_OK) {
        throw(std::runtime_error("inflateInit failed while decompressing."));
    }

    d_stream.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(compressed_str.data()));
    d_stream.avail_in = compressed_str.size();

    int status;
    const size_t buffer_size = 32768; // 32KB
    char buffer[buffer_size];
    std::string decompressed_str;

    do {
        d_stream.next_out = reinterpret_cast<Bytef*>(buffer);
        d_stream.avail_out = buffer_size;

        status = inflate(&d_stream, 0);

        if (decompressed_str.size() < d_stream.total_out) {
            decompressed_str.append(buffer, d_stream.total_out - decompressed_str.size());
        }
    } while (status == Z_OK);

    if (inflateEnd(&d_stream) != Z_OK) {
        throw(std::runtime_error("inflateEnd failed while decompressing."));
    }

    if (status != Z_STREAM_END) {
        std::ostringstream oss;
        oss << "Exception during zlib decompression: (" << status << ") " << d_stream.msg;
        throw(std::runtime_error(oss.str()));
    }

    return decompressed_str;
}

std::string compress_string (const std::string& input_str) {
    z_stream c_stream;
    memset(&c_stream, 0, sizeof(c_stream));

    if (deflateInit(&c_stream, Z_DEFAULT_COMPRESSION) != Z_OK) {
        throw(std::runtime_error("deflateInit failed while compressing."));
    }

    c_stream.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(input_str.data()));
    c_stream.avail_in = input_str.size();

    int status;
    const size_t buffer_size = 32768; // 32KB
    char buffer[buffer_size];
    std::string compressed_str;

    do {
        c_stream.next_out = reinterpret_cast<Bytef*>(buffer);
        c_stream.avail_out = sizeof(buffer);

        status = deflate(&c_stream, Z_FINISH);

        if (compressed_str.size() < c_stream.total_out) {
            compressed_str.append(buffer, c_stream.total_out - compressed_str.size());
        }
    } while (status == Z_OK);

    if (deflateEnd(&c_stream) != Z_OK) {
        throw(std::runtime_error("deflateEnd failed while compressing."));
    }

    if (status != Z_STREAM_END) {
        std::ostringstream oss;
        oss << "Exception during zlib compression: (" << status << ") " << c_stream.msg;
        throw(std::runtime_error(oss.str()));
    }

    return compressed_str;
}
