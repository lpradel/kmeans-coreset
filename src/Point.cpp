#include "Point.h"
#include "Partition.h"

#include <algorithm>
#include <sstream>
#include <string>

Point::Point(void)
{
}

Point::Point(const Point& other)
{
    this->index = other.index;
    this->dim = other.dim;
    this->isCenter = other.isCenter;
    this->coordinates = other.coordinates;
    this->partition = other.partition;
}

Point::Point(int index, int dim, const DoubleVec& coordinates)
{
    this->index = index;
    this->dim = dim;
    this->isCenter = false;

    DoubleVec coords(coordinates);
    this->coordinates = coords;
}

Point::~Point(void)
{
}

std::ostream& operator<<(std::ostream &strm, const Point &point)
{
    std::string pointAsString = "(";
    for (unsigned int i = 0; i < point.dim-1; i++)
    {
        pointAsString = pointAsString + point.double2string(point.coordinates[i]) + ",";
    }
    pointAsString = pointAsString + point.double2string(point.coordinates[point.dim-1]);
    pointAsString = pointAsString + ")";
    return strm << pointAsString;
}

bool Point::operator<(const Point& rhs) const
{
    return (this->coordinates < rhs.coordinates);
}

std::string Point::double2string(double d)
{
    std::ostringstream strs;
    strs << d;
    std::string str = strs.str();
    return str;
}
