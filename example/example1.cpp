// Convert little endian NBT to big endian NBT
#include "lnbt.hpp"
#include <bit>
#include <fstream>
using namespace std;
int main()
{
    nbt::NBT data = nbt::read<endian::little>(ifstream("little_endian.nbt", ios::binary));
    nbt::write<endian::big>(ofstream("big_endian.nbt", ios::binary), data);
    return 0;
}