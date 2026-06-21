cmake_minimum_required(VERSION 3.10.0)
project("Data Loader Examples"
    VERSION 0.9.6
    DESCRIPTION "Template Based Data Loader Class Examples"
    HOMEPAGE_URL "https://github.com/ccorsi/dataloader/tree/main"
    LANGUAGES C CXX)

include(CTest)
# This is required to be able to have cmake generate the required test build files
enable_testing()

# include the different examples
add_subdirectory(simple)
add_subdirectory(single)
add_subdirectory(multiple)
add_subdirectory(intvector)
add_subdirectory(pointvector)
add_subdirectory(protocol)
add_subdirectory(repeat)
add_subdirectory(string)
