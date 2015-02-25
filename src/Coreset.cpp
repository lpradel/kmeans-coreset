#include "Coreset.h"
#include "Point.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <fstream>
#include <random>
#include <set>
#include <sstream>
#include <stdexcept>
#include <stdlib.h>
#include <time.h>

Coreset::Coreset(const std::string &file, unsigned int k) : n(0), k(k), pointsByIndex(std::vector<Point>(0)), partitions(std::vector<Partition*>(0)), coresetPartitions(std::vector<Partition*>(0)), coreset(std::vector<Point*>(0))
{
    std::ifstream is(file.c_str());

    if (is.fail())
    {
        throw std::invalid_argument("File " + file + " does not exist or cannot be opened!");
    }

    if (!is.is_open())
    {
        throw std::invalid_argument("File " + file + " does not exist or cannot be opened!");
    }

    // determine dimension of points using one row
    std::ifstream is2(file.c_str());
    std::string line;
    std::getline(is2, line);
    unsigned int pointDim = 0;

    std::stringstream linestream(line);
    std::string value;
    while (std::getline(linestream, value, MATRIX_FILE_DELIMITER))
    {
        pointDim++;
    }
    is2.close();

    this->pointDim = pointDim;
    this->file = file;

    srand((unsigned int) time(NULL));
}

Coreset::~Coreset(void)
{
    pointsByIndex.clear();

    for (unsigned int i = 0; i < partitions.size(); i++)
    {
        delete partitions[i];
    }

    for (unsigned int i = 0; i < intermediatePartitions.size(); i++)
    {
        delete intermediatePartitions[i];
    }
}

void Coreset::process() {

    std::ifstream is(file.c_str());

    if (is.fail())
    {
        throw std::invalid_argument("File " + file + " does not exist or cannot be opened!");
    }

    if (!is.is_open())
    {
        throw std::invalid_argument("File " + file + " does not exist or cannot be opened!");
    }

    std::cout << "Processing input file..." << std::endl;

    unsigned int processedLines = 0;

    std::string line;
    while (std::getline(is, line))
    {
        DoubleVec pointCoordinates;
        pointCoordinates.reserve(pointDim);

        std::stringstream linestream(line);
        std::string value;
        unsigned int i = 0;
        while (std::getline(linestream, value, MATRIX_FILE_DELIMITER))
        {
            pointCoordinates.push_back(string2double(value));
            i++;
        }

        Point point(processedLines, pointDim, pointCoordinates);
        pointsByIndex.push_back(point);

        processedLines++;

        if (processedLines % 10000 == 0)
        {
            std::cout << "Processed " << processedLines << " lines." << std::endl;
        }
    }

    this->n = pointsByIndex.size();

    is.close();
}

void Coreset::writeCoresetToFile(const std::string &file)
{
    std::ofstream os(file.c_str());

    if (os.fail())
    {
        throw std::invalid_argument("File " + file + " does not exist or cannot be opened!");
    }

    if (!os.is_open())
    {
        throw std::invalid_argument("File " + file + " does not exist or cannot be opened!");
    }

    std::cout << "Processing output file..." << std::endl;

    for (unsigned int i = 0; i < coreset.size(); i++)
    {
        os << *(coreset[i]);
        os << std::endl;
    }

    os.close();
}

void Coreset::calculateCoreset(int c, int maxDepthT, int d, double costFactorF)
{
    calculateInitialPartitions(c);

    std::cout << "Initial partitions: " << partitions.size() << std::endl;

    calculatePartitions(maxDepthT, d, costFactorF);

    // Take centers of final partitions as coreset
    for (unsigned int i = 0; i < coresetPartitions.size(); i++)
    {
        Point* coresetPoint = coresetPartitions[i]->center;
        coreset.push_back(coresetPoint);
    }
}

void Coreset::calculateInitialPartitions(int c)
{
    unsigned int numberOfSamples = c * k;

    if (numberOfSamples >= n)
    {
        throw std::invalid_argument("c * k must be < n!");
    }

    // D^2-Sampling from all points
    std::vector<Point*> samplePool(n);
    for (unsigned int i = 0; i < n; i++)
    {
        samplePool[i] = &pointsByIndex[i];
    }

    std::cout << "Sampling initial centers via kmeans++..." << std::endl;

    // Perform sampling
    std::vector<Point*> samples = samplePointsByKmeanspp(samplePool, numberOfSamples);

    std::cout << "Creating initial partitions.." << std::endl;

    // Create partitions
    for (unsigned int i = 0; i < samples.size(); i++)
    {
        Partition* newPartition = new Partition(samples[i]);
        partitions.push_back(newPartition);
        newPartition->center = samples[i];
        
        samples[i]->isCenter = true;
        samples[i]->partition = newPartition;
        partitionCenters.push_back(samples[i]);
    }

    // Assign remaining points to partitions
    for (unsigned int i = 0; i < n; i++)
    {
        if (!pointsByIndex[i].isCenter)
        {
            // Determine closest center/partition
            Point* nearestCenter = nearestCenterForPoint(&pointsByIndex[i], partitionCenters);
            Partition* nearestPartition = nearestCenter->partition;

            // Add point to closest partition
            nearestPartition->addMember(&(pointsByIndex[i]));
            pointsByIndex[i].partition = nearestPartition;
        }
    }
}

void Coreset::calculatePartitions(int maxDepthT, int d, double costFactorF)
{
    int depth = 1;   // we partition once in calculateInitialPartitions

    for (unsigned int i = 0; i < partitions.size(); i++)
    {
        std::cout << "Recursively partitioning subset " << i << "/" << partitions.size() << std::endl;
        calculatePartitionsRecursive(partitions[i], depth, maxDepthT, d, costFactorF);
    }
}

void Coreset::calculatePartitionsRecursive(Partition* partition, int currentDepth, int maxDepthT, int d, double costFactorF)
{
    if (currentDepth == maxDepthT)
    {
        // We have reached max depth, cancel partitioning
        coresetPartitions.push_back(partition);
        return;
    }

    unsigned int sampleSize = d*k;
    std::vector<Point*> allPointsInPartition = partition->allMembers();

    if (sampleSize >= allPointsInPartition.size())
    {
        // We can no longer partition this partition as there are not enough points in it
        coresetPartitions.push_back(partition);
        return;
    }

    // Compute potential d*k-partitioning
    std::vector<Partition*> potentialNewPartitions = calculatePotentialPartitions(allPointsInPartition, sampleSize);

    // Compute k-means-cost of potential partitioning
    double partitioningKMeansCost = 0;
    for (unsigned int i = 0; i < sampleSize; i++)
    {
        partitioningKMeansCost += potentialNewPartitions[i]->cost();
    }

    // Compare k-means-cost of potential partitioning to 1-means cost of original point set
    double centroidcost = partition->cost();

    if (partitioningKMeansCost < (centroidcost * costFactorF))
    {
        // The potential partitioning is cheaper
        for (unsigned int i = 0; i < sampleSize; i++)
        {
            Partition* newPartition = potentialNewPartitions[i];
            std::vector<Point*> newPartitionPoints = newPartition->allMembers();

            potentialNewPartitions[i]->center->partition = potentialNewPartitions[i];
            potentialNewPartitions[i]->center->potentialPartition = 0;
            potentialNewPartitions[i]->center->isCenter = true;
            potentialNewPartitions[i]->center->isPotentialCenter = false;

            for (unsigned int j = 0; j < newPartitionPoints.size(); j++)
            {
                newPartitionPoints[j]->partition = newPartition;
                newPartitionPoints[j]->potentialPartition = 0;
            }

            // Partition recursion
            calculatePartitionsRecursive(newPartition, currentDepth + 1, maxDepthT, d, costFactorF);
        }
    }
    else
    {
        // It is sufficient to replace partition by its center
        coresetPartitions.push_back(partition);

        // Cleanup potential partitions
        for (unsigned int i = 0; i < sampleSize; i++)
        {
            Partition* potentialNewPartition = potentialNewPartitions[i];
            std::vector<Point*> potentialNewPartitionMembers = potentialNewPartition->allMembers();
            
            for (unsigned int j = 0; j < potentialNewPartitionMembers.size(); j++)
            {
                potentialNewPartitionMembers[j]->isPotentialCenter = false;
                potentialNewPartitionMembers[j]->potentialPartition = 0;
            }
        }
    }
}

std::vector<Partition*> Coreset::calculatePotentialPartitions(std::vector<Point*> originalPartitionPoints, unsigned int partitionCount)
{
    std::vector<Partition*> potentialNewPartitions;
    std::vector<Point*> potentialNewCenters;

    // D^2-sampling for potential centers
    std::vector<Point*> centerSamples = samplePointsByKmeanspp(originalPartitionPoints, partitionCount);

    // Compute potential partitions with centers
    for (unsigned int i = 0; i < partitionCount; i++)
    {
        Partition* potentialNewPartition = new Partition(centerSamples[i]);
        potentialNewPartitions.push_back(potentialNewPartition);
        intermediatePartitions.push_back(potentialNewPartition);
        potentialNewPartition->center = centerSamples[i];
        potentialNewCenters.push_back(centerSamples[i]);

        centerSamples[i]->isPotentialCenter = true;
        centerSamples[i]->potentialPartition = potentialNewPartition;
    }

    // Assign remaining points to closest potential center
    for (unsigned int i = 0; i < originalPartitionPoints.size(); i++)
    {
        if (!originalPartitionPoints[i]->isPotentialCenter)
        {
            // Determine closest potential center/partition
            Point* nearestPotentialCenter = nearestCenterForPoint(originalPartitionPoints[i], potentialNewCenters);
            Partition* nearestPotentialPartition = nearestPotentialCenter->potentialPartition;

            // Add point to closest potential partition
            nearestPotentialPartition->addMember(originalPartitionPoints[i]);
            originalPartitionPoints[i]->potentialPartition = nearestPotentialPartition;
        }
    }

    return potentialNewPartitions;
}

std::vector<Point*> Coreset::samplePointsByKmeanspp(std::vector<Point*> points, unsigned int sampleSize)
{
    unsigned int numPoints = points.size();

    if (sampleSize >= numPoints)
    {
        throw std::invalid_argument("Sample size must be smaller than the number of points to sample from!");
    }

    std::vector<Point*> chosenSamples(0);

    // Choose first sample uniformly at random
    int firstSampleIndex = random(0, numPoints-1);
    Point* firstSample = (points[firstSampleIndex]);
    chosenSamples.push_back(firstSample);

    // Choose remaining samples via kmeans++ D^2-sampling
    for (unsigned int i = 1; i < sampleSize; i++)
    {
        std::cout << "Sample " << i << "/" << sampleSize << std::endl;

        // Calculate D^2(x) for all points (and sum thereof)
        std::vector<double> d_squared(numPoints);
        double d_squared_sum = 0;
        for (unsigned int j = 0; j < numPoints; j++)
        {
            Point* pointJ = points[j];
            Point* nearestCenterForPointJ = nearestCenterForPoint(pointJ, chosenSamples);
            double D_pointJ = euclideanDistance(pointJ, nearestCenterForPointJ);
            d_squared[j] = pow(D_pointJ, 2);

            d_squared_sum += d_squared[j];
        }

        // Sample according to D^2-distribution
        double random_0_1 = randomZeroToOne();
        double randomNumber = random_0_1 * d_squared_sum;

        double d_squared_sum_selection = 0;
        for (unsigned int j = 0; j < numPoints; j++)
        {
            d_squared_sum_selection += d_squared[j];

            if (d_squared_sum_selection >= d_squared_sum)
            {
                chosenSamples.push_back(points[j]);
                break;
            }
        }
    }

    // Ensure that no samples are missing, e.g. due to computational inaccuracies
    if (chosenSamples.size() < sampleSize)
    {
        while (chosenSamples.size() < sampleSize)
        {
            int randomSampleIndex = random(0, numPoints-1);
            Point* randomSample = points[randomSampleIndex];

            // Use random sample, unless it was already sampled
            if(std::find(chosenSamples.begin(), chosenSamples.end(), randomSample) != chosenSamples.end()) {
                continue;
            } else {
                chosenSamples.push_back(randomSample);
            }
        }
    }

    return chosenSamples;
}

Point* Coreset::nearestCenterForPoint(Point* p, std::vector<Point*> centers)
{
    double nearestDistance = std::numeric_limits<double>::infinity();
    Point* nearestCenter = 0;

    for (unsigned int i = 0; i < centers.size(); i++)
    {
        double distance = euclideanDistance(p, centers[i]);
        if (distance < nearestDistance)
        {
            nearestDistance = distance;
            nearestCenter = centers[i];
        }
    }

    return nearestCenter;
}

double Coreset::euclideanDistance(Point* p1, Point* p2)
{
    double dist = 0;
    for (unsigned int i = 0; i < pointDim; i++)
    {
        dist += std::pow((p1->coordinates[i] - p2->coordinates[i]), 2);
    }
    return dist;
}

double Coreset::string2double(const std::string &s)
{
    std::istringstream i(s);
    double x;

    if (!(i >> x))
        return 0;
    return x;
}

float Coreset::string2float(const std::string &s)
{
    std::istringstream i(s);
    float x;

    if (!(i >> x))
        return 0;
    return x;
}

int Coreset::random(int min, int max)
{
    return rand()%(max-min + 1) + min;
}

double Coreset::randomZeroToOne()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 1);
    double uniformOn01 = dis(gen);

    return uniformOn01;
}
