#include <iostream>

#include <gflags/gflags.h>

DEFINE_string(input_path, "", "Path to test file");

int main(int argc, char * argv[]) {
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    std::cout << "path: " << FLAGS_input_path << std::endl;
    return 0;
}
