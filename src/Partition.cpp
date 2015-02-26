#include "Partition.h"

#include <algorithm>
#include <cmath>

Partition::Partition(void) : members(std::vector<Point*>(0))
{
}

Partition::Partition(const Partition& other)
{
    this->center = other.center;
    this->members = std::vector<Point*>(other.members);
}

Partition::Partition(Point* center) : members(std::vector<Point*>(0))
{
    this->center = center;
}

Partition::Partition(Point* center, std::vector<Point*> members)
{
    this->center = center;
    this->members = std::vector<Point*>(members);
}

Partition::~Partition(void)
{
    this->center = 0;
    this->members.clear();
}

void Partition::addMember(Point* point)
{
    members.push_back(point);
}

void Partition::removeMember(Point* member)
{
    if (!members.empty() && std::find(members.begin(), members.end(), member) != members.end())
    {
        members.erase(std::remove(members.begin(), members.end(), member), members.end());
    }
}

std::vector<Point*> Partition::allMembers()
{
    std::vector<Point*> allMembersIncludingCenter = std::vector<Point*>();

    allMembersIncludingCenter.push_back(center);
    allMembersIncludingCenter.insert(allMembersIncludingCenter.end(), members.begin(), members.end());

    return allMembersIncludingCenter;
}

double Partition::cost()
{
    double cost = 0;

    for (unsigned int i = 0; i < members.size(); i++)
    {
        cost += euclideanDistance(center, members[i]);
    }

    return cost;
}

double Partition::euclideanDistance(Point* p1, Point* p2)
{
    unsigned int pointDim = p1->dim;

    double dist = 0;
    for (unsigned int i = 0; i < pointDim; i++)
    {
        dist += std::pow((p1->coordinates[i] - p2->coordinates[i]), 2);
    }
    return dist;
}
