// Convert little endian NBT to big endian NBT
#include "lnbt.hpp"
#include <fstream>
using namespace std;
int main()
{
    nbt::NBT data = nbt::bin::read<endian::little>(ifstream("little_endian.nbt", ios::binary));
    nbt::bin::write<endian::big>(ofstream("big_endian.nbt", ios::binary), data);
    return 0;
}