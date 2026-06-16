/**
 * @file main.cpp
 * @author Claudio Corsi (clcorsi@yahoo.com)
 * @brief This example shows how to use the data loader template class to populate instances using binary input data.
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
#include <vector>
#include <cstdint>
#include <random>

#include "data_loader.h"

using namespace valhalla::loader;
using namespace valhalla::checkers;

namespace examples {
namespace protocol {

/**
 * @brief This point class will be used to store the (x,y) value of a point
 *      on a plane
 *
 */
class point {
    uint16_t m_x, m_y;
public:
    point() = default;
    /**
     * @brief Construct a new point object using the passed values
     *
     * @param x value associated to the x axis point
     * @param y value associated to the y axis point
     */
    point(uint16_t x, uint16_t y) : m_x(x), m_y(y) {}

    /**
     * @brief This operator is used to display the data associated to a point instance
     *
     * @param out A reference to the output stream
     * @param pt A reference to the point to display
     * @return std::ostream& A reference to the passed output stream for chaining
     */
    friend std::ostream& operator<<(std::ostream& out, const point & pt) {
        return out << "(" << pt.m_x << "," << pt.m_y << ")";
    }
};

/**
 * @brief This class stores that data associated to a binary file
 *
 */
class header {
    uint16_t m_major_version,
        m_minor_version,
        m_patch_version;
    std::vector<point> m_points;
public:
    header() = default;
    /**
     * @brief Construct a new header object using the passed data
     *
     * @param major the major value associated to this header
     * @param minor the minor value associated to this header
     * @param patch the patch value associated to this header
     * @param points A array of points associated to this header
     */
    header(uint16_t major, uint16_t minor, uint16_t patch, std::vector<point> & points) :
        m_major_version(major), m_minor_version(minor), m_patch_version(patch),
        m_points(points) {}

    /**
     * @brief This operator is used to display a header instance using the passed output stream
     *
     * @param out A reference to the output stream
     * @param hdr A reference to the header instance to display
     * @return std::ostream& A reference to the passed output stream for chaining
     */
    friend std::ostream& operator<<(std::ostream& out, const header & hdr) {
        out << "header[major=" << hdr.m_major_version << ",minor="
            << hdr.m_minor_version << ", patch=" << hdr.m_patch_version
            << ", points=[ ";
        for (auto & pt : hdr.m_points)
            out << pt << " ";
        return out << "]]";
    }
};

/**
 * @brief Defines the magic number associated to the generated binary data file
 *
 */
const uint8_t magic[]{
    static_cast<uint8_t>(0x12),
    static_cast<uint8_t>(0x34),
    static_cast<uint8_t>(0x56),
    static_cast<uint8_t>(0x78)
};

/**
 * @brief This function is called to load an unsigned integer of two bytes
 *  from a binary input stream into the referenced value that is passed.
 *
 * @param in A reference to the input stream with the binary data
 * @param value A reference to the value that will be populated
 * @return std::istream& A reference to the passed input stream for chaining
 */
std::istream& load_uint16_t(std::istream & in, uint16_t & value) {
    // clear the current quantity of the passed value;
    value = 0;

    for (int cnt = 0 ; cnt < 2 ; cnt++) {
        // get the next byte from the input stream...
        uint8_t chunk = in.get();
        // ...determine if you've reached the eof
        if (in.eof()) {
            throw std::runtime_error("Insufficent data to produce uint16_t value");
        } // if (in.eof())
        // multiple by 256...
        value >>= 8;
        // ...add current chunk of the value
        value += chunk;
    } // for (int cnt = 0 ; cnt < 2 ; cnt++)

    return in;
}

/**
 * @brief This reader will be used by the data loader template class to populate a point instance
 *
 */
struct point_reader {
    /// the x value axis of the point instance
    uint16_t x,
        /// the y value axis of the point instance
        y;

    /**
     * @brief This operator is called to populate the passed point from the passed
     *  input stream of binary data.  It uses the passed field value to determine
     *  which point field is being read.  It will then return a reference to the
     *  passed input stream for chaining.
     *
     * @param in A reference to the input stream
     * @param pt A reference to the point instane to be populated
     * @param field Used to determine which field is being processed
     * @return std::istream& A reference to the passed input stream for chaining
     */
    std::istream& operator()(std::istream& in, point & pt, int field) {
        switch(field) {
            case 0: // load x
                return load_uint16_t(in, x);
            case 1: // load y
            {
                load_uint16_t(in, y);
                pt = point(x,y);
                return in;
            }
            // The above statement is similar to the following compact statement
            // return (load_uint16_t(in, y), pt = point(x,y), in);
            default:
                throw std::runtime_error("invalid field");
        } // switch(field)

        return in;
    }
};

/**
 * @brief This reader will be used to populate a header instance
 *
 */
struct header_reader {
    /// @brief contains the major value associated to the header
    uint16_t major,
        /// contains the minor value associated to the header
        minor,
        /// contains the patch value associated to the header
        patch,
        /// contains the number of points associated to the header
        count;
    /// @brief contains the points associated to the header
    std::vector<point> points;

    /**
     * @brief This operator is used to populate the passed header instance using
     *  the passed input stream and field value.
     *
     * @param in A reference to the input stream
     * @param hdr A reference to the header instance to be populated
     * @param field A value used to determine which field to populate
     * @return std::istream& A reference to the passed input stream
     */
    std::istream& operator()(std::istream& in, header & hdr, int field) {
        switch(field) {
            case 0: // read header
            {
                const int size = (sizeof(magic) / sizeof(magic[0]));

                for (auto v : magic)
                    if (static_cast<uint8_t>(in.get()) != v)
                        throw std::runtime_error("Invalid header type");

                return in;
            }
            case 1: // read major
                return load_uint16_t(in, major);
            case 2: // read minor
                return load_uint16_t(in, minor);
            case 3: // read patch
                return load_uint16_t(in, patch);
            case 4: // read count
                return load_uint16_t(in, count);
            case 5: // read point
            {
                // std::cout << "count = " << count << "\n";
                while (count-- > 0) {
                    point pt;
                    // initialize a point data loader instance...
                    dataLoader<
                        point,
                        char,
                        point_reader,
                        2,
                        is_character_noop<char>,
                        is_no_character<char>,
                        is_space_noop<char>
                    > loader(pt);
                    // ... populate the point instance
                    in >> loader;

                    // add the populated point instance to the point vector
                    points.push_back(pt);
                } // while (count-- > 0)

                hdr = header(major, minor, patch, points);

                return in;
            }
            default:
                throw std::runtime_error("Invalid field");
        }
    }
};

/**
 * @brief This function is used to convert the passed unsigned integer of two bytes
 *  to the passed output stream.
 *
 * @param v The value that will be sent to the output stream
 * @param out A reference to the output stream
 */
void store_uint16_t(uint16_t v, std::fstream &out) {
    uint8_t e = (v & 0xff) >> 8;
    out.put(e);
    // std::cout << "Stored: '0x" << std::hex << static_cast<uint16_t>(e) << std::dec;
    e = v & 0xff;
    // std::cout << "', '0x" << std::hex << static_cast<uint16_t>(e) << std::dec << "'\n";
    out.put(e);
}

/**
 * @brief Create a data file object of binary data
 *
 */
void create_data_file()
{
    std::fstream out{"data", out.binary | out.out | out.trunc};

    const int size = (sizeof(magic) / sizeof(magic[0]));

    for (auto v : magic) {
        out.put(v);
        // std::cout << "Stored: '0x" << std::hex << static_cast<uint32_t>(v) << std::dec << "\n";
    } // for (auto v : magic)

    const uint16_t
        major = 1,
        minor = 3,
        patch = 13;

    store_uint16_t(major, out);
    store_uint16_t(minor, out);
    store_uint16_t(patch, out);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint16_t> distrib(10, 20);

    // generate the number of points that will be part of the header points vector
    uint16_t count = distrib(gen);

    // store the number of points that will be stored within the header point vector
    store_uint16_t(count, out);

    std::uniform_int_distribution<uint16_t> axis(0, 65535);
    for (auto cnt = 0 ; cnt < count ; cnt++) {
        // store the point definition in binary data
        store_uint16_t(axis(gen), out);
        store_uint16_t(axis(gen), out);
    } // for (auto cnt = 0 ; cnt < count ; cnt++)

    out.close();
}

} // namespace protocol
} // namespace examples

// @cond
int main(int argc, char** argv) {
    std::cout << "Calling protocol reader example\n";

    try {
        examples::protocol::create_data_file();

        // create an input stream
        std::fstream in{ "data", in.binary | in.in };

        // std::cout << "magic size = " << size << "\n";

        if (in.peek() == std::char_traits<char>::eof()) {
            std::cout << "Input file is empty.\n";
            return 1;
        } // if (in.peek() == std::char_traits<char>::eof())

        examples::protocol::header value;

        // define the dataLoader for a header instance...
        dataLoader<
            examples::protocol::header,
            char,
            examples::protocol::header_reader,
            6,
            is_character_noop<char>,
            is_no_character<char>,
            is_space_noop<char>
        > loader(value);
        // ...initialize the header value
        in >> loader;

        std::cout << "value = " << value << "\n";

        return 0;
    } catch (std::exception & ex) {
        std::cout << "An exception was caught: " << ex.what() << "\n";
        return 1;
    }
}
// @endcond
