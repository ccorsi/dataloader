/**
 * @file main.cpp
 * @author Claudio Corsi (clcorsi@yahoo.com)
 * @brief Simple Data Loader Template Class example populating a point vector
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
namespace pointvector {

/**
 * @brief A point class to store the x and y values
 *
 */
class point {
    int m_x, m_y;
public:
    point() = default;
    /**
     * @brief Construct a new point object using the passed x and y values
     *
     * @param x The value of the x-axis for the point
     * @param y The value of the y-axis for the point
     */
    point(int x, int y) : m_x(x), m_y(y) {}

    /**
     * @brief operator to used to print this point data to the output stream
     *
     * @param out A reference to the output stream
     * @param pt A reference to the point to be printed to the output stream
     * @return std::ostream& A reference to the passed output stream
     */
    friend std::ostream & operator<<(std::ostream & out, const point & pt) {
        return out << "point[x=" << pt.m_x << ",y=" << pt.m_y << "]";
    }
};

/**
 * @brief This reader will be used by the data loader template class to populate a point instance.
 *
 */
struct point_reader {
    /**
     * @brief attribute used to store the current x value of the point to be populated
     *
     */
    int m_x;
    /**
     * @brief operator called to populate a point instance
     *
     * @param in A reference to the input stream containing the data
     * @param pt A reference to the point instance to populate
     * @param field Used to determine which attribute of the point to populate
     * @return std::istream& A reference to the passed input stream
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

            // Always add a defensive check even when we know better
            default:
                throw std::runtime_error("INVALID STATE");
        }
        return in;
    }
};

/**
 * @brief Data Loader Template Class Reader operator for loading a point vector
 *
 */
struct point_vector_reader {
    /**
     * @brief This is the operator called by the data loader template class to populate the point vector
     *
     * @param in A reference to the input stream used to extract the data from
     * @param vec A reference to the point vector that will be populated with point instances
     * @param field This informs the reader which field is being populated
     * @return std::istream& A referene to the passed input stream
     */
    std::istream & operator()(std::istream & in, std::vector<point> & vec, int field) {
        switch (field) {
            case 0: {
                // define a local point instance that will be added to the passed point vector
                point pt;

                // define the dataLoader template class
                dataLoader<
                    point,
                    char,
                    point_reader,
                    2,                      // informs the data loader that we will be populating 2 fields in point
                    is_character<char,'{'>,
                    is_character<char,'}'>,
                    is_space_or<','>
                > loader(pt);
                // initialize the point pt with the input data
                in >> loader;

                // add the point to the passed vector
                vec.push_back(pt);
            }
                break;

            // Always add a defensive check even when we know better
            default:
                throw std::runtime_error("INVALID STATE");
        } // switch (field)

        // return a reference to the input stream for chaining purposes
        return in;
    }
};

} // namespace pointvector
} // namespace examples

// @cond
int main(int argc, char** argv) {
    std::cout << "Calling point vector reader example\n";

    try {
        // create an input stream
        std::fstream in("data.txt");

        std::vector<examples::pointvector::point> points;

        // define the dataLoader for a point vector...
        dataLoader<
            std::vector<examples::pointvector::point>,
            char,
            examples::pointvector::point_vector_reader,
            1,
            is_character<char,'['>,
            is_character<char,']'>,
            is_space,
            is_space_or<','>
        > loader(points);
        // ...initialize the point vector
        in >> loader;

        std::cout << "points = [ ";
        for (auto & pt : points)
            std::cout << pt << " ";
        std::cout << "]\n";

        return 0;
    } catch (std::exception & ex) {
        std::cout << "An expection was caught: " << ex.what() << "\n";
        return 1;
    }
}
// @endcond
