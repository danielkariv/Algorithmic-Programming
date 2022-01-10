
#ifndef SIMPLEANOMALYDETECTOR_H_
#define SIMPLEANOMALYDETECTOR_H_

#include "anomaly_detection_util.h"
#include "AnomalyDetector.h"
#include <vector>
#include <algorithm>
#include <string.h>
#include <math.h>

struct correlatedFeatures{
	string feature1,feature2;  // names of the correlated features
	float corrlation;
	Line lin_reg;
	float threshold;
    float cx,cy;
};


class SimpleAnomalyDetector:public TimeSeriesAnomalyDetector{
protected:
	vector<correlatedFeatures> cf;
    float minThreshold = 0.9;
public:
	SimpleAnomalyDetector();
	virtual ~SimpleAnomalyDetector();

	virtual void learnNormal(const TimeSeries& ts);
	virtual vector<AnomalyReport> detect(const TimeSeries& ts);

    //virtual void learnHelper(const TimeSeries& ts, int first, int highId, float correlation,Point** points,correlatedFeatures current);
	vector<correlatedFeatures> getNormalModel(){
		return cf;
	}
    virtual void learnHelper(const TimeSeries& ts,int first, int second, float correlation, Point** points);
    virtual bool isAnomaly(float firstValue, float secondValue,correlatedFeatures c);
    float findThreshold(Point** points,int rowCount, correlatedFeatures cf);
    void setMinThreshold(float minThreshold){
        this->minThreshold = minThreshold;
    }
};



#endif /* SIMPLEANOMALYDETECTOR_H_ */
