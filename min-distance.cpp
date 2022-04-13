#include <iostream>
#include <cmath>
#include <algorithm>
#include <cstdlib> // for malloc
#include <cstring>
#include <cassert>
#include <utility> // for std::pair

struct Point
{
    double x, y;
};

inline bool isZero (double d)
{
    const double EPS = 1e-15;
    return d < EPS && d > -EPS;
}

namespace dak
{
    template <class ParamT, class KeyT, class Higher, class... Args>
    const ParamT* binsearch (const ParamT *first, const ParamT *last, const KeyT &key, Args& ... args)
    {
        const ParamT *start = first, *end = last - 1, *mid;
        int comp = 666;

        Higher fool;

        while (start <= end)
        {
            mid = start + (end - start) / 2;

            comp = fool (mid, key, args...);

            if (comp) 
                end = mid - 1;
            else 
                start = mid + 1;
        }

        return comp ? mid - 1 : mid;
    }
}

struct X_Higher
{
    int operator() (const int *&index, const double &key, const Point *&points)
    {
        return points [*index].x > key;
    }
};

struct Y_Higher
{
    int operator() (const int *index, const double key, const Point *points)
    {
        return points [*index].y > key;
    }
};

inline double calcDistanceSqr (const std::pair <int, int> &pair, const Point *points)
{
    return 
      std::pow (points[pair.first].x - points[pair.second].x, 2)
    + std::pow (points[pair.first].y - points[pair.second].y, 2);
}

std::pair <int, int> findClosestPair (const int *range, const Point *points, int start, int end)
{
    if (end - start < 4)
    {
        std::pair <int, int> result = { range[start], range[start + 1] };
        double min_distance_sqr = INFINITY;

        for (int i = start; i < end; i++)
        for (int j = i + 1; j < end; j++)
            {
                double dist_sqr = calcDistanceSqr ({range[i], range[j]}, points);
                if (isZero (dist_sqr - min_distance_sqr))
                {
                    if (range[i] <= result.first && range[j] <= result.second
                    || range[j] <= result.first && range[i] <= result.second)
                    {
                        min_distance_sqr = dist_sqr;
                        result = {range[i], range[j]};
                        result = {range[i], range[j]};
                    }
                }
                else if (dist_sqr < min_distance_sqr)
                {
                    min_distance_sqr = dist_sqr;
                    result = {range[i], range[j]};
                    result = {range[i], range[j]};
                }
            }
        
        return result;

    }


    int pivot = (start + end) / 2;

    std::pair <int, int> result_1 = findClosestPair (range, points, start, pivot);
    double distance_sqr_1 = calcDistanceSqr (result_1, points);

    std::pair <int, int> result_2 = findClosestPair (range, points, pivot, end);
    double distance_sqr_2 = calcDistanceSqr (result_2, points);

    double min_distance_sqr;
    std::pair <int, int> result;

    if (isZero (distance_sqr_2 - distance_sqr_1))
    {
        if (result_1.first <= result_2.first && result_1.second <= result_2.second
         || result_1.first <= result_2.second && result_1.second <= result_2.first)
        {
            result = result_1;
            min_distance_sqr = distance_sqr_1;
        }
        else
        {
            result = result_2;
            min_distance_sqr = distance_sqr_2;
        }
    }

    else if (distance_sqr_1 < distance_sqr_2)
    {
        result = result_1;
        min_distance_sqr = distance_sqr_1;
    }

    else
    {
        result = result_2;
        min_distance_sqr = distance_sqr_2;
    }


    double min_distance = sqrt (min_distance_sqr);

    // Слияние: нужно найти \a и \b, такие что на [\a, \pivot - 1] и [\pivot, \b]
    // нужно сравнить все пары, отстоящие по y не более чем на \min_distance

    const int *a = dak::binsearch <const int, double, X_Higher, const Point *> (range + start, range + pivot, points[range[pivot]].x - min_distance, points);
    const int *b = dak::binsearch <const int, double, X_Higher, const Point *> (range + pivot, range + end  , points[range[pivot]].x + min_distance, points);

    if (a < range + start) a = range + start;
    if (b < range + pivot) return result; //!

    const int leftSize = (range + pivot) - a;
    const int rightSize = (b + 1) - (range + pivot);
    int leftPointsToCompare [leftSize];
    int rightPointsToCompare [rightSize];
    std::memcpy (leftPointsToCompare, a, leftSize * sizeof (int));
    std::memcpy (rightPointsToCompare, range + pivot, rightSize * sizeof (int));

    std::sort (leftPointsToCompare, leftPointsToCompare + leftSize,
    [points](const int &lhs, const int &rhs) -> bool { return points[lhs].y < points[rhs].y; });

    std::sort (rightPointsToCompare, rightPointsToCompare + rightSize,
    [points](const int &lhs, const int &rhs) -> bool { return points[lhs].y < points[rhs].y; });

    for (int *it = leftPointsToCompare; it < leftPointsToCompare + leftSize; it++)
    {
        const int *lowest = 
        dak::binsearch 
        <const int, double, Y_Higher, const Point *> 
        (rightPointsToCompare, rightPointsToCompare + rightSize, points[*it].y - min_distance, points);

        const int *highest = 
        dak::binsearch 
        <const int, double, Y_Higher, const Point *> 
        (rightPointsToCompare, rightPointsToCompare + rightSize, points[*it].y + min_distance, points)
        + 1;


        if (lowest < rightPointsToCompare)
            lowest = rightPointsToCompare;

        for (const int *itt = lowest; itt < highest; itt++)
        {
            double dist_sqr = calcDistanceSqr ({*it, *itt}, points);
            if (isZero (dist_sqr - min_distance_sqr))
            {
                if (*it <= result.first && *itt <= result.second
                || *itt <= result.first && *it <= result.second)
                {
                    min_distance_sqr = dist_sqr;
                    result = { *it, *itt };
                    result = { *it, *itt };
                }
            }
            if (dist_sqr < min_distance_sqr)
            {   
                min_distance_sqr = dist_sqr;
                min_distance = sqrt (min_distance_sqr);
                result = { *it, *itt };
                result = { *it, *itt };
            }
        }
    }

    return result;

}

int main ()
{
    int n;
    scanf ("%d", &n);

    char *buffer = (char *) malloc (n * sizeof (Point) + n * sizeof (int));

    Point *points = (Point *) buffer;
    int *range = (int *)(buffer + n * sizeof (Point));

    for (int i = 0; i < n; i++)
    {
        scanf ("%lf %lf", &(points[i].x), &(points[i].y));

        range[i] = i;
    }

    std::sort (range, range + n, [points](const int &lhs, const int &rhs) -> bool { return points[lhs].x < points[rhs].x; });

    /*printf ("Range: ");
    for (int i = 0; i < n; i++)
        printf ("%d ", range[i]);
    printf ("\n");*/

    std::pair <int, int> closestPair = findClosestPair (range, points, 0, n);
    if (closestPair.first > closestPair.second) std::swap (closestPair.first, closestPair.second);
    printf ("%.10lf %d %d\n", sqrt (calcDistanceSqr (closestPair, points)), closestPair.first + 1, closestPair.second + 1);


    free (buffer);
}
