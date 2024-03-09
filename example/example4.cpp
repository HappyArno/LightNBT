// Get the position of the player from level.dat
#include "lnbt.hpp"
#include "zstr.hpp"
#include <iostream>
using namespace std;
int main(int argc, char *argv[])
{
    if (argc <= 1)
    {
        cout << "Please pass `level.dat` as arguments" << endl;
        return 0;
    }
    for (int i = 1; i < argc; i++)
    {
        cout << argv[i] << ": ";
        nbt::NBT data = nbt::bin::read(zstr::ifstream(argv[i], ios::binary));
        nbt::Compound *player = data.tag.get_if("Data")->get_if<nbt::Compound>("Player");
        string *dimension = player->get_if<string>("Dimension");
        nbt::List *pos = player->get_if<nbt::List>("Pos");
        double *x = pos->get_if<double>(0), *y = pos->get_if<double>(1), *z = pos->get_if<double>(2);
        if (dimension == nullptr || x == nullptr || y == nullptr || z == nullptr)
        {
            cout << "Invalid `level.dat` file" << endl;
            return 0;
        }
        cout << *x << " " << *y << " " << *z << " in " << *dimension << endl;
    }
    return 0;
}