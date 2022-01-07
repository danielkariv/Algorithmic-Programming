
#include "minCircle.h"

/// return distance between two points.
/// \param a point 1.
/// \param b point 2.
/// \return distance.
float dist(const Point& a, const Point& b)
{
    // calculate distance from known distance algorithm.
    return sqrt(pow(a.x - b.x, 2)
                + pow(a.y - b.y, 2));
}

/// check if given point are inside or on given circle/circle boundary.
/// \param c given circle.
/// \param p points to check.
/// \return true if point inside/on circle.
bool is_inside(const Circle& c, const Point& p)
{
    return dist(c.center, p) <= c.radius;
}

/// Helper method to get a circle defined by 3 points
/// It calculate circle center for circle_from function, when runs on 3 points.
/// \param bx point B, x value.
/// \param by point B, y value.
/// \param cx point C, x value.
/// \param cy point C, y value.
/// \return circle center.
Point get_circle_center(float bx, float by,
                        float cx, float cy)
{
    float B = bx * bx + by * by;
    float C = cx * cx + cy * cy;
    float D = bx * cy - by * cx;
    return { (cy * B - by * C) / (2 * D),
             (bx * C - cx * B) / (2 * D) };
}


/// Helper function to return a circle that intersects 3 points.
/// \param A point 1.
/// \param B point 2.
/// \param C point 3.
/// \return circle that intersects given 3 points.
Circle circle_from(const Point& A, const Point& B,
                   const Point& C)
{
    // an algorithm to calculate a circle intersects with 3 points.
    // seems to calculate first center circle by removing first points from both.
    // and then when has it, offset it by A, and the radius is between center point to A.
    Point I = get_circle_center(B.x - A.x, B.y - A.y,
                                C.x - A.x, C.y - A.y);
 
    I.x += A.x;
    I.y += A.y;
    return { I, dist(I, A) };
}

/// Helper function that returns the smallest circle that intersects 2 points.
/// \param A point 1.
/// \param B point 2.
/// \return smallest circle that intersects 2 points.
Circle circle_from(const Point& A, const Point& B)
{
    // Set the center to be the midpoint of A and B
    Point C = { (A.x + B.x) / 2.0f, (A.y + B.y) / 2.0f};
 
    // Set the radius to be half the distance AB
    return { C, dist(A, B) / 2.0f };
}

/// Helper function that check if given circle encloses the given points.
/// \param c given circle.
/// \param P points that given circle suppose to encloses.
/// \return true if circle does indeed encloses the given points.
bool is_valid_circle(const Circle& c,
                     const vector<Point>& P)
{
    // Run over all points and check if their inside the circle or not.
    for (const Point& p : P)
        if (!is_inside(c, p))
            return false;
    return true;
}

/// Helper function which called when getting enough points to calculate MEC.
/// Happens when don't have more points to check, or found 3 points on boundary of the MEC.
/// \param P points on boundary of the MEC.
/// \return minimum enclosing circle (MEC)
Circle min_circle_trivial(vector<Point>& P)
{
    // 0 <= P.size() <= 3
    // check different cases.
    if (P.empty()) {
        return { { 0, 0 }, 0 };
    }
    else if (P.size() == 1) {
        return { P[0], 0 };
    }
    else if (P.size() == 2) {
        return circle_from(P[0], P[1]);
    }
    // P.size() == 3 so...

    // check if MEC can be determined with 2 points only.
    for (int i = 0; i < 3; i++) {
        for (int j = i + 1; j < 3; j++) {
 
            Circle c = circle_from(P[i], P[j]);
            if (is_valid_circle(c, P))
                return c;
        }
    }
    // determined MEC with 3 points.
    return circle_from(P[0], P[1], P[2]);
}

/// Gets the minimum enclosing circle (MEC) using Welzl's algorithm.
/// Should be O(n) runtime, as it checks up to 3 random points.
/// \param P points to check with.
/// \param R points on circle boundary (Starts from empty vector).
/// \param n points to run over (0-n).
/// \return minimum enclosing circle (MEC)
Circle welzl(Point** P, vector<Point> R, int n)
{
    // Base case when all points processed or |R| = 3
    if (n == 0 || R.size() == 3) {
        return min_circle_trivial(R);
    }
 
    // Pick a random point randomly
    int index = rand() % n;
    Point point = *P[index];
 
    // Put the picked point at the end of P
    // since it's more efficient than
    // deleting from the middle of the vector
    swap(P[index], P[n - 1]);
    
    // Get the MEC circle d from the
    // set of points P - {p}
    Circle d = welzl(P, R, n - 1);
 
    // If d contains p, return d
    if (is_inside(d, point)) {
        return d;
    }
 
    // Otherwise, must be on the boundary of the MEC
    R.push_back(point);
 
    // Return the MEC for P - {p} and R U {p}
    return welzl(P, R, n - 1);
}
/// Finds minimum enclosing circle (MEC), for given points.
/// \param points array of pointers of points to enclose.
/// \param size amount of points.
/// \return
Circle findMinCircle(Point** points,size_t size){
    // random_shuffle
    return welzl(points,{},size);
}