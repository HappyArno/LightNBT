// Convert SNBT to NBT
#include "lnbt.hpp"
#include <fstream>
using namespace std;
int main()
{
    nbt::NBT data;
    data = nbt::str::read(ifstream("test_LF.snbt"));
    nbt::bin::write(ofstream("test_LF.nbt", ios::binary), data);
    data = nbt::str::read(ifstream("test_CRLF.snbt"));
    nbt::bin::write(ofstream("test_CRLF.nbt", ios::binary), data);
    return 0;
}