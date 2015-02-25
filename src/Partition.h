#ifndef PARTITION_H_
#define PARTITION_H_

#include <vector>

#include "Point.h"

class Partition
{
public:
    Partition(void);
    Partition(const Partition& other);
    Partition(Point* center);
    Partition(Point* center, std::vector<Point*> members);
    ~Partition(void);

    void addMember(Point* point);
    void removeMember(Point* member);
    std::vector<Point*> allMembers();
    double cost();

    Point* center;

private:
    double euclideanDistance(Point* p1, Point* p2);

    std::vector<Point*> members;
};

#endif
