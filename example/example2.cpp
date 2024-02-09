// Print NBT as SNBT
#include "lnbt.hpp"
#include "zstr.hpp"
#include <iostream>
using namespace std;
using nbt::TagType, nbt::Tag, nbt::List, nbt::Compound, nbt::NBT;
const char *indent = "    ";
void print_indent(size_t depth)
{
    for (size_t i = 0; i < depth; i++)
        cout << indent;
}
void print(const Tag &tag, size_t depth);
template <typename T>
    requires same_as<T, int8_t> || same_as<T, int> || same_as<T, long long>
void print_array(const vector<T> &vec, size_t depth = 0)
{
    cout << '[';
    if constexpr (same_as<T, int8_t>)
        cout << 'B';
    else if constexpr (same_as<T, int>)
        cout << 'I';
    else if constexpr (same_as<T, long long>)
        cout << 'L';
    cout << ';';
    auto i = vec.begin();
    if (i != vec.end())
    {
        cout << ' ';
        print(*i, depth + 1);
        for (i++; i != vec.end(); i++)
        {
            cout << ", ";
            print(*i, depth + 1);
        }
    }
    cout << ']';
}
template <typename T>
void print_list(const vector<T> &vec, size_t depth = 0)
{
    constexpr bool need_wrap = same_as<T, Compound> || same_as<T, string> || nbt::is_vector<T>;
    cout << '[';
    auto i = vec.begin();
    if (i != vec.end())
    {
        if constexpr (need_wrap)
        {
            cout << '\n';
            print_indent(depth + 1);
        }
        print(*i, depth + 1);
        for (i++; i != vec.end(); i++)
        {
            if constexpr (need_wrap)
            {
                cout << ",\n";
                print_indent(depth + 1);
            }
            else
                cout << ", ";
            print(*i, depth + 1);
        }
    }
    if (need_wrap)
    {
        cout << '\n';
        print_indent(depth);
    }
    cout << ']';
}
void print_name(const string &name)
{
    if (name.find(' ') == -1 && name.find(':') == -1)
        cout << name;
    else
        cout << '\"' << name << '\"';
}
void print(const pair<string, Tag> &pair, size_t depth = 0)
{
    print_indent(depth + 1);
    print_name(pair.first);
    cout << ": ";
    print(pair.second, depth + 1);
}
void print(const Tag &tag, size_t depth = 0)
{
    switch (tag.getType())
    { // clang-format off
    case TagType::End: cout << "(End)"; break;
    case TagType::Byte: cout << static_cast<int>(tag.get<int8_t>()) << 'b'; break;
    case TagType::Short: cout << tag.get<short>() << 's'; break;
    case TagType::Int: cout << tag.get<int>(); break;
    case TagType::Long: cout << tag.get<long long>() << 'L'; break;
    case TagType::Float: cout << tag.get<float>() << 'f'; break;
    case TagType::Double: cout << tag.get<double>() << 'd'; break;
    case TagType::ByteArray: print_array(tag.get<vector<int8_t>>(), depth); break;
    case TagType::String: cout << '\"' << tag.get<string>() << '\"'; break;
    case TagType::List: {
        List list = tag.get<List>();
        switch (list.getType())
        {
            case TagType::End: print_list(list.get<monostate>(), depth); break;
            case TagType::Byte: print_list(list.get<int8_t>(), depth); break;
            case TagType::Short: print_list(list.get<short>(), depth); break;
            case TagType::Int: print_list(list.get<int>(), depth); break;
            case TagType::Long: print_list(list.get<long long>(), depth); break;
            case TagType::Float: print_list(list.get<float>(), depth); break;
            case TagType::Double: print_list(list.get<double>(), depth); break;
            case TagType::ByteArray: print_list(list.get<vector<int8_t>>(), depth); break;
            case TagType::String: print_list(list.get<string>(), depth); break;
            case TagType::List: print_list(list.get<List>(), depth); break;
            case TagType::Compound: print_list(list.get<Compound>(), depth); break;
            case TagType::IntArray: print_list(list.get<vector<int>>(), depth); break;
            case TagType::LongArray: print_list(list.get<vector<long long>>(), depth); break;
        }
        break;
    }
    case TagType::Compound: {
        cout << "{";
        const Compound &compound = tag.get<Compound>();
        auto i = compound.begin();
        if (i != compound.end())
        {
            cout << '\n';
            print(*i, depth);
            for (i++; i != compound.end(); i++)
            {
                cout << ",\n";
                print(*i, depth);
            }
            cout << '\n';
            print_indent(depth);
        }
        cout << '}';
        break;
    }
    case TagType::IntArray: print_array(tag.get<vector<int>>(), depth); break;
    case TagType::LongArray: print_array(tag.get<vector<long long>>(), depth); break;
    default: throw;
    } // clang-format on
}
void print(const NBT &nbt, size_t depth = 0)
{
    if (!nbt.name.empty())
    {
        print_name(nbt.name);
        cout << ": ";
    }
    print(nbt.tag, depth);
}
int main(int argc, char *argv[])
{
    if (argc <= 1)
    {
        cout << "Please pass the files to be printed as arguments" << endl;
        return 0;
    }
    for (int i = 1; i < argc; i++)
    {
        cout << argv[i] << ":\n";
        print(nbt::read(zstr::ifstream(argv[i], ios::binary)));
        cout << endl;
    }
    return 0;
}