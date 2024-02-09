/*
   LightNBT: A lightweight C++23 header-only NBT library
   Copyright (C) 2024 HappyArno

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef _LNBT_HPP
#define _LNBT_HPP

#include <bit>
#include <concepts>
#include <cstdint>
#include <istream>
#include <map>
#include <ostream>
#include <string>
#include <variant>
#include <vector>
namespace nbt
{
using namespace std;
/// The type ID of a tag
enum class TagType
{
    End = 0,
    Byte = 1,
    Short = 2,
    Int = 3,
    Long = 4,
    Float = 5,
    Double = 6,
    ByteArray = 7,
    String = 8,
    List = 9,
    Compound = 10,
    IntArray = 11,
    LongArray = 12
};
class Tag;
class NBT;
/// A list of name-tag pairs
class Compound : public map<string, Tag>
{
public:
    using map::map;
    Tag &get(string name)
    {
        return at(name);
    }
    const Tag &get(string name) const
    {
        return at(name);
    }
    template <typename T>
    T &get(string name);
    template <typename T>
    const T &get(string name) const;
    Tag *get_if(string name);
    const Tag *get_if(string name) const;
    template <typename T>
    T *get_if(string name);
    template <typename T>
    const T *get_if(string name) const;
};
/// An ordered list of unnamed tags, all of the same type
class List : public variant<vector<monostate>, vector<int8_t>, vector<short>, vector<int>, vector<long long>, vector<float>, vector<double>, vector<vector<int8_t>>, vector<string>, vector<List>, vector<Compound>, vector<vector<int>>, vector<vector<long long>>>
{
public:
    using variant::variant;
    /// Get the type of the tags in the List
    inline TagType getType() const noexcept
    {
        return static_cast<TagType>(index());
    }
    template <typename T>
    vector<T> &get()
    {
        return ::std::get<vector<T>>(*this);
    }
    template <typename T>
    const vector<T> &get() const
    {
        return ::std::get<vector<T>>(*this);
    }
    template <typename T>
    T &get(size_t i)
    {
        return ::std::get<vector<T>>(*this).at(i);
    }
    template <typename T>
    const T &get(size_t i) const
    {
        return ::std::get<vector<T>>(*this).at(i);
    }
    template <typename T>
    vector<T> *get_if()
    {
        return ::std::get_if<vector<T>>(this);
    }
    template <typename T>
    const vector<T> *get_if() const
    {
        return ::std::get_if<vector<T>>(this);
    }
    template <typename T>
    T *get_if(size_t i)
    {
        vector<T> *vec = get_if<T>();
        if (vec == nullptr || i >= vec->size())
            return nullptr;
        return &((*vec)[i]);
    }
    template <typename T>
    const T *get_if(size_t i) const
    {
        vector<T> *vec = get_if<T>();
        if (vec == nullptr || i >= vec->size())
            return nullptr;
        return &((*vec)[i]);
    }
};
/// A unnamed tag
class Tag : public variant<monostate, int8_t, short, int, long long, float, double, vector<int8_t>, string, List, Compound, vector<int>, vector<long long>>
{
public:
    using variant::variant;
    /// Get the type of the tag
    inline TagType getType() const noexcept
    {
        return static_cast<TagType>(index());
    }
    template <typename T>
    T &get()
    {
        return ::std::get<T>(*this);
    }
    template <typename T>
    const T &get() const
    {
        return ::std::get<T>(*this);
    }
    Tag &get(string name)
    {
        return get<Compound>().at(name);
    }
    const Tag &get(string name) const
    {
        return get<Compound>().at(name);
    }
    template <typename T>
    T &get(string name)
    {
        return get(name).get<T>();
    }
    template <typename T>
    const T &get(string name) const
    {
        return get(name).get<T>();
    }
    template <typename T>
        requires same_as<T, vector<int8_t>> || same_as<T, vector<int>> || same_as<T, vector<long long>>
    T::value_type &get(size_t i)
    {
        return get<T>().at(i);
    }
    template <typename T>
        requires same_as<T, vector<int8_t>> || same_as<T, vector<int>> || same_as<T, vector<long long>>
    const T::value_type &get(size_t i) const
    {
        return get<T>().at(i);
    }
    template <typename T>
        requires same_as<T, int8_t> || same_as<T, short> || same_as<T, int> || same_as<T, long long> || same_as<T, float> || same_as<T, double>
    T get_num_as() const
    {
        switch (getType())
        { // clang-format off
        case TagType::Byte: return static_cast<T>(get<int8_t>());
        case TagType::Short: return static_cast<T>(get<short>());
        case TagType::Int: return static_cast<T>(get<int>());
        case TagType::Long: return static_cast<T>(get<long long>());
        case TagType::Float: return static_cast<T>(get<float>());
        case TagType::Double: return static_cast<T>(get<double>());
        default: throw runtime_error("not a number");
        } // clang-format on
    }
    template <typename T>
    T *get_if()
    {
        return ::std::get_if<T>(this);
    }
    template <typename T>
    const T *get_if() const
    {
        return ::std::get_if<T>(this);
    }
    Tag *get_if(string name)
    {
        return get_if<Compound>()->get_if(name);
    }
    const Tag *get_if(string name) const
    {
        return get_if<Compound>()->get_if(name);
    }
    template <typename T>
    T *get_if(string name)
    {
        return get_if(name)->get_if<T>();
    }
    template <typename T>
    const T *get_if(string name) const
    {
        return get_if(name)->get_if<T>();
    }
    template <typename T>
        requires same_as<T, vector<int8_t>> || same_as<T, vector<int>> || same_as<T, vector<long long>>
    T::value_type *get_if(size_t i)
    {
        T *vec = get_if<T>();
        if (vec == nullptr || i >= vec->size())
            return nullptr;
        return &((*vec)[i]);
    }
    template <typename T>
        requires same_as<T, vector<int8_t>> || same_as<T, vector<int>> || same_as<T, vector<long long>>
    const T::value_type *get_if(size_t i) const
    {
        T *vec = get_if<T>();
        if (vec == nullptr || i >= vec->size())
            return nullptr;
        return &((*vec)[i]);
    }
};
template <typename T>
T &Compound::get(string name)
{
    return get(name).get<T>();
}
template <typename T>
const T &Compound::get(string name) const
{
    return get(name).get<T>();
}
inline Tag *Compound::get_if(string name)
{
    if (this == nullptr)
        return nullptr;
    auto iter = find(name);
    if (iter == end())
        return nullptr;
    return &(iter->second);
}
inline const Tag *Compound::get_if(string name) const
{
    if (this == nullptr)
        return nullptr;
    auto iter = find(name);
    if (iter == end())
        return nullptr;
    return &(iter->second);
}
template <typename T>
T *Compound::get_if(string name)
{
    return get_if(name)->get_if<T>();
}
template <typename T>
const T *Compound::get_if(string name) const
{
    return get_if(name)->get_if<T>();
}
/// A named tag (NBT)
class NBT
{
public:
    string name;
    Tag tag;
    NBT() = default;
    NBT(string name, Tag tag) : name(name), tag(tag) {}
    NBT(Tag tag) : name(""), tag(tag) {}
    NBT(pair<string, Tag> p) : name(p.first), tag(p.second) {}
};

/// Specify that a type is a valid tag type
template <typename T>
concept is_tag = same_as<T, monostate> || same_as<T, int8_t> || same_as<T, short> || same_as<T, int> || same_as<T, long long> || same_as<T, float> || same_as<T, double> || same_as<T, vector<int8_t>> || same_as<T, string> || same_as<T, List> || same_as<T, Compound> || same_as<T, vector<int>> || same_as<T, vector<long long>>;
/// Check if a type is a instance of vector
template <typename T>
constexpr bool is_vector = false;
template <typename T>
constexpr bool is_vector<vector<T>> = true;

/// A helper function for changing the endian of a value to endian::native
template <endian endian, typename T>
inline T endianswap(T val)
{
    if constexpr (endian::native == endian)
        return val;
    else
    {
        constexpr size_t size = sizeof(T);
        if constexpr (size == 1)
            return val;
        else if constexpr (size == 2)
            return bit_cast<T>(byteswap(bit_cast<int16_t>(val)));
        else if constexpr (size == 4)
            return bit_cast<T>(byteswap(bit_cast<int32_t>(val)));
        else if constexpr (size == 8)
            return bit_cast<T>(byteswap(bit_cast<int64_t>(val)));
        else
            static_assert(false, "not a supported type");
    }
}

/// A helper class for making binary io support both big endian and little endian
template <endian endian>
class io
{
protected:
    template <typename T>
        requires same_as<T, monostate>
    static T read(istream &in);
    template <typename T>
        requires same_as<T, TagType>
    static T read(istream &in);
    template <typename T>
        requires integral<T> || floating_point<T>
    static T read(istream &in);
    template <typename T>
        requires same_as<T, string>
    static T read(istream &in);
    template <typename T>
        requires is_vector<T> && is_tag<typename T::value_type>
    static T read(istream &in);
    template <typename T>
        requires same_as<T, List>
    static T read(istream &in);
    template <typename T>
        requires same_as<T, Compound>
    static T read(istream &in);
    template <typename T>
        requires same_as<T, Tag>
    static T read(istream &in, TagType type);
public:
    static NBT read(istream &in);
protected:
    template <typename T>
        requires same_as<T, monostate>
    static void write(ostream &out, T val);
    template <typename T>
        requires same_as<T, TagType>
    static void write(ostream &out, T val);
    template <typename T>
        requires integral<T> || floating_point<T>
    static void write(ostream &out, T val);
    template <typename T>
        requires same_as<T, string>
    static void write(ostream &out, const T &val);
    template <typename T>
        requires is_vector<T> && is_tag<typename T::value_type>
    static void write(ostream &out, const T &val);
    template <typename T>
        requires same_as<T, List>
    static void write(ostream &out, const T &val);
    template <typename T>
        requires same_as<T, Compound>
    static void write(ostream &out, const T &val);
    template <typename T>
        requires same_as<T, Tag>
    static void write(ostream &out, const T &val);
public:
    static void write(ostream &out, const NBT &val);
};
/// Read NBT from a binary input stream
template <endian endian = endian::big>
inline NBT read(istream &in)
{
    return io<endian>::read(in);
}
/// Read NBT from a binary input stream
template <endian endian = endian::big>
inline NBT read(istream &&in)
{
    return io<endian>::read(in);
}
/// Write NBT to a binary output stream
template <endian endian = endian::big>
inline void write(ostream &out, const NBT &val)
{
    io<endian>::write(out, val);
}
/// Write NBT to a binary output stream
template <endian endian = endian::big>
inline void write(ostream &&out, const NBT &val)
{
    io<endian>::write(out, val);
}

template <endian endian>
template <typename T>
    requires same_as<T, monostate>
T io<endian>::read(istream &in)
{
    return monostate();
}
template <endian endian>
template <typename T>
    requires same_as<T, TagType>
T io<endian>::read(istream &in)
{
    return static_cast<TagType>(in.get());
}
template <endian endian>
template <typename T>
    requires integral<T> || floating_point<T>
T io<endian>::read(istream &in)
{
    T val;
    in.read(reinterpret_cast<char *>(&val), sizeof(T));
    return endianswap<endian>(val);
}
template <endian endian>
template <typename T>
    requires same_as<T, string>
T io<endian>::read(istream &in)
{
    size_t size = read<short>(in);
    string str(size, '\0');
    in.read(str.data(), size);
    return str;
}
template <endian endian>
template <typename T>
    requires is_vector<T> && is_tag<typename T::value_type>
T io<endian>::read(istream &in)
{
    T vec(read<int>(in));
    for (auto &e : vec)
        e = read<typename T::value_type>(in);
    return vec;
}
template <endian endian>
template <typename T>
    requires same_as<T, List>
T io<endian>::read(istream &in)
{
    TagType type = read<TagType>(in);
    switch (type)
    { // clang-format off
    case TagType::End: return read<vector<monostate>>(in);
    case TagType::Byte: return read<vector<int8_t>>(in);
    case TagType::Short: return read<vector<short>>(in);
    case TagType::Int: return read<vector<int>>(in);
    case TagType::Long: return read<vector<long long>>(in);
    case TagType::Float: return read<vector<float>>(in);
    case TagType::Double: return read<vector<double>>(in);
    case TagType::ByteArray: return read<vector<vector<int8_t>>>(in);
    case TagType::String: return read<vector<string>>(in);
    case TagType::List: return read<vector<List>>(in);
    case TagType::Compound: return read<vector<Compound>>(in);
    case TagType::IntArray: return read<vector<vector<int>>>(in);
    case TagType::LongArray: return read<vector<vector<long long>>>(in);
    default: throw runtime_error("unsupported tag ID");
    } // clang-format on
}
template <endian endian>
template <typename T>
    requires same_as<T, Compound>
T io<endian>::read(istream &in)
{
    Compound compound;
    for (TagType type = read<TagType>(in); type != TagType::End; type = read<TagType>(in))
    {
        string name = read<string>(in);
        Tag tag = read<Tag>(in, type);
        compound.insert(make_pair(name, tag));
        // compound[name] = tag;
    }
    return compound;
}
template <endian endian>
template <typename T>
    requires same_as<T, Tag>
T io<endian>::read(istream &in, TagType type)
{
    switch (type)
    { // clang-format off
    case TagType::End: return read<monostate>(in);
    case TagType::Byte: return read<int8_t>(in);
    case TagType::Short: return read<short>(in);
    case TagType::Int: return read<int>(in);
    case TagType::Long: return read<long long>(in);
    case TagType::Float: return read<float>(in);
    case TagType::Double: return read<double>(in);
    case TagType::ByteArray: return read<vector<int8_t>>(in);
    case TagType::String: return read<string>(in);
    case TagType::List: return read<List>(in);
    case TagType::Compound: return read<Compound>(in);
    case TagType::IntArray: return read<vector<int>>(in);
    case TagType::LongArray: return read<vector<long long>>(in);
    default: throw runtime_error("unsupported tag ID");
    } // clang-format on
}
template <endian endian>
NBT io<endian>::read(istream &in)
{
    TagType type = read<TagType>(in);
    string name = read<string>(in);
    Tag tag = read<Tag>(in, type);
    return NBT(name, tag);
}

template <endian endian>
template <typename T>
    requires same_as<T, monostate>
void io<endian>::write(ostream &out, T val)
{
    return;
}
template <endian endian>
template <typename T>
    requires same_as<T, TagType>
void io<endian>::write(ostream &out, T val)
{
    out.put(static_cast<int8_t>(val));
}
template <endian endian>
template <typename T>
    requires integral<T> || floating_point<T>
void io<endian>::write(ostream &out, T val)
{
    val = endianswap<endian>(val);
    out.write(reinterpret_cast<const char *>(&val), sizeof(T));
}
template <endian endian>
template <typename T>
    requires same_as<T, string>
void io<endian>::write(ostream &out, const T &val)
{
    write<short>(out, val.size());
    out.write(val.c_str(), val.size());
}
template <endian endian>
template <typename T>
    requires is_vector<T> && is_tag<typename T::value_type>
void io<endian>::write(ostream &out, const T &val)
{
    write<int>(out, val.size());
    for (auto &e : val)
        write<typename T::value_type>(out, e);
}
template <endian endian>
template <typename T>
    requires same_as<T, List>
void io<endian>::write(ostream &out, const T &val)
{
    write<TagType>(out, val.getType());
    switch (val.getType())
    { // clang-format off
    case TagType::End: write(out, val.template get<monostate>()); return;
    case TagType::Byte: write(out, val.template get<int8_t>()); return;
    case TagType::Short: write(out, val.template get<short>()); return;
    case TagType::Int: write(out, val.template get<int>()); return;
    case TagType::Long: write(out, val.template get<long long>()); return;
    case TagType::Float: write(out, val.template get<float>()); return;
    case TagType::Double: write(out, val.template get<double>()); return;
    case TagType::ByteArray: write(out, val.template get<vector<int8_t>>()); return;
    case TagType::String: write(out, val.template get<string>()); return;
    case TagType::List: write(out, val.template get<List>()); return;
    case TagType::Compound: write(out, val.template get<Compound>()); return;
    case TagType::IntArray: write(out, val.template get<vector<int>>()); return;
    case TagType::LongArray: write(out, val.template get<vector<long long>>()); return;
    default: throw runtime_error("unsupported tag ID");
    } // clang-format on
}
template <endian endian>
template <typename T>
    requires same_as<T, Compound>
void io<endian>::write(ostream &out, const T &val)
{
    for (const auto &[name, tag] : val)
    {
        write<TagType>(out, tag.getType());
        write(out, name);
        write(out, tag);
    }
    write<TagType>(out, TagType::End);
}
template <endian endian>
template <typename T>
    requires same_as<T, Tag>
void io<endian>::write(ostream &out, const T &val)
{
    TagType type = val.getType();
    switch (val.getType())
    { // clang-format off
    case TagType::End: write(out, val.template get<monostate>()); return;
    case TagType::Byte: write(out, val.template get<int8_t>()); return;
    case TagType::Short: write(out, val.template get<short>()); return;
    case TagType::Int: write(out, val.template get<int>()); return;
    case TagType::Long: write(out, val.template get<long long>()); return;
    case TagType::Float: write(out, val.template get<float>()); return;
    case TagType::Double: write(out, val.template get<double>()); return;
    case TagType::ByteArray: write(out, val.template get<vector<int8_t>>()); return;
    case TagType::String: write(out, val.template get<string>()); return;
    case TagType::List: write(out, val.template get<List>()); return;
    case TagType::Compound: write(out, val.template get<Compound>()); return;
    case TagType::IntArray: write(out, val.template get<vector<int>>()); return;
    case TagType::LongArray: write(out, val.template get<vector<long long>>()); return;
    default: throw runtime_error("unsupported tag ID");
    } // clang-format on
}
template <endian endian>
void io<endian>::write(ostream &out, const NBT &val)
{
    write<TagType>(out, val.tag.getType());
    write(out, val.name);
    write(out, val.tag);
}
} // namespace nbt

#endif // _LNBT_HPP