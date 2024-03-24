// Convert SNBT to NBT and convert NBT to SNBT
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
    data = nbt::bin::read<endian::little>(ifstream("little_endian.nbt"));
    nbt::str::stdWriter.write(ofstream("test_output.snbt"), data.tag);
    return 0;
}