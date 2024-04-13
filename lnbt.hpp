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
#include <charconv>
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
struct Tag;
struct NBT;
/// A list of name-tag pairs
struct Compound : public map<string, Tag>
{
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
struct List : public variant<vector<monostate>, vector<int8_t>, vector<short>, vector<int>, vector<long long>, vector<float>, vector<double>, vector<vector<int8_t>>, vector<string>, vector<List>, vector<Compound>, vector<vector<int>>, vector<vector<long long>>>
{
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

/// Specify that a type is a valid tag type
template <typename T>
concept is_tag = same_as<T, monostate> || same_as<T, int8_t> || same_as<T, short> || same_as<T, int> || same_as<T, long long> || same_as<T, float> || same_as<T, double> || same_as<T, vector<int8_t>> || same_as<T, string> || same_as<T, List> || same_as<T, Compound> || same_as<T, vector<int>> || same_as<T, vector<long long>>;
/// Check if a type is a instance of vector
template <typename T>
constexpr bool is_vector = false;
template <typename T>
constexpr bool is_vector<vector<T>> = true;
/// Specify that a type is a valid array type
template <typename T>
concept is_array = same_as<T, vector<int8_t>> || same_as<T, vector<int>> || same_as<T, vector<long long>>;
/// Specify that a type is a valid list type
/// Note: it is different from `same_as<T, nbt::List>`
template <typename T>
concept is_list = is_vector<T> && is_tag<typename T::value_type>;

/// A unnamed tag
struct Tag : public variant<monostate, int8_t, short, int, long long, float, double, vector<int8_t>, string, List, Compound, vector<int>, vector<long long>>
{
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
    template <is_array T>
    T::value_type &get(size_t i)
    {
        return get<T>().at(i);
    }
    template <is_array T>
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
    template <is_array T>
    T::value_type *get_if(size_t i)
    {
        T *vec = get_if<T>();
        if (vec == nullptr || i >= vec->size())
            return nullptr;
        return &((*vec)[i]);
    }
    template <is_array T>
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
// Comparing 'this' pointer with nullptr is necessary here
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-undefined-compare"
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
#pragma clang diagnostic pop
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
struct NBT
{
    string name;
    Tag tag;
    NBT() = default;
    NBT(string name, Tag tag) : name(name), tag(tag) {}
    NBT(Tag tag) : name(""), tag(tag) {}
    NBT(pair<string, Tag> p) : name(p.first), tag(p.second) {}
};
/// Match a tag type to the corresponding C++ type through explicitly specifying template arguments of a lambda expressions with an explicit template parameter list
template <typename Func>
auto match(TagType type, Func &&func)
{
    switch (type)
    { // clang-format off
    case TagType::End: return func.template operator()<monostate>();
    case TagType::Byte: return func.template operator()<int8_t>();
    case TagType::Short: return func.template operator()<short>();
    case TagType::Int: return func.template operator()<int>();
    case TagType::Long: return func.template operator()<long long>();
    case TagType::Float: return func.template operator()<float>();
    case TagType::Double: return func.template operator()<double>();
    case TagType::ByteArray: return func.template operator()<vector<int8_t>>();
    case TagType::String: return func.template operator()<string>();
    case TagType::List: return func.template operator()<List>();
    case TagType::Compound: return func.template operator()<Compound>();
    case TagType::IntArray: return func.template operator()<vector<int>>();
    case TagType::LongArray: return func.template operator()<vector<long long>>();
    default: throw runtime_error("unsupported tag ID");
    } // clang-format on
}
} // namespace nbt

namespace nbt::bin
{
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
/// Instead of using the functions in this class directly, use nbt::read and nbt::write
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
    template <is_list T>
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
    template <is_list T>
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
template <is_list T>
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
    return match(read<TagType>(in), [&in]<typename U> { return T(read<vector<U>>(in)); });
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
    return match(type, [&in]<typename U> { return T(read<U>(in)); });
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
template <is_list T>
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
    match(val.getType(), [&out, &val]<typename U> { write(out, val.template get<U>()); });
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
    match(val.getType(), [&out, &val]<typename U> { write(out, val.template get<U>()); });
}
template <endian endian>
void io<endian>::write(ostream &out, const NBT &val)
{
    write<TagType>(out, val.tag.getType());
    write(out, val.name);
    write(out, val.tag);
}

/// Read NBT from a binary input stream
template <endian endian = endian::big>
inline NBT read(istream &in)
{
    in.exceptions(istream::eofbit | istream::failbit | istream::badbit);
    return io<endian>::read(in);
}
/// Read NBT from a binary input stream
template <endian endian = endian::big>
inline NBT read(istream &&in)
{
    return read<endian>(in);
}
/// Write NBT to a binary output stream
template <endian endian = endian::big>
inline void write(ostream &out, const NBT &val)
{
    out.exceptions(ostream::eofbit | ostream::failbit | ostream::badbit);
    io<endian>::write(out, val);
}
/// Write NBT to a binary output stream
template <endian endian = endian::big>
inline void write(ostream &&out, const NBT &val)
{
    write<endian>(out, val);
}
} // namespace nbt::bin

namespace nbt::str
{
/// The functions in this namespace have some checks removed to improve performance, so never use these functions and use the functions in the nbt::str namespace instead.
namespace detail
{
inline void skipWhitespace(istream &in)
{
    while (isspace(in.peek()))
        in.ignore();
}
inline void expect(istream &in, char c)
{
    if (in.get() != c)
        throw runtime_error("unexpected character");
}
inline void check(istream &in, char c)
{
    if (in.peek() != c)
        throw runtime_error("unexpected character");
}
inline bool isAllowedInUnquotedString(char c)
{
    return c >= '0' && c <= '9' || c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z' || c == '_' || c == '-' || c == '.' || c == '+';
}
template <typename T>
    requires integral<T> || floating_point<T>
T readNum(const string_view s)
{
    T val;
    auto [ptr, ec]{from_chars(s.data(), s.data() + s.size(), val)};
    if (ec != errc() || ptr != s.data() + s.size())
        throw runtime_error("from_chars() error");
    return val;
}
template <typename T, char suffix>
    requires integral<T> || floating_point<T>
inline T readNumWithSuffix(const string_view s)
{
    if (tolower(s.back()) != suffix)
        throw runtime_error("unexpected type");
    return readNum<T>(string_view(s.data(), s.size() - 1));
}
template <typename T>
    requires integral<T> || floating_point<T>
T read(const string_view s)
{
    if constexpr (same_as<T, int8_t>)
    {
        if (s == "true")
            return 1;
        else if (s == "false")
            return 0;
        return readNumWithSuffix<T, 'b'>(s);
    }
    else if constexpr (same_as<T, short>)
        return readNumWithSuffix<T, 's'>(s);
    else if constexpr (same_as<T, int>)
        return readNum<T>(s);
    else if constexpr (same_as<T, long long>)
        return readNumWithSuffix<T, 'l'>(s);
    else if constexpr (same_as<T, float>)
        return readNumWithSuffix<T, 'f'>(s);
    else if constexpr (same_as<T, double>)
    {
        if (tolower(s.back()) == 'd')
            return readNum<T>(string_view(s.data(), s.size() - 1));
        else
            return readNum<T>(s);
    }
    else
        static_assert(false, "unsupported type");
}
inline string readQuotedString(istream &in, char mark)
{
    in.ignore(); // expect mark to be ignored
    string s;
    bool escaped = false;
    for (;;)
    {
        char c = in.get();
        if (escaped)
        {
            switch (c)
            { // clang-format off
            case '\'': s.push_back('\''); break;
            case '\"': s.push_back('\"'); break;
            case '?': s.push_back('?'); break;
            case '\\': s.push_back('\\'); break;
            case 'a': s.push_back('\a'); break;
            case 'b': s.push_back('\b'); break;
            case 'f': s.push_back('\f'); break;
            case 'n': s.push_back('\n'); break;
            case 'r': s.push_back('\r'); break;
            case 't': s.push_back('\t'); break;
            case 'v': s.push_back('\v'); break;
            default: throw runtime_error("the character cannot be escaped");
            } // clang-format on
            escaped = false;
        }
        else if (c == '\\')
            escaped = true;
        else if (c == mark)
            return s;
        else
            s.push_back(c);
    }
}
inline string readUnquotedString(istream &in)
{
    string s;
    for (char c = in.peek(); isAllowedInUnquotedString(c); c = in.peek())
        s.push_back(c), in.ignore();
    return s;
}
template <typename T>
    requires same_as<T, string>
T read(istream &in)
{
    if (char c = in.peek(); c == '\"' || c == '\'')
        return readQuotedString(in, c);
    else
        return readUnquotedString(in);
}

template <typename T>
    requires same_as<T, Tag>
T read(istream &in);

template <typename T>
    requires same_as<T, Compound>
T read(istream &in)
{
    in.ignore(); // expect '{' to be ignored
    skipWhitespace(in);
    Compound compound;
    if (in.peek() != '}')
        for (;;)
        {
            string name = read<string>(in);
            skipWhitespace(in);
            expect(in, ':');
            skipWhitespace(in);
            Tag tag = read<Tag>(in);
            compound.insert(make_pair(name, tag));
            skipWhitespace(in);
            if (in.peek() == '}')
                break;
            expect(in, ',');
            skipWhitespace(in);
        }
    in.ignore();
    return compound;
}
template <typename T>
    requires same_as<T, int8_t> || same_as<T, int> || same_as<T, long long>
vector<T> readArray(istream &in)
{
    skipWhitespace(in);
    vector<T> vec;
    if (in.peek() != ']')
        for (;;)
        {
            vec.push_back(read<T>(readUnquotedString(in)));
            skipWhitespace(in);
            if (in.peek() == ']')
                break;
            expect(in, ',');
            skipWhitespace(in);
        }
    in.ignore();
    return vec;
}
template <typename T>
vector<T> readList(istream &in, vector<T> vec)
{
    for (;;)
    {
        skipWhitespace(in);
        if (in.peek() == ']')
        {
            in.ignore();
            return vec;
        }
        expect(in, ',');
        skipWhitespace(in);
        Tag tag = read<Tag>(in);
        vec.push_back(tag.get<T>());
    }
}
inline Tag readListOrArray(istream &in)
{
    in.ignore(); // expect '[' to be ignored
    // read array
    if (char c = in.peek(); c == 'B' || c == 'I' || c == 'L')
    {
        in.ignore();
        expect(in, ';');
        switch (c)
        { // clang-format off
        case 'B': return readArray<int8_t>(in);
        case 'I': return readArray<int>(in);
        case 'L': return readArray<long long>(in);
        } // clang-format on
    }
    // read list
    skipWhitespace(in);
    if (in.peek() == ']')
    {
        in.ignore();
        return vector<monostate>();
    }
    Tag tag = read<Tag>(in);
    return match(tag.getType(), [&in, &tag]<typename U> { return List(readList(in, vector{tag.get<U>()})); });
}
template <typename T>
    requires same_as<T, Tag>
T read(istream &in)
{
    char c = in.peek();
    switch (c)
    {
    case '\"':
    case '\'':
        return readQuotedString(in, c);
    case '{':
        return read<Compound>(in);
    case '[':
        return readListOrArray(in);
    default:
    {
        string s;
        bool hasPoint = false;
        for (char c = in.peek(); isAllowedInUnquotedString(c); c = in.peek())
        {
            if (c == '.')
                hasPoint = true;
            s.push_back(c);
            in.ignore();
        }
        switch (tolower(s.back()))
        { // clang-format off
        case 'b': return readNum<int8_t>(string_view(s.data(), s.size() - 1));
        case 's': return readNum<short>(string_view(s.data(), s.size() - 1));
        case 'l': return readNum<long long>(string_view(s.data(), s.size() - 1));
        case 'f': return readNum<float>(string_view(s.data(), s.size() - 1));
        case 'd': return readNum<double>(string_view(s.data(), s.size() - 1));
        // clang-format on
        default:
            if (s == "true")
                return static_cast<int8_t>(1);
            else if (s == "false")
                return static_cast<int8_t>(0);
            else if (hasPoint)
                return readNum<double>(s);
            else
                return readNum<int>(s);
        }
    }
    }
}
} // namespace detail

template <typename T>
    requires integral<T> || floating_point<T>
inline T read(istream &in)
{
    detail::skipWhitespace(in);
    return detail::read<T>(detail::readUnquotedString(in));
}
template <typename T>
    requires same_as<T, string>
inline T read(istream &in)
{
    detail::skipWhitespace(in);
    char c = in.peek();
    if (c != '\'' && c != '\"')
        throw runtime_error("unexpected character");
    return detail::readQuotedString(in, c);
}
template <typename T>
    requires same_as<T, Compound>
inline T read(istream &in)
{
    detail::skipWhitespace(in);
    detail::check(in, '{');
    return detail::read<T>(in);
}
template <typename T>
    requires same_as<T, List>
inline T read(istream &in)
{
    detail::skipWhitespace(in);
    detail::check(in, '[');
    return detail::readListOrArray(in).get<T>();
}
template <is_list T>
inline T read(istream &in)
{
    detail::skipWhitespace(in);
    detail::check(in, '[');
    return detail::readListOrArray(in).get<T>();
}
template <typename T>
    requires same_as<T, Tag>
inline T read(istream &in)
{
    detail::skipWhitespace(in);
    return detail::read<T>(in);
}
template <typename T>
inline T read(istream &&in)
{
    return read<T>(in);
}
/// Read SNBT from an input stream
inline Tag read(istream &in)
{
    return read<Tag>(in);
}
/// Read SNBT from an input stream
inline Tag read(istream &&in)
{
    return read(in);
}

/// Write SNBT and configure the format
struct Writer
{
    /// Specify indent
    string_view indent = "    ";
    /// Whether throw an exception when meeting an end tag
    bool end_tag_throw = false;
    /// Specify what to write when meeting an end tag
    string_view end_tag = "(End)";
    /// Whether use escape sequences when writing string tags
    bool escape = true;
    /// Whether make line feeds
    bool line_feed = true;
    /// Whether write spaces
    bool space = true;
    /// Specify floating-point formatting for std::to_chars
    chars_format fmt{};
    /// Specify the default size of a buffer
    size_t buffer_size = 100;
    string_view byte_tag_suffix = "b";
    string_view short_tag_suffix = "s";
    string_view int_tag_suffix = "";
    string_view long_tag_suffix = "L";
    string_view float_tag_suffix = "f";
    string_view double_tag_suffix = "d";

    inline void writeSpace(ostream &out) const
    {
        if (space)
            out.put(' ');
    }
    inline void writeString(ostream &out, const string_view s) const
    {
        out.write(s.data(), s.size());
    }
    inline void writeLineFeed(ostream &out, size_t depth) const
    {
        if (line_feed)
        {
            out.put('\n');
            for (size_t i = 0; i < depth; i++)
                writeString(out, indent);
        }
    }
    inline void writeCommaWithSpace(ostream &out) const
    {
        out.put(',');
        writeSpace(out);
    }
    inline void writeCommaWithLineFeed(ostream &out, size_t depth) const
    {
        out.put(',');
        if (line_feed)
            writeLineFeed(out, depth);
        else
            writeSpace(out);
    }
    inline void write(ostream &out, monostate val, size_t depth = 0) const
    {
        if (end_tag_throw)
            throw runtime_error("Reach end tag");
        else
            writeString(out, end_tag);
    }
// Variable-length array (VLA) is required here
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wvla-cxx-extension"
    template <integral T>
    void write(ostream &out, T val, size_t depth = 0) const
    {
        char buf[buffer_size];
        auto [ptr, ec]{to_chars(buf, buf + buffer_size, val)};
        if (ec != errc())
            throw runtime_error("to_chars() error");
        writeString(out, string_view(buf, ptr));
    }
    template <floating_point T>
    void write(ostream &out, T val, size_t depth = 0) const
    {
        char buf[buffer_size];
        auto [ptr, ec]{to_chars(buf, buf + buffer_size, val, fmt)};
        if (ec != errc())
            throw runtime_error("to_chars() error");
        writeString(out, string_view(buf, ptr));
    }
#pragma clang diagnostic pop
    inline void write(ostream &out, int8_t val, size_t depth = 0) const
    {
        write<>(out, val, depth);
        writeString(out, byte_tag_suffix);
    }
    inline void write(ostream &out, short val, size_t depth = 0) const
    {
        write<>(out, val, depth);
        writeString(out, short_tag_suffix);
    }
    inline void write(ostream &out, int val, size_t depth = 0) const
    {
        write<>(out, val, depth);
        writeString(out, int_tag_suffix);
    }
    inline void write(ostream &out, long long val, size_t depth = 0) const
    {
        write<>(out, val, depth);
        writeString(out, long_tag_suffix);
    }
    inline void write(ostream &out, float val, size_t depth = 0) const
    {
        write<>(out, val, depth);
        writeString(out, float_tag_suffix);
    }
    inline void write(ostream &out, double val, size_t depth = 0) const
    {
        write<>(out, val, depth);
        writeString(out, double_tag_suffix);
    }
    inline void write(ostream &out, const string_view s, size_t depth = 0) const
    {
        char mark = s.find('\"') != string_view::npos && s.find('\'') == string_view::npos ? '\'' : '\"';
        out.put(mark);
        if (escape)
        {
            for (char c : s)
                switch (c)
                { // clang-format off
                case '\'': if (mark == '\'') out.put('\\'); out.put('\''); break;
                case '\"': if (mark == '\"') out.put('\\'); out.put('\"'); break;
                case '\\': writeString(out, R"(\\)"); break;
                case '\a': writeString(out, R"(\a)"); break;
                case '\b': writeString(out, R"(\b)"); break;
                case '\f': writeString(out, R"(\f)"); break;
                case '\n': writeString(out, R"(\n)"); break;
                case '\r': writeString(out, R"(\r)"); break;
                case '\t': writeString(out, R"(\t)"); break;
                case '\v': writeString(out, R"(\v)"); break;
                default: out.put(c);
                } // clang-format on
        }
        else
        {
            for (char c : s)
            {
                if (c == '\"' || c == '\\')
                    out.put('\\');
                out.put(c);
            }
        }
        out.put(mark);
    }
    inline void write(ostream &out, const string &s, size_t depth = 0) const
    {
        write(out, string_view(s), depth);
    }
    inline void writeName(ostream &out, const string_view name) const
    {
        bool quoted = false;
        for (char c : name)
            if (!str::detail::isAllowedInUnquotedString(c))
            {
                write(out, name);
                return;
            }
        writeString(out, name);
    }
    template <typename T>
        requires same_as<T, int8_t> || same_as<T, int> || same_as<T, long long>
    void writeArray(ostream &out, const vector<T> &vec, size_t depth = 0) const;
    template <is_array T>
    void write(ostream &out, T val, size_t depth = 0) const
    {
        writeArray(out, val, depth);
    }
    inline void write(ostream &out, const Compound &compound, size_t depth = 0) const;
    template <typename T>
    void writeList(ostream &out, const vector<T> &vec, size_t depth = 0) const;
    inline void write(ostream &out, const List &list, size_t depth = 0) const;
    /// Write SNBT to an output stream
    inline void write(ostream &out, const Tag &tag, size_t depth = 0) const;
    /// Write SNBT to an output stream
    inline void write(ostream &&out, const Tag &tag, size_t depth = 0) const
    {
        write(out, tag, depth);
    }
};
template <typename T>
    requires same_as<T, int8_t> || same_as<T, int> || same_as<T, long long>
void Writer::writeArray(ostream &out, const vector<T> &vec, size_t depth) const
{
    out.put('[');
    if constexpr (same_as<T, int8_t>)
        out.put('B');
    else if constexpr (same_as<T, int>)
        out.put('I');
    else if constexpr (same_as<T, long long>)
        out.put('L');
    else
        static_assert(false, "not a supported type");
    out.put(';'), writeSpace(out);
    if (auto i = vec.begin(); i != vec.end())
        for (;;)
        {
            write(out, *i, depth + 1);
            i++;
            if (i == vec.end())
                break;
            writeCommaWithSpace(out);
        }
    out.put(']');
}
void Writer::write(ostream &out, const Compound &compound, size_t depth) const
{
    out.put('{');
    if (auto i = compound.begin(); i != compound.end())
    {
        writeLineFeed(out, depth + 1);
        for (;;)
        {
            writeName(out, i->first);
            out.put(':'), writeSpace(out);
            write(out, i->second, depth + 1);
            i++;
            if (i == compound.end())
                break;
            writeCommaWithLineFeed(out, depth + 1);
        }
        writeLineFeed(out, depth);
    }
    out.put('}');
}
template <typename T>
void Writer::writeList(ostream &out, const vector<T> &vec, size_t depth) const
{
    constexpr bool needWrap = same_as<T, Compound> || same_as<T, List> || same_as<T, string> || is_vector<T>;
    out.put('[');
    if (auto i = vec.begin(); i != vec.end())
    {
        if constexpr (needWrap)
            writeLineFeed(out, depth + 1);
        for (;;)
        {
            write(out, *i, depth + 1);
            i++;
            if (i == vec.end())
                break;
            if constexpr (needWrap)
                writeCommaWithLineFeed(out, depth + 1);
            else
                writeCommaWithSpace(out);
        }
        if constexpr (needWrap)
            writeLineFeed(out, depth);
    }
    out.put(']');
}
inline void Writer::write(ostream &out, const List &list, size_t depth) const
{
    match(list.getType(), [this, &out, &list, depth]<typename U> { writeList(out, list.get<U>(), depth); });
}
inline void Writer::write(ostream &out, const Tag &tag, size_t depth) const
{
    match(tag.getType(), [this, &out, &tag, depth]<typename U> { write(out, tag.get<U>(), depth); });
}
const Writer stdWriter;
const Writer noLineFeedWriter{.line_feed = false};
const Writer compactWriter{.line_feed = false, .space = false};
} // namespace nbt::str

#endif // _LNBT_HPP