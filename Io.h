#ifndef IO_h
#define IO_h

#include <iosfwd>

// write a header for a VTK image data in legacy
// format into the stream
void writeImageHeader(std::ostream &os, size_t nx, size_t ny, size_t nz);

// write the given array into the stream in legacy
// format
void writeImageScalar(std::ostream &os, float *s, const char *name,
    size_t nx, size_t ny, size_t nz);

#endif
