/**
 * @file main.cpp
 * @author Claudio Corsi (clcorsi@yahoo.com)
 * @brief This example shows how to use the data loader template class to load a single point instance
 * @version 0.1
 * @date 2026-05-17
 *
 * @copyright Copyright (c) 2026 Claudio Corsi
 *
 * [MIT License](https://raw.githubusercontent.com/ccorsi/dataloader/main/LICENSE)
 */

#include <iostream>
#include <fstream>

#include "data_loader.h"

using namespace valhalla::loader;
using namespace valhalla::checkers;

namespace examples {
namespace single {

/**
 * @brief A class that will contain the x and y axis values of a point on a plain.
 *
 */
class point {
    /// @brief The value of the x-axis
    int m_x,
        /// The value of the y-axis
        m_y;
public:
    point() = default;
    /**
     * @brief Construct a new point object using the passed x and y values
     *
     * @param x The value of the point x-axis
     * @param y The value of the point y-axis
     */
    point(int x, int y) : m_x(x), m_y(y) {}

    /**
     * @brief This operator will display the contents of the passed point to the passed
     *  output stream
     *
     * @param out A reference to the output stream
     * @param pt A reference to the point to be displayed
     * @return std::ostream& A reference to the passed output stream for chaining
     */
    friend std::ostream & operator<<(std::ostream & out, const point & pt) {
        return out << "point[x=" << pt.m_x << ",y=" << pt.m_y << "]";
    }
};

/**
 * @brief This reader will load the x and y values of the passed point
 *      within a single call instead of two seperate calls.
 *
 */
struct point_reader {
    /**
     * @brief Operator used to populate the passed point using the passed input stream.
     *
     * @param in A reference to the input stream
     * @param p A reference to the point
     * @param field Used to determine which attribute in the point to populate
     * @return std::istream& A reference to the passed input stream for chaining
     */
    std::istream & operator()(std::istream & in, point & p, int field) {
        int x;
        // load data for the x field
        in >> x;

        int y;
        // load data for the y field
        in >> y;

        // initialize the passed point with the loaded x & y data
        p = point(x,y);

        return in;
    }
};

} // namespace single
} // namespace examples

// @cond
int main(int argc, char** argv) {
    std::cout << "Calling single reader example\n";

    try {
        // create an input stream
        std::fstream in("data.txt");

        examples::single::point value;

        // define the dataLoader for a point instance...
        dataLoader<
            examples::single::point,
            char,
            examples::single::point_reader,
            1,
            is_character<char,'{'>,
            is_character<char,'}'>,
            is_space
        > loader(value);
        // ...initialize the point value
        in >> loader;

        std::cout << "value = " << value << "\n";

        return 0;
    } catch (std::exception & ex) {
        std::cout << "An exception was caught: " << ex.what() << "\n";
        return 1;
    } // catch (std::exception & ex)
}
// @endcond
