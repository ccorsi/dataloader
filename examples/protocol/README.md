# Binary Data Loader Example

This example show you how to use the data loader template class to populate instances
using binary data as input.  While this type of data loader example is not something
that the data loader implementation was initially implemented for.  It is nonetheless
something that can also be done with the data loader template class implementation.
Let us them delve into this current example that provides a fairly simple use case
of using the data loader template class to load binary data within a C++ instance.

## Definition of the Runtime Data Type

We will then begin with the definition of the data type that will be populated using
the binary data input.  This example will populate the header class that is defined
as follows:

```cpp
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
};
```

The above class just defines several simple unsigned 16 bit integers and a vector array of points.
The vector will be populated with one or more point instances defined as follows.

```cpp
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
};
```

As you can see the point class defines a point on a plane using unsigned 16 bit integers.

Now that we defined the different classes that we want to populate.  We now need to define
the reader instance that will be used to populate the header instance containing one or more
point instance.  Let us then look at how this reader instance will be defined.

## The Header Reader Instance Definition

The definition of the header reader instance will require that it is defined as a struct or class
type.  We will then define a struct for this example as follow

```cpp
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
```

There are some things to note with the definition of the header reader instance.  You will notice that the
struct defines the <code>std::istream& operator()(std::istream& in, header & hdr, int field)</code>
operator needed by the dataloader template class.  This particular implementation will only be used using
an istream as the input stream.  This should suffice in this case since we are processing binary data and
we are not expecting and char or wchar_t data processing.  Looking into the implementation of the operator
we see that there are several points to take into account.

The initial thing to notice is that the implementation uses a switch to determine which entry is being
processed next.   This is the recommended pattern that you should follow to insure that the data is being
properly processed.  It removes the need to track which attribute is being populated within the header
instance being populated.  In this case, it is dividing this process into 6 different steps.  Where each
step will populate the different attributes within the header instance.  The header instance will then
be populated when we are processing the sixth attribute using the passed header instance.  This header
instance will be the same instance that was passed through the different attributes being populated.  The
data loader template class manages this for you, allowing you to focus on transforming your data.

While the above will be used to populate the header instance.  We still need to be able to populate the
point vector.  This will require us to be able to iterate through the populating of the point instances
that will be added to the point vector.  In the above definition we are going to use a dataloader instance
using a point_reader reader instance to populate each point instance that will be added to the point
vector.  Thus, the point_reader struct is defined as the following.

```cpp
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
```


### Helpers

While the definition of the header reader class might seem like enough.  We do need to be able to
transfer the binary data into appropriate runtime data.  In this example, we want to be able to
convert the unsigned 16 bit integer binary representation into a valid <code>uint16_t</code> type.
This is then done using the following tool.

```cpp
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
```

The above function is just used to convert the binary data into a valid unsigned 16 bit integer
while ensuring that there is enough input data to produce the expected unsigned 16 integer.  This
function is used throughout the different reader implementations and is just reusing the same
code throughout the reader definitions.

We've reached the point where we are ready to put all of the above together to populate the header
instances using the header reader defined above and all of their dependent workers.

## Putting it all together

While the above <code>header_reader</code> struct offers some idea of how to put this all together.
We are nonetheless show how this can then be used.

```cpp
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
```

The above dataloader instance states that a header instance will be populated using a
<code>header_reader</code> instance.  The header instance passed to the loader instance
will then be populated after the input call using the define input stream.  The reader
expects the header instance to be loaded using 6 different passes. Where each pass will
be mandated to load different attributes of the header instance.  After the input stream
call has completed the passed header instance will be populated.

We now just need to explain what the remaining data loader template class type names mean.
The first states that we are populating a header type.  The second just states that we are
using an <code>basic_istream</code> of type <code>char</code>.  The third type name informs
the data loader template class which reader to use to populate the header instance.  Let us
now look at what the remaining three type name definitions imply in this case.

The first two of these just inform that the dataloader template class should not expect to
encounter any opening or closing characters using the <code>is_character_noop<char></code>
and <code>is_no_character<char></code> operators.  While the remaining type name definitions
states that there are no white space type data to skip within the binary data that is being
processed.

While the above is a fairly simple implementation but one can use the data loader template
class for any structure however simple or complexly defined.

While the above provides a fairly simple example on how to use the data loader template
class to populate instances using binary data.  It isn't something that the data loader
template class was initially defined to be able to do.  It is just something that came
naturally from its current implementation.
