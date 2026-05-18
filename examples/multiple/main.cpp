/**
 * @file main.cpp
 * @author Claudio Corsi (clcorsi@yahoo.com)
 * @brief This will show you how to use the data loader template class to populate a point instance using multiple passes
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
namespace multiple {

/**
 * @brief This contains the x and y axis value of a point on a plain
 *
 */
class point {
    /// @brief the value associated to the points x-axis
    int m_x,
        /// the value associated to the points y-axis
        m_y;
public:
    point() = default;
    /**
     * @brief Construct a new point object using the passed x and y values
     *
     * @param x the value of the points x-axis
     * @param y the value of the points y-axis
     */
    point(int x, int y) : m_x(x), m_y(y) {}

    /**
     * @brief This operator is used to display the values within a point instance
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
 * @brief The data loader reader used to load the x and y fields into
 *      the passed point instance.
 *
 */
struct point_reader {
    /**
     * @brief This will contain the x value of the populating point instance
     *
     */
    int x;

    /**
     * @brief This operator will be called whenever there is a field of the point
     *  instance will be populated.
     *
     * @details This operator is used to populate the reference point instance. It
     *          will use the passed field value to determine which field of the point
     *          instance is being processed.  In this case, the field parameter will
     *          contain the values 0 for the x attribute of the point. While when the
     *          field parameter is set to 1 will be associated to the y attribute of
     *          the point instance.  The point will be defined as soon as we've processed
     *          the y attribute of the point.
     *
     * @param in This is a reference to the input stream
     * @param p This is a reference to the point that will be populated
     * @param field This informs the operator which field is being populated
     * @return std::istream& A reference to the passed input stream for chaining
     */
    std::istream & operator()(std::istream & in, point & p, int field) {
        switch(field) {
            case 0: // load data for x field
                in >> x;
                break;
            case 1: // load data for y field
                {
                    int y;
                    in >> y;
                    // initialize the passed point with the loaded x & y data
                    p = point(x,y);
                }
                break;
            // Add a default case as a defense mechanism against changes to the point class
            default:
                throw std::runtime_error("INVALID field");
        }
        return in;
    }
};

} // namespace multiple
} // namespace examples

// @cond
int main(int argc, char** argv) {
    // create an input stream
    std::fstream in("data.txt");

    try {
        examples::multiple::point value;

        // define the dataLoader for a point instance...
        dataLoader<
            examples::multiple::point,
            char,
            examples::multiple::point_reader,
            2,
            is_character<char,'{'>,
            is_character<char, '}'>,
            is_space
        > loader(value);
        // ...initialize the value
        in >> loader;

        std::cout << "value = " << value << "\n";

        return 0;
    } catch (std::exception & ex) {
        std::cout << "An exception was caught: " << ex.what() << "\n";
        return 1;
    }
}
// @endcond
