# LightNBT

A free/libre, open-source and lightweight C++23 header-only library for reading, writing and accessing NBT (Named Binary Tag) data, supporting both big endian and little endian.

## Getting Started

```cpp
#include "lnbt.hpp"
#include <bit>
#include <fstream>
#include <iostream>
using namespace std;
int main()
{
    nbt::NBT data = nbt::read(ifstream("in.nbt", ios::binary));
    try {
        data.tag.get<string>("string") = "test";
    } catch (const exception &e) {
        cerr << e.what() << ": String tag `string` is not found" << endl;
    }
    int *a = data.tag.get_if<nbt::Compound>("compound")->get_if<int>("a");
    if (a != nullptr) cout << *a << endl;
    nbt::write<endian::little>(ofstream("out.nbt", ios::binary), data);
    return 0;
}
```

## Usage

Just include [`lnbt.hpp`](./lnbt.hpp) in the root directory to get started. Notice that C++23 is required.

### Data Structure

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
class Compound : public map<string, Tag>;
class List : public variant<vector<monostate>, vector<int8_t>, and other vectors>;
class Tag : public variant<monostate, int8_t, and other tag types>;
class NBT { string name; Tag tag; };
```

### Reading & Writing

`read` functions and `write` functions are respectively provided to read from `istream`s and write to `ostream`s. You can specify endian by specifying the template parameter `endian` as `std::endian::big` or `std::endian::little`.

```cpp
using std::endian, std::istream, nbt::NBT;
/// Read NBT from a binary input stream
template<endian endian = endian::big> inline NBT nbt::read(istream &in);
template<endian endian = endian::big> inline NBT nbt::read(istream &&in);
/// Write NBT to a binary output stream
template<endian endian = endian::big> inline void nbt::write(ostream &out, const NBT &val);
template<endian endian = endian::big> inline void nbt::write(ostream &&out, const NBT &val);
```

Please ensure that you open a file in binary mode, or the functions may not work as expected. Also notice that most of the NBT files are compressed, so a compression library is necessary.

### Access

In general, there are two ways to access a NBT data, `get` function series and `get_if` function series. All of `class Tag`, `class Compound` and `class List` have the two member function series to get the value they stored. Each `get` function has a corresponding `get_if` function, and each `get` function and `get_if` function has a corresponding const-qualified function. Functions of `get` function series will return the reference to the value, or throw `runtime_error` on error. While functions of `get_if` function series will return the pointer to the value, or return `nullptr` on error. Specially, `get_if` functions will also simply return `nullptr` when `this == nullptr`, so optional chaining can be implemented.

```cpp
// (Notice that all `get_if` functions and const-qualified functions are ignored because they are very similar to `get` functions)

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

You can use the `getType` member functions to get the type of a tag or the type of the tags in a List.

```cpp
inline nbt::TagType nbt::Tag::getType() const noexcept;
inline nbt::TagType nbt::List::getType() const noexcept;
```

### Example

The examples are in the [example](./example) directory.

- [example1](./example/example1.cpp): Convert little endian NBT to big endian NBT
- [example2](./example/example2.cpp): Print NBT as SNBT
- [example3](./example/example3.cpp): Get the position of the player from level.dat

## Todo

- Support `.mca` file
- Support SNBT (Stringified Named Binary Tag)
- Support the 8-byte header of bedrock `level.dat`

## License

Copyright (C) 2024 HappyArno

This program is licensed under [LGPL-3.0-or-later](https://www.gnu.org/licenses/lgpl-3.0.html). See [COPYING](./COPYING) and [COPYING.LESSER](./COPYING.LESSER) within the root directory for more information.