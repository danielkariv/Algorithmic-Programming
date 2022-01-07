
#include "HybridAnomalyDetector.h"

HybridAnomalyDetector::HybridAnomalyDetector(){
	// TODO Auto-generated constructor stub

}

HybridAnomalyDetector::~HybridAnomalyDetector() {
	// TODO Auto-generated destructor stub
}
/// Helper function which deals with finding corrections.
/// \param ts
/// \param first
/// \param second
/// \param correlation
/// \param points
void HybridAnomalyDetector::learnHelper(const TimeSeries& ts,int first, int second, float correlation, Point** points){
    SimpleAnomalyDetector::learnHelper(ts,first,second,correlation,points);
    if(correlation>0.5 && correlation<minThreshold){
        Circle circle = findMinCircle(points,ts.GetRowAmount());
        correlatedFeatures current = correlatedFeatures();
        current.feature1 = ts.GetFeatureName(first);
        current.feature2 = ts.GetFeatureName(second);
        current.corrlation = correlation;
        current.threshold=circle.radius*1.1; // 10% increase
        current.cx=circle.center.x;
        current.cy=circle.center.y;
        cf.push_back(current);
    }
}

/// Check if given values are detected as an anomaly.
/// \param firstValue
/// \param secondValue
/// \param c current.
/// \return true if found anomaly.
bool HybridAnomalyDetector::isAnomaly(float firstValue, float secondValue,correlatedFeatures c){
    return (c.corrlation>=minThreshold && SimpleAnomalyDetector::isAnomaly(firstValue,secondValue,c)) ||
           (c.corrlation>0.5 && c.corrlation<minThreshold && dist(Point(c.cx,c.cy),Point(firstValue,secondValue))>c.threshold);
}
