# LightNBT

A free/libre, open-source and lightweight C++23 header-only library, supporting reading and writing either big endian or little endian NBT (Named Binary Tag) data, reading and writing SNBT (stringified Named Binary Tag) data, reading region file, and accessing the data simply.

## Getting Started

```cpp
#include "lnbt.hpp"
#include <fstream>
#include <iostream>
int main()
{
    using namespace std;
    nbt::NBT data = nbt::str::read(ifstream("in.snbt"));
    try {
        data.tag.get<string>("string") = "test";
    } catch (const exception &e) {
        cerr << e.what() << ": String tag `string` is not found" << endl;
    }
    int *a = data.tag.get_if<nbt::Compound>("compound")->get_if<int>("a");
    if (a != nullptr) cout << *a << endl;
    nbt::bin::write<endian::little>(ofstream("out.nbt", ios::binary), data);
    return 0;
}
```

## Usage of NBT Part

Just include [`lnbt.hpp`](./lnbt.hpp) in the root directory to get started. Note that **C++23** is required.

### Data Structure

Each tag type maps to a C++ type.

| Tag Type          | C++ Type                   |
| ----------------- | -------------------------- |
| End               | `std::monostate`           |
| Byte              | `std::int8_t`              |
| Short             | `short`                    |
| Int               | `int`                      |
| Long              | `long long`                |
| Float             | `float`                    |
| Double            | `double`                   |
| Byte Array        | `std::vector<std::int8_t>` |
| String            | `std::string`              |
| List              | `nbt::List`                |
| Compound          | `nbt::Compound`            |
| Int Array         | `std::vector<int>`         |
| Long Array        | `std::vector<long long>`   |
| Tag (unnamed tag) | `nbt::Tag`                 |
| NBT (named tag)   | `nbt::NBT`                 |

Where `nbt::Compound`, `nbt::List`, `nbt::Tag` and `nbt::NBT` are classes:

```cpp
struct Compound : public map<string, Tag>;
struct List : public variant<vector<monostate>, vector<int8_t>, and other vectors>;
struct Tag : public variant<monostate, int8_t, and other tag types>;
struct NBT { string name; Tag tag; };
```

### Reading & Writing NBT

`nbt::bin::read` functions and `nbt::bin::write` functions are respectively provided to read NBT from `istream`s and write NBT to `ostream`s. You can specify endian by specifying the template parameter `endian` as `std::endian::big` or `std::endian::little`.

```cpp
using std::endian, std::istream, nbt::NBT;
/// Read NBT from a binary input stream
template<endian endian = endian::big> inline NBT nbt::bin::read(istream &in);
template<endian endian = endian::big> inline NBT nbt::bin::read(istream &&in);
/// Write NBT to a binary output stream
template<endian endian = endian::big> inline void nbt::bin::write(ostream &out, const NBT &val);
template<endian endian = endian::big> inline void nbt::bin::write(ostream &&out, const NBT &val);
```

Please ensure that you open a file in binary mode, or the functions may not work as expected. Also Note that most of the NBT files are compressed, so a compression library is necessary.

### Reading & Writing SNBT

`nbt::str::read` functions are provided to read SNBT from `istream`s. The functions return `nbt::Tag` instead of `nbt::NBT` because SNBT usually has no name. `nbt::str::Writer` class is provided to write SNBT to `ostream`s by using its non-static member function `nbt::str::Writer::write`, and configure the format by specifying its objects' non-static data members. `stdWriter`, `noLineFeedWriter` and `compactWriter` are provided as objects of class `Writer` for convenience, so that you can use them like `stdWriter.write(out, tag)` to write SNBT directly.

```cpp
/// Read SNBT from an input stream
inline nbt::Tag nbt::str::read(std::istream &in);
inline nbt::Tag nbt::str::read(std::istream &&in);
/// Write SNBT to an output stream
inline void nbt::str::Writer::write(std::ostream &out, const nbt::Tag &tag, size_t depth = 0ULL) const;
inline void nbt::str::Writer::write(std::ostream &&out, const nbt::Tag &tag, size_t depth = 0ULL) const;
/// Build-in Writers
const nbt::str::Writer stdWriter;
const nbt::str::Writer noLineFeedWriter{.line_feed = false};
const nbt::str::Writer compactWriter{.line_feed = false, .space = false};
```

### Access

Some functions are provided to access a NBT data. They are `getType` functions, `get` function series, `get_if` function series, `get_num_as` function and `match` function.

#### `getType`

You can use the member function `getType` to get the type of a tag or the type of the tags in a List.

```cpp
inline nbt::TagType nbt::Tag::getType() const noexcept;
inline nbt::TagType nbt::List::getType() const noexcept;
```

#### `get` & `get_if`

All of `class Tag`, `class Compound` and `class List` have the two member function series to get the value they stored. Each `get` function has a corresponding `get_if` function, and each `get` function and `get_if` function has a corresponding const-qualified function. Functions of `get` function series will return the reference to the value, or throw `runtime_error` on error. While functions of `get_if` function series will return the pointer to the value, or return `nullptr` on error. Specially, `get_if` functions will also simply return `nullptr` when `this == nullptr`, so optional chaining can be implemented.

```cpp
// (Note that all `get_if` functions and const-qualified functions are ignored because they are very similar to `get` functions)

using nbt::Compound, nbt::List, nbt::Tag, std::string, std::vector;

/// Get the tag as type `T`
template<class T> T &Tag::get();
/// Get the tag as Compound and get the tag in the Compound by name
Tag &Tag::get(string name);
/// Get the tag as Compound and get the tag in the Compound by name as type `T`
template<class T> T &Tag::get(string name);
/// Get the tag as vector and get the tag in the vector by index
template<class T>
    requires same_as<T, vector<int8_t>> || same_as<T, vector<int>> || same_as<T, vector<long long>>
T::value_type &Tag::get(size_t i);

/// Get the tag by name
Tag &Compound::get(string name);
/// Get the tag by name as type `T`
template<class T> T &Compound::get(string name);

/// Get the tag as `vector<T>`
template<class T> vector<T> &List::get();
/// Get the tag as `vector<T>` and get the tag in the vector by index
template<class T> T &List::get(size_t i);
```

#### `get_num_as`

You can use the member function `get_num_as` to get the number in a tag as the provided numeric type.

```cpp
template<class T> inline T nbt::Tag::get_num_as() const;
```

#### `match`

`match` function is provided to match a tag type to the corresponding C++ type through explicitly specifying template arguments of a lambda expressions with an explicit template parameter list.

```cpp
template<class Func> auto nbt::match(nbt::TagType type, Func &&func);

// example
Tag tag(3.14);
match(tag.getType(), [&tag]<typename T> {
    if constexpr (integral<T> || floating_point<T>)
        cout << tag.get<T>() << endl;
});
List list = vector<int>{1, 2, 3};
bool is_nonempty_list = match(list.getType(), [&list]<typename T> {
    return !list.get<T>().empty();
});
```

## Usage of Region Part

Just include [`lmca.hpp`](./lmca.hpp) in the root directory to get started. Note that **C++23** and **zlib** is required.

### Data Structure

```cpp
/// A chunk
struct mca::Chunk
{
    uint32_t timestamp;
    NBT data;
};
/// A region, which stores a group of 32×32 chunks
struct mca::Region : std::array<std::optional<mca::Chunk>, 1024>;
```

### Read Chunks

```cpp
/// Read the data of a chunk from a region file
nbt::NBT mca::readChunk(std::istream &region, mca::SectorInfo location);
nbt::NBT mca::readChunk(std::istream &&region, mca::SectorInfo location);
/// Read a chunk from a region file
mca::Chunk mca::readChunk(std::istream &region, size_t x, size_t z);
mca::Chunk mca::readChunk(std::istream &&region, size_t x, size_t z);
```

### Read Regions

```cpp
/// Read a region from a region file
mca::Region mca::readRegion(std::istream &region);
mca::Region mca::readRegion(std::istream &&region);
```

### Access

```cpp
/// Get a chunk by its local coordinates within a region
inline std::optional<mca::Chunk> &mca::Region::get(size_t x, size_t z);
inline const std::optional<mca::Chunk> &mca::Region::get(size_t x, size_t z) const;
```

## Example

The examples are in the [example](./example) directory.

- [example1](./example/example1.cpp): Convert little endian NBT to big endian NBT
- [example2](./example/example2.cpp): Convert SNBT to NBT and convert NBT to SNBT
- [example3](./example/example3.cpp): Print NBT as SNBT
- [example4](./example/example4.cpp): Get the position of the player from level.dat

## Todo

- Support the 8-byte header of bedrock `level.dat`
- Support automatic identifying the type of a file

## License

Copyright (C) 2024 HappyArno

This program is licensed under [LGPL-3.0-or-later](https://www.gnu.org/licenses/lgpl-3.0.html). See [COPYING](./COPYING) and [COPYING.LESSER](./COPYING.LESSER) within the root directory for more information.