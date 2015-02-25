#ifndef CORESET_H_
#define CORESET_H_

#include "Partition.h"
#include "Point.h"

#include <string>

class Coreset
{
public:
    Coreset(const std::string &file, unsigned int k);
    ~Coreset(void);

    void process();
    void calculateCoreset(int c, int maxDepthT, int d, double costFactorF);
    void writeCoresetToFile(const std::string &file);

private:
    /** Pointer to input file */
    std::string file;

    /** Number of points */
    unsigned int n;

    /** Number of point dimensions */
    unsigned int pointDim;

    /** Number of clusters/centers */
    unsigned int k;

    /** Character separating dimensions of points */
    static const char MATRIX_FILE_DELIMITER = ',';

    /** Maximum number of k-means Iterations */
    static const unsigned int MAX_ITERATIONS = 50;

    /** Vector of points by index */
    std::vector<Point> pointsByIndex;

    /** 2D Distance matrix for all points */
    double** distances;

    /** Vector of partitions of points */
    std::vector<Partition*> partitions;

    /** Vector of partition center points */
    std::vector<Point*> partitionCenters;

    /** Partitions generated during recursive partitioning, tracked for cleanup */
    std::vector<Partition*> intermediatePartitions;

    /** Final partitions for coreset */
    std::vector<Partition*> coresetPartitions;

    /** Resulting coreset */
    std::vector<Point*> coreset;

    void calculateDistanceMatrix();
    void calculateInitialPartitions(int c);
    void calculatePartitions(int maxDepthT, int d, double costFactorF);
    void calculatePartitionsRecursive(Partition* partition, int currentDepth, int maxDepthT, int d, double costFactorF);

    std::vector<Partition*> calculatePotentialPartitions(std::vector<Point*> originalPartitionPoints, unsigned int partitionCount);
    std::vector<Point*> samplePointsByKmeanspp(std::vector<Point*> points, unsigned int sampleSize);
    Point* nearestCenterForPoint(Point* p, std::vector<Point*> centers);
    double euclideanDistance(Point* p1, Point* p2);
    static double string2double(const std::string &s);
    static float string2float(const std::string &s);
    static int random(int min, int max);
    static double randomZeroToOne();
};

#endif
