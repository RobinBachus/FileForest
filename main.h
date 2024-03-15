#include <iostream>
#include <vector>
#include <filesystem>
#include <format>
#include <map>

using namespace std;

void display_files(string &c_dir, map<char, map<string, uintmax_t>> &dirs, bool get_size);
void display_size(uintmax_t size);
map<char, map<string, uintmax_t>> get_files_and_dirs(const string &path, const bool get_size);
void insert_sizes(map<char, map<string, uintmax_t>> &dirs, const string &path);
int get_last_index(string str, char target);
string shorten_path(const filesystem::path _path, const string c_dir);