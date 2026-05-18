/**
 * @file main.cpp
 * @author Claudio Corsi (clcorsi@yahoo.com)
 * @brief This examples shows you how to use the data loader template class to populate container type instances
 * @version 0.1
 * @date 2026-05-17
 *
 * @copyright Copyright (c) 2026 Claudio Corsi
 *
 * [MIT License](https://raw.githubusercontent.com/ccorsi/dataloader/main/LICENSE)
 */

#include <iostream>
#include <fstream>
#include <vector>

#include "data_loader.h"

using namespace valhalla::loader;
using namespace valhalla::checkers;

namespace examples {
namespace repeat {

/**
 * @brief This class will store the values of the x and y axis of a point on a plain.
 *
 */
class point {
    /// @brief the x-axis value of the point
    int m_x,
        /// the y-axis value of the point
        m_y;
public:
    point() = default;
    /**
     * @brief Construct a new point object using the passed x and y values
     *
     * @param x The x-axis value of the plain
     * @param y The y-axis value of the plain
     */
    point(int x, int y) : m_x(x), m_y(y) {}

    /**
     * @brief This operator is used to display that data within a point instance
     *
     * @param out A reference to the output stream
     * @param pt A reference to the point to display
     * @return std::ostream& A reference to the passed output stream for chaining
     */
    friend std::ostream & operator<<(std::ostream & out, const point & pt) {
        return out << "point[x=" << pt.m_x << ",y=" << pt.m_y << "]";
    }
};

/**
 * @brief A reader struct used by the data loader template class to populate a
 *  point instance
 *
 */
struct point_reader {
    /// @brief The value of the point x-axis
    int m_x;
    /**
     * @brief This operator is used to populate the passed point instance using the
     *  passed input stream
     *
     * @param in A reference to the input stream
     * @param pt A reference to the point to be populated
     * @param field Used to determine which field is being processed
     * @return std::istream& A reference to the output stream for chaining
     */
    std::istream & operator()(std::istream & in, point & pt, int field) {
        switch (field) {
            case 0: // load x
                in >> m_x;
                break;
            case 1: // load y and initialize pt
            {
                int y;
                in >> y;
                pt = point(m_x,y);
                break;
            }
            default:
                throw std::runtime_error("INVALID STATE");
        }
        return in;
    }
};

/**
 * @brief This reader will be used by the data loader template class to populate a vector of point instances.
 *
 */
struct point_vector_reader {
    /**
     * @brief This operator will populate the passed point vector using the passed input stream
     *
     * @param in A reference to the input stream
     * @param vec A reference to the point vector
     * @param field Used to determine which field is being populated
     * @return std::istream& A reference to the passed input stream for chaining
     */
    std::istream & operator()(std::istream & in, std::vector<point> & vec, int field) {
        point pt;

        // define the dataLoader template class
        dataLoader<
            point,
            char,
            point_reader,
            2,
            is_character<char,'{'>,
            is_character<char,'}'>,
            is_space_or<','>
        > loader(pt);
        // initialize the point pt with the input data
        in >> loader;

        // add the point to the passed vector
        vec.push_back(pt);

        return in;
    }
};

} // namespace repeat
} // namespace examples

// @cond
int main(int argc, char** argv) {
    std::cout << "Calling repeat reader example\n";

    try {
        // create an input stream
        std::fstream in("data.txt");

        while ( ! in.eof() ) {
            std::vector<examples::repeat::point> points;

            // define the dataLoader for a int primitive...
            dataLoader<
                std::vector<examples::repeat::point>,
                char,
                examples::repeat::point_vector_reader,
                1,
                is_character<char,'['>,
                is_character<char,']'>,
                is_space_or<','>
            > loader(points);
            // ...initialize the points
            in >> loader;

            std::cout << "points = [ ";
            for (auto & pt : points)
                std::cout << pt << " ";
            std::cout << "]\n";
        } // while (in.peek() != in.eof())

        return 0;
    } catch (std::exception & ex) {
        std::cout << "An expection was caught: " << ex.what() << "\n";
        return 1;
    }
}
// @endcond
