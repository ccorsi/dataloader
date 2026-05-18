# Data Loader Template Class Examples

The following set of examples will showcase the different ways that one can
use the dataloader template class to load data within an instance using a
basic\_istream.  While these examples cover several ways one can go
about using the data loader template class to populate an instance.  It
is very possible that one can come up with other configurations to load
data into an instance using the data loader template class.

While we can continue to explain how one can use the data loader template
class.  We will just describe the different examples on how one can load
data into an instance using the data loader template class.

## Simple Primitive Data Value Initialization

The [primitive reader](simple/README) example will show one how you can
populate a primitive data type.  Thou one won't use this technique since the
standard std::in can be used to read primitive data types.  This is a good
starting point to get aquainted with the data loader template class.

## Complex Data Value Initialization

The [single reader call](single/README) example is an example on how you can
load all the fields within a single reader call using the data loader template
class.

The [multiple reader call](multiple/README) example is an example on how you
can define the reader such that it is called __n__ times for each field of the
type that is being loaded.

The [string reader call](string/README) example will show how you can define
different readers used to load a string using the data loader template class.
This example will display two different ways that one can go about loading a
data string.  The first version loads the complete string while the second
version will load a single character at a time until the closing character was
encountered.  Both solutions are viable implementations so this example shows
how one can choose to use the dataloader template class.

## Container Type Data Initialization

The [integer vector reader](intvector/README) example shows how you can load a
primitive type into a integer vector using a dataloader template class.

The [point vector reader](pointvector/README) example shows how you can load
zero or more point instances into a standard vector instance.

## Binary to Type Data Initialization

The [protocol reader](protocol/README) example uses a binary protocol to
populate a type instance using a dataloader template class.  This example
shows how you can use the dataloader template class to initialize an instance
using binary input data.

## Repeated Type Data Initialization

The [repeat reader](repeat/README) example is an example on how you can load
instances within a loop condition using a dataloader template class.  This
use case can be used when you are creating test case for your class and want to
be able to add tests conditions without having to update the source tests.
