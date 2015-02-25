#ifndef POINT_H_
#define POINT_H_

#include <fstream>
#include <ostream>
#include <string>
#include <vector>

typedef std::vector<std::vector<double> > DoubleMat;
typedef std::vector<double> DoubleVec;

class Partition;

class Point
{
public:
    Point(void);
    Point(const Point& other);
    Point(int index, int dim, const DoubleVec& coordinates);
    ~Point(void);

    bool operator<(const Point& rhs) const;

    unsigned int index;
    unsigned int dim;
    DoubleVec coordinates;
    
    Partition* partition;
    Partition* potentialPartition;
    bool isCenter;
    bool isPotentialCenter;

private:
    friend std::ostream& operator<<(std::ostream&, const Point&);
    //friend std::ofstream& operator<<(std::ofstream&, const Point&);
    static std::string double2string(double d);
};

#endif
