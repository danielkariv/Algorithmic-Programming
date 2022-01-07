
#include "anomaly_detection_util.h"
#include <cmath>
/*  
* Return the avarege of the sum in array of numbers;
*
* @param x - gets float pointer
* @param size-get size of arr
*/
float avg(float* x, int size){

    float total = 0;
    for (int i = 0; i < size; i++)
    {
        total += x[i];
    }
    return total/size;
}
/*  
* returns the variance of X and Y
*
* @param x - gets float pointer
* @param size-get size of arr
*/
float var(float* x, int size){

    float average = avg(x, size);
    float total = 0;
    for (int i = 0; i < size; i++)
    {
        total += x[i]*x[i];
    }
    return total/size - average*average;
}
/*  
* returns the covariance of X and Y;
*
* @param x - gets float pointer
* @param y - gets float pointer
* @param size-get size of arr
*/
float cov(float* x, float* y, int size){

    float total = 0;
    for (int i = 0; i < size; i++)
    {
        total += x[i]*y[i];
    }
    return total/size - avg(x, size)*avg(y, size);
}
/*  
* Return the avarege of the sum in array of numbers;
*
* @param x - gets float pointer
* @param y - gets float pointer
* @param size-get size of arr
*/
float pearson(float* x, float* y, int size){

    return cov(x,y,size)/sqrt(var(x,size)*var(y,size));
}
/*
* performs a linear regression and return s the line equation
*
* @param points- gets pointer of arr points
* @param size-get size of arr
*/
Line linear_reg(Point** points, int size){

    float x[size], y[size];
    for(int i=0; i < size; i++){
        x[i] = points[i]->x;
        y[i] = points[i]->y;
    }
    float a = cov(x, y, size)/var(x, size);
    float b = avg(y, size) - a*avg(x, size);
    return Line(a,b);
}
/*
* @param points- gets pointer of arr points
* @param p - gets Point object
* @param size-get size of arr
*/
float dev(Point p, Point** points, int size){

    Line line = linear_reg(points, size);
    return dev(p,line);
}
/*
* @param p - gets point object 
* @param l- gets Line obect
*/
float dev(Point p,Line l){
    return std::abs(l.f(p.x) - p.y);
}