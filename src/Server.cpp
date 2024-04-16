#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>
#include <algorithm>
#include <set>
#include <ctime>
#include <curl/curl.h>
#include "zlib_implement.h"

bool git_init (const std::string& dir) {
    std::cout << "git init \n";
    try {
        std::filesystem::create_directory(dir + "/.git");
        std::filesystem::create_directory(dir + "/.git/objects");
        std::filesystem::create_directory(dir + "/.git/refs");

        std::ofstream headFile(dir + "/.git/HEAD");
        if (headFile.is_open()) { // create .git/HEAD file
            headFile << "ref: refs/heads/master\n"; // write to the headFile
            headFile.close();
        } else {
            std::cerr << "Failed to create .git/HEAD file.\n";
            return false;
        }
       
        std::cout << "Initialized git directory in " << dir << "\n";
        return true;
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << e.what() << '\n';
        return false;
    }
}


int cat_file(const char* filepath) {
        FILE* dataFile = fopen(filepath, "rb");
        if (!dataFile) {
            std::cerr << "Invalid object hash.\n";
            return EXIT_FAILURE;
        }

        // create output file for standard output
        FILE* outputFile = fdopen(1, "wb");
        if (!outputFile) {
            std::cerr << "Failed to create output file.\n";
            return EXIT_FAILURE;
        }

        // decompress data file
        if (decompress(dataFile, outputFile) != EXIT_SUCCESS) {
            std::cerr << "Failed to decompress data file.\n";
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
}

std::string compute_sha1 (const std::string& data, bool print_out = false) {
    unsigned char hash[20]; // 160 bits long for SHA1
    SHA1(reinterpret_cast<const unsigned char*>(data.c_str()), data.size(), hash);
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (const auto& byte : hash) {
        ss << std::setw(2) << static_cast<int>(byte);
    }

    if (print_out)  {
        std::cout << ss.str() << std::endl;
    }

    return ss.str();
}

void compress_and_store (const std::string& hash, const std::string& content, std::string dir = ".") {
    FILE* input = fmemopen((void*) content.c_str(), content.length(), "rb");
    std::string hash_folder = hash.substr(0, 2);
    std::string object_path = dir + "/.git/objects/" + hash_folder + '/';
    if (!std::filesystem::exists(object_path)) {
        std::filesystem::create_directories(object_path);
    }
    
    std::string object_file_path = object_path + hash.substr(2);
    if (!std::filesystem::exists(object_file_path)) {
        FILE* output = fopen(object_file_path.c_str(), "wb");
        if (compress(input, output) != EXIT_SUCCESS) {
            std::cerr << "Failed to compress data.\n";
            return;
        }
        fclose(output);
    }

    fclose(input);
}

std::string hash_object (std::string filepath, std::string type = "blob", bool print_out = false) {
        // open the file
        std::ifstream inputFile(filepath, std::ios::binary);
        if(inputFile.fail()) {
            std::cerr << "Failed to open file.\n";
            return {};
        }

        // read the file
        std::string content(
            (std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>()
        );

        // create the content
        std::string header = type + " " + std::to_string(content.size());
        std::string file_content = header + '\0' + content;

        std::string hash = compute_sha1(file_content, false);

        compress_and_store(hash, file_content);
        inputFile.close();

        if (print_out) {
            std::cout << hash << std::endl;
        }
        return hash;
}

std::set<std::string> parse_tree_object (FILE* tree_object) {
    rewind(tree_object); // set the file position indicator to the beginning of the file
    
    std::vector<std::string> unsorted_directories;
    char mode[7];
    char filename[256];
    unsigned char hash[20];
    while (fscanf(tree_object, "%6s", mode) != EOF) {
        // read the filename (up to the null byte)
        int i = 0;
        int c;
        while ((c = fgetc(tree_object)) != 0 && c != EOF) {
            // if the character is a blank space, continue
            if (c == ' ') {
                continue;
            }
            filename[i++] = c;
        }
        filename[i] = '\0'; // null-terminate the filename

        // read the hash
        fread(hash, 1, 20, tree_object);

        unsorted_directories.push_back(filename);
    }

    std::sort(unsorted_directories.begin(), unsorted_directories.end()); // sort the directories lexicographically
    std::set<std::string> sorted_directories(unsorted_directories.begin(), unsorted_directories.end()); // remove duplicates

    return sorted_directories;
}

int ls_tree (const char* object_hash) {
    // retrieve the object path
    char object_path[64];
    snprintf(object_path, sizeof(object_path), ".git/objects/%.2s/%s", object_hash, object_hash + 2);

    // set the input and output file descriptors
    FILE* object_file = fopen(object_path, "rb");
    if(object_file == NULL) {
        std::cerr << "Invalid object hash.\n";
        return EXIT_FAILURE;
    }
    FILE* output_file = tmpfile();
    if(output_file == NULL) {
        std::cerr << "Failed to create output file.\n";
        return EXIT_FAILURE;
    }

    if(decompress(object_file, output_file) != EXIT_SUCCESS) {
        std::cerr << "Failed to decompress object file.\n";
        return EXIT_FAILURE;
    }

    std::set<std::string> directories = parse_tree_object(output_file);

    // print the directories
    for (const std::string& directory : directories) {
        std::cout << directory << '\n';
    }

    return EXIT_SUCCESS;
}

std::string hash_digest (const std::string& input) {
    std::string condensed;

    for (size_t i = 0; i < input.length(); i += 2) {
        std::string byte_string = input.substr(i, 2);
        char byte = static_cast<char>(std::stoi(byte_string, nullptr, 16));

        condensed.push_back(byte);
    }

    return condensed;
}

std::string write_tree (const std::string& directory) {
    std::vector<std::string> tree_entries;
    std::vector<std::string> skip = {
        ".git", "server", "CMakeCache.txt", 
        "CMakeFiles", "Makefile", "cmake_install.cmake"
    };

    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        std::string path = entry.path().string();
        
        if (std::any_of(skip.begin(), skip.end(), [&path](const std::string& s) {
            return path.find(s) != std::string::npos;
        })) {
            continue;
        }

        std::error_code ec;
        std::string entry_type = std::filesystem::is_directory(path, ec) ? "40000 " : "100644 ";
        std::string relative_path = path.substr(path.find(directory) + directory.length() + 1);
        std::string hash = std::filesystem::is_directory(path, ec) ?
                           hash_digest(write_tree(path.c_str())):
                           hash_digest(hash_object(path.c_str(), "blob", false));
        
        tree_entries.emplace_back(path + '\0' + entry_type + relative_path + '\0' + hash);
    }

    // sort the entries based on the absolute path
    std::sort(tree_entries.begin(), tree_entries.end());

    // delete the path from the beginning of each entry
    int bytes = 0;
    for (auto& entry : tree_entries) {
        entry = entry.substr(entry.find('\0') + 1);
        bytes += entry.length();
    }

    // concatenate the entries
    std::string tree_content = "tree " + std::to_string(bytes) + '\0';
    for (const auto& entry : tree_entries) {
        tree_content += entry;
    }

    // storing the tree object
    std::string tree_hash = compute_sha1(tree_content, false);
    compress_and_store(tree_hash.c_str(), tree_content);

    return tree_hash;
}

std::string commit_tree (std::string tree_sha, std::string parent_sha, std::string message) {
    std::string author = "John Doe <john.doe@gmail.com>";
    std::string committer = "John Doe <john.doe@gmail.com>";
    std::string timestamp = std::to_string(std::time(nullptr));

    std::string commit_content = "tree " + tree_sha + "\n" +
                                 "parent " + parent_sha + "\n" +
                                 "author " + author + " " + timestamp + " -0800\n" +
                                 "committer " + committer + " " + timestamp + " -0800\n" +
                                 "\n" + message + "\n";
    

    std::string header = "commit " + std::to_string(commit_content.length()) + '\0';
    commit_content = header + commit_content;


    std::string commit_hash = compute_sha1(commit_content, false);
    compress_and_store(commit_hash.c_str(), commit_content);

    return commit_hash;
}

// curl helper function
size_t write_callback (void* received_data, size_t element_size, size_t num_element, void* userdata) {
    size_t total_size = element_size * num_element;
    std::string received_text((char*) received_data, num_element);

    std::string* master_hash = (std::string*) userdata;
    if (received_text.find("servie=git-upload-pack") == std::string::npos) {
        size_t hash_pos = received_text.find("refs/heads/master\n");
        if (hash_pos != std::string::npos) {
            *master_hash = received_text.substr(hash_pos - 41, 40);
        }
    }

    return total_size;
}

// curl helper function
size_t pack_data_callback (void* received_data, size_t element_size, size_t num_element, void* userdata) {
    std::string* accumulated_data = (std::string*) userdata;
    *accumulated_data += std::string((char*) received_data, num_element);

    return element_size * num_element;
}

std::pair<std::string, std::string> curl_request (const std::string& url) {
    CURL* handle = curl_easy_init();
    if (handle) {
        // fetch info/refs
        curl_easy_setopt(handle, CURLOPT_URL, (url + "/info/refs?service=git-upload-pack").c_str());
        
        std::string packhash;
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*) &packhash);
        curl_easy_perform(handle);
        curl_easy_reset(handle);

        // fetch git-upload-pack
        curl_easy_setopt(handle, CURLOPT_URL, (url + "/git-upload-pack").c_str());
        std::string postdata = "0032want " + packhash + "\n" +
                               "00000009done\n";
        curl_easy_setopt(handle, CURLOPT_POSTFIELDS, postdata.c_str());

        std::string pack;
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*) &pack);
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, pack_data_callback);

        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/x-git-upload-pack-request");
        curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);
        curl_easy_perform(handle);

        // clean up
        curl_easy_cleanup(handle);
        curl_slist_free_all(headers);

        return {pack, packhash};
    }
    else {
        std::cerr << "Failed to initialize curl.\n";
        return {};
    }
}

// convert git hash digest to hash
std::string digest_to_hash (const std::string& digest) {
    std::stringstream ss;
    for (unsigned char c : digest) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c);
    }

    return ss.str();
}

int read_length (const std::string& pack, int* pos) {
    int length = 0;

    // extract the lower 4 bits of the first byte
    length |= pack[*pos] & 0x0F;

    // if the MSB is set, read the next byte
    if (pack[*pos] & 0x80) {
        (*pos)++;

        while (pack[*pos] & 0x80) {
            length <<= 7;
            length |= pack[*pos] & 0x7F;
            (*pos)++;
        }

        // read the last byte
        length <<= 7;
        length |= pack[*pos];
    }

    (*pos)++; // move to the next position

    return length;
}

std::string apply_delta (const std::string& delta_contents, const std::string& base_contents) {
    std::string reconstructed_object;
    int current_position_in_delta = 0;

    // read and skip the length of the base object
    read_length(delta_contents, &current_position_in_delta);
    read_length(delta_contents, &current_position_in_delta);

    // iterate through the delta contents
    while (current_position_in_delta < delta_contents.length()) {
        unsigned char current_instruction = delta_contents[current_position_in_delta++];

        // check if the highest bit of the instruction byte is set
        if (current_instruction & 0x80) {
            int copy_offset = 0;
            int copy_size = 0;
            int bytes_processed_for_offset = 0;

            // calculate copy offset from the delta contents
            for (int i = 3; i >= 0; i--) {
                copy_offset <<= 8;
                if (current_instruction & (1 << i)) {
                    copy_offset += static_cast<unsigned char>(delta_contents[current_position_in_delta + i]);
                    bytes_processed_for_offset++;
                }
            }

            int bytes_processed_for_size = 0;
            // calculate copy size from the delta contents
            for (int i = 6; i >= 4; i--) {
                copy_size <<= 8;
                if (current_instruction & (1 << i)) {
                    copy_size += static_cast<unsigned char>(delta_contents[current_position_in_delta + i - (4 - bytes_processed_for_offset)]);
                    bytes_processed_for_size++;
                }
            }

            // default size to 0x100000 if no size was specified
            if (copy_size == 0) {
                copy_size = 0x100000;
            }

            // append the copied data from base contents to the reconstructed object
            reconstructed_object += base_contents.substr(copy_offset, copy_size);
            current_position_in_delta += bytes_processed_for_size + bytes_processed_for_offset;
        }
        else {
            // direct add insturction, the highest bit is not set
            int add_size = current_instruction & 0x7F;
            reconstructed_object += delta_contents.substr(current_position_in_delta, add_size);
            current_position_in_delta += add_size;
        }
    }

    return reconstructed_object;
}

int cat_file_for_clone(const char* file_path, const std::string& dir, FILE* dest, bool print_out = false) {
    try {
        std::string blob_sha = file_path;
        std::string blob_path = dir + "/.git/objects/" + blob_sha.insert(2, "/");
        if (print_out) std::cout << "blob path: " << blob_path << std::endl;

        FILE* blob_file = fopen(blob_path.c_str(), "rb");
        if (blob_file == NULL) {
            std::cerr << "Invalid object hash.\n";
            return EXIT_FAILURE;
        }

        decompress(blob_file, dest);
        fclose(blob_file);
    }
    catch (const std::filesystem::filesystem_error& e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void restore_tree (const std::string& tree_hash, const std::string& dir, const std::string& proj_dir) {
    // construct the path to the tree object
    std::string object_path = proj_dir + "/.git/objects/" + tree_hash.substr(0, 2) + '/' + tree_hash.substr(2);
    std::ifstream master_tree(object_path);

    // read the contents of the tree object into a buffer
    std::ostringstream buffer;
    buffer << master_tree.rdbuf();

    // decompress the tree object
    std::string tree_contents = decompress_string(buffer.str());

    // skip the metadata part of the tree object
    tree_contents = tree_contents.substr(tree_contents.find('\0') + 1);

    // iterate over each entry in the tree object
    int pos = 0;
    while (pos < tree_contents.length()) {
        if (tree_contents.find("40000", pos) == pos) {
            pos += 6; // skip the mode 40000

            // extract the directory path
            std::string path = tree_contents.substr(pos, tree_contents.find('\0', pos) - pos);
            pos += path.length() + 1; // skip the path and the null byte

            // extract the hash of the nested tree object
            std::string next_hash = digest_to_hash(tree_contents.substr(pos, 20));

            // create directories and recursively restore the nested tree
            std::filesystem::create_directory(dir + '/' + path);
            restore_tree(next_hash, dir + '/' + path, proj_dir);
            pos += 20; // skip the hash
        }
        else {
            pos += 7; // skip the mode 100644

            // extract the file path
            std::string path = tree_contents.substr(pos, tree_contents.find('\0', pos) - pos);
            pos += path.length() + 1; // skip the path and the null byte

            // extract the hash of the blob object
            std::string blob_hash = digest_to_hash(tree_contents.substr(pos, 20));

            // create the file and write its contents
            FILE* new_file = fopen((dir + '/' + path).c_str(), "wb");
            cat_file_for_clone(blob_hash.c_str(), proj_dir, new_file);
            fclose(new_file);
            pos += 20; // skip the hash
        }
    }
}

int clone (std::string url, std::string dir) {
    // create the repository directory and initialize it
    std::filesystem::create_directory(dir);
    if (git_init(dir) != true) {
        std::cerr << "Failed to initialize git repository.\n";
        return EXIT_FAILURE;
    }

    // fetch the repository
    auto [pack, packhash] = curl_request(url);

    // parse the pack file
    int num_objects = 0;
    for (int i=16; i<20; i++) {
        num_objects = num_objects << 8;
        num_objects = num_objects | (unsigned char) pack[i];
    }
    pack = pack.substr(20, pack.length() - 40); // removing the headers of HTTP

    // proecessing object files in a git pack file
    int object_type;
    int current_position = 0;
    std::string master_commit_contents;
    for (int object_index = 0; object_index < num_objects; object_index++) {
        // extract object type from the first byte
        object_type = (pack[current_position] & 112) >> 4; // 112 is 11100000

        // read the object's length
        int object_length = read_length(pack, &current_position);

        // process based on object type
        if (object_type == 6) { // offset deltas: ignore it
            throw std::invalid_argument("Offset deltas not implemented.\n");
        }
        else if (object_type == 7) { // reference deltas
            // process reference deltas
            std::string digest = pack.substr(current_position, 20);
            std::string hash = digest_to_hash(digest);
            current_position += 20;

            // read the base object's contents
            std::ifstream file(dir + "/.git/objects/" + hash.insert(2, "/"));
            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string file_contents = buffer.str();

            std::string base_object_contents = decompress_string(file_contents);
            
            // extract and remove the object type
            std::string object_type_extracted = base_object_contents.substr(0, base_object_contents.find(" "));
            base_object_contents = base_object_contents.substr(base_object_contents.find('\0') + 1);

            // apply delta to base object
            std::string delta_contents = decompress_string(pack.substr(current_position));
            std::string reconstructed_contents = apply_delta(delta_contents, base_object_contents);

            // reconstruct the object with its type and length
            reconstructed_contents = object_type_extracted + ' ' + std::to_string(reconstructed_contents.length()) + '\0' + reconstructed_contents;

            // compute the object hash and store it
            std::string object_hash = compute_sha1(reconstructed_contents);
            compress_and_store(object_hash.c_str(), reconstructed_contents, dir);

            // advance position past the delta data
            std::string compressed_delta = compress_string(delta_contents);
            current_position += compressed_delta.length();

            // update master commits if hash matches
            if (hash.compare(packhash) == 0) {
                master_commit_contents = reconstructed_contents.substr(reconstructed_contents.find('\0'));
            }
        }
        else { // other object types (1: commit, 2: tree, other: blob)
            // process standard objects
            std::string object_contents = decompress_string(pack.substr(current_position));
            current_position += compress_string(object_contents).length();

            // prepare object header
            std::string object_type_str = (object_type == 1) ? "commit " : (object_type == 2) ? "tree " : "blob ";
            object_contents = object_type_str + std::to_string(object_contents.length()) + '\0' + object_contents;

            // store the object and update master commits if hash matches
            std::string object_hash = compute_sha1(object_contents, false);
            std::string compressed_object = compress_string(object_contents);
            compress_and_store(object_hash.c_str(), object_contents, dir);
            if (object_hash.compare(packhash) == 0) {
                master_commit_contents = object_contents.substr(object_contents.find('\0'));
            }
        }
    }

    // restore the tree
    std::string tree_hash = master_commit_contents.substr(master_commit_contents.find("tree") + 5, 40);
    restore_tree(tree_hash, dir, dir);

    return EXIT_SUCCESS;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "No command provided.\n";
        return EXIT_FAILURE;
    }

    std::string command = argv[1];

    if (command == "init") {
        if (git_init(".") != true) {
            std::cerr << "Failed to initialize git repository.\n";
            return EXIT_FAILURE;
        }
    }
    else if (command == "cat-file") {
        // check if object hash is provided
        if (argc < 3) {
            std::cerr << "No object hash provided.\n";
            return EXIT_FAILURE;
        }

        // retrieve file path and check if object hash is valid
        char dataPath[64];
        snprintf(dataPath, sizeof(dataPath), ".git/objects/%.2s/%s", argv[3], argv[3] + 2);
        if (cat_file(dataPath) != EXIT_SUCCESS) {
            std::cerr << "Failed to retrieve object.\n";
            return EXIT_FAILURE;
        }
    }
    else if (command == "hash-object") {
        // check if file path is provided
        if (argc < 4) {
            std::cerr << "No file path provided.\n";
            return EXIT_FAILURE;
        }

        // retrieve file name
        std::string fileName = argv[3];

        // hash the object
        std::string hash = hash_object(fileName, "blob", false);
        if (hash.empty()) {
            std::cerr << "Failed to hash object.\n";
            return EXIT_FAILURE;
        }

        std::cout << hash << std::endl;
    }
    else if (command == "ls-tree") {
        if (argc < 4) {
            std::cerr << "No object hash provided.\n";
            return EXIT_FAILURE;
        }

        // retrieve file path and check if object hash is valid
        std::string objectHash = argv[3];
        if (ls_tree(objectHash.c_str()) != EXIT_SUCCESS) {
            std::cerr << "Failed to retrieve object.\n";
            return EXIT_FAILURE;
        }
    }
    else if (command == "write-tree") {
        if (argc < 2) {
            std::cerr << "No command provided.\n";
            return EXIT_FAILURE;
        }

        std::filesystem::path current_path = std::filesystem::current_path();
        std::string tree_hash = write_tree(current_path.string());
        std::cout << tree_hash << std::endl;
    }
    else if (command == "commit-tree") {
        if (argc < 7) {
            std::cerr << "Too few arguments.\n";
            return EXIT_FAILURE;
        }

        std::string tree_sha = argv[2];
        std::string parent_sha = argv[4];
        std::string message = argv[6];
        
        std::string commit_hash = commit_tree(tree_sha, parent_sha, message);
        std::cout << commit_hash << std::endl;
    }
    else if (command == "clone") {
        if (argc < 3) {
            std::cerr << "No repository provided.\n";
            return EXIT_FAILURE;
        }

        std::string url = argv[2];
        std::string directory = argv[3];

        if (clone(url, directory) != EXIT_SUCCESS) {
            std::cerr << "Failed to clone repository.\n";
            return EXIT_FAILURE;
        }
    }
    else {
        std::cerr << "Unknown command " << command << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
