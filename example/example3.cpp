// Print NBT as SNBT
#include "lnbt.hpp"
#include "zstr.hpp"
#include <iostream>
using namespace std;
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
        nbt::str::stdWriter.write(cout, nbt::bin::read(zstr::ifstream(argv[i], ios::binary)).tag);
        cout << endl;
    }
    return 0;
}