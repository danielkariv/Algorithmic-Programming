

#ifndef HYBRIDANOMALYDETECTOR_H_
#define HYBRIDANOMALYDETECTOR_H_

#include "SimpleAnomalyDetector.h"
#include "minCircle.h"

class HybridAnomalyDetector:public SimpleAnomalyDetector {
public:
	HybridAnomalyDetector();
	virtual ~HybridAnomalyDetector();

    virtual void learnHelper(const TimeSeries& ts,int first, int second, float correlation, Point** points);
    virtual bool isAnomaly(float firstValue, float secondValue,correlatedFeatures c);
};
#endif /* HYBRIDANOMALYDETECTOR_H_ */
