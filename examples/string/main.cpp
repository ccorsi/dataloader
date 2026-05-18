/**
 * @file main.cpp
 * @author Claudio Corsi (clcorsi@yahoo.com)
 * @brief This will show you how to use the data loader template class to populate a string
 * @version 0.1
 * @date 2026-05-17
 *
 * @copyright Copyright (c) 2026 Claudio Corsi
 *
 * [MIT License](https://raw.githubusercontent.com/ccorsi/dataloader/main/LICENSE)
 */

#include <iostream>
#include <fstream>
#include <string>

#include "data_loader.h"

using namespace valhalla::loader;
using namespace valhalla::checkers;

namespace examples {

namespace string {

/**
 * @brief This string reader will read in the whole string altogether.
 *
 */
struct string_reader {
    /**
     * @brief This operator will be called to populate the passed string
     *
     * @param in The input stream containing the string
     * @param str A reference to the string to be populated
     * @param field Which field is being populated, in this case it is set to 0
     * @return std::istream& A reference to the passed input stream for chaining calls
     */
    std::istream & operator()(std::istream & in, std::string & str, int field) {
        // Get the first character from the passed input stream
        char chr = in.get();
        // loop through the input characters until we are done or none is left
        while (chr != std::string::traits_type::eof() && chr != '"') {
            chr = in.get();
            if (chr == '\\') {
                chr = in.get();
                if (chr == std::string::traits_type::eof()) {
                    return in;
                } // if (chr == std::string::traits_type::eof())
            } // if (chr == '\\')
            str += chr;
        } // while (chr != std::string::traits_type::eof() && chr != '"')
        // return a reference to the input stream for chaining calls
        return in;
    }
};

/**
 * @brief This string reader will read a single character at the time unless it
 *  encounters an escape character.
 *
 * This reader requires that the is_space_noop inner space operator is used.  If
 * not then the spaces will be removed from the string.
 *
 */
struct string_single_character_reader {
    /**
     * @brief This operator will be called to populate the passed string
     *
     * @param in A reference to the input stream contains the string data
     * @param str A reference to the string to be populated
     * @param field Which field is being populated, in this case 0
     * @return std::istream& A reference to the passed input stream to chain calls
     */
    std::istream & operator()(std::istream & in, std::string & str, int field) {
        char chr = in.get();

        if (chr != std::string::traits_type::eof()) {
            if (chr == '\\') {
                chr = in.get();
                if (chr == std::string::traits_type::eof()) {
                    return in;
                } // if (chr == std::string::traits_type::eof())
            } // if (chr == '\\')
            str += chr;
        } // if (chr != std::string::traits_type::eof())

        return in;
    }
};

} // namespace string
} // namespace examples

// @cond
int main(int argc, char** argv) {
    std::cout << "Calling string reader example\n";

    try {
        // create an input stream
        std::fstream in("data.txt");

        std::string value;

        // define the dataLoader for a string...
        dataLoader<
            std::string,
            char,
            examples::string::string_single_character_reader,
            1,
            is_character<char,'\'','"'>,
            is_character<char,'\'','"'>,
            is_space,
            is_space_noop<char> // This is required when using the string_single_character_reader reader
        > loader(value);
        // ...initialize the string value
        in >> loader;

        std::cout << "value = " << value << "\n";

        return 0;
    } catch (std::exception & ex ) {
        std::cout << "An exception was caught: " << ex.what() << "\n";
        return 1;
    } // catch (std::exception & ex )// try
}
// @endcond
