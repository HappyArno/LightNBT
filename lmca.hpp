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

#ifndef _LMCA_HPP
#define _LMCA_HPP

#include "lnbt.hpp"
#include <array>
#include <functional>
#include <memory>
#include <optional>

namespace mca
{
using namespace std;
using namespace nbt;

/// A chunk
struct Chunk
{
    uint32_t timestamp;
    NBT data;
};
/// A region, which stores a group of 32×32 chunks
struct Region : array<optional<Chunk>, 1024>
{
    /// Get a chunk by its local coordinates within a region
    reference get(size_t x, size_t z)
    {
        return at(x + 32 * z);
    }
    /// Get a chunk by its local coordinates within a region
    const_reference get(size_t x, size_t z) const
    {
        return at(x + 32 * z);
    }
};
template <typename T>
T getValue(istream &in)
{
    T val;
    in.read(reinterpret_cast<char *>(&val), sizeof(T));
    return val;
}
template <endian endian = endian::big, typename T>
T endianswap(T val)
{
    return endian::native == endian ? val : byteswap(val);
}
struct SectorInfo
{
    uint32_t offset;
    uint8_t count;
};
inline SectorInfo getLocation(uint32_t location)
{
    return {endian::native == endian::little ? byteswap(location) >> 8 : location << 8,
            reinterpret_cast<uint8_t *>(&location)[3]};
}
/// Specify the compression scheme
enum class CompressionScheme
{
    GZip = 1,    // GZip (RFC1952)
    Zlib = 2,    // Zlib (RFC1950)
    LZ4 = 4,     // LZ4
    Custom = 127 // Custom compression algorithm
};
/// Define a function type that decompresses an input stream using a given compression scheme
using Decompressor = move_only_function<unique_ptr<istream>(istream &, CompressionScheme)>;
/// Read the data of a chunk from a region file
inline NBT readChunk(istream &region, Decompressor &func, SectorInfo location)
{
    region.seekg(0x1000 * location.offset);
    uint32_t length;
    region.read(reinterpret_cast<char *>(&length), 4);
    length = endianswap(length);
    uint8_t compression_type = region.get();
    switch (compression_type)
    {
    case 3: // Uncompressed
        return bin::read(region);
    case 1:   // GZip
    case 2:   // Zlib
    case 4:   // LZ4
    case 127: // Custom
        return bin::read(*func(region, static_cast<CompressionScheme>(compression_type)));
    default:
        throw runtime_error("unknown compression schemes");
    }
}
inline NBT readChunk(istream &&region, Decompressor &func, SectorInfo location)
{
    return readChunk(region, func, location);
}
/// Read a chunk from a region file
inline Chunk readChunk(istream &region, Decompressor func, size_t x, size_t z)
{
    region.exceptions(istream::eofbit | istream::failbit | istream::badbit);

    size_t offset = x + 32 * z;

    region.seekg(4 * offset);
    SectorInfo location = getLocation(getValue<uint32_t>(region));
    if (location.offset == 0 && location.count == 0)
        throw runtime_error("the chunk doesn't exist in the region file");
    else if (location.offset < 2)
        throw runtime_error("sector overlaps with header");
    else if (location.count == 0)
        throw runtime_error("size has to be > 0");

    region.seekg(0x1000 + 4 * offset);
    uint32_t timestamp = endianswap(getValue<uint32_t>(region));

    return {timestamp, readChunk(region, func, location)};
}
inline Chunk readChunk(istream &&region, Decompressor func, size_t x, size_t z)
{
    return readChunk(region, std::move(func), x, z);
}
/// Read a region from a region file
inline Region readRegion(istream &region, Decompressor func)
{
    region.exceptions(istream::eofbit | istream::failbit | istream::badbit);

    // Header
    uint32_t locations[1024], timestamps[1024];
    region.read(reinterpret_cast<char *>(&locations), sizeof(locations));
    region.read(reinterpret_cast<char *>(&timestamps), sizeof(timestamps));

    // Payload
    Region ret;
    for (size_t i = 0; i < 1024; i++)
    {
        SectorInfo location = getLocation(locations[i]);
        if (location.offset >= 2 && location.count > 0)
            ret[i] = optional(Chunk{endianswap(timestamps[i]), readChunk(region, func, location)});
    }
    return ret;
}
inline Region readRegion(istream &&region, Decompressor func)
{
    return readRegion(region, std::move(func));
}
} // namespace mca

#endif // _LMCA_HPP