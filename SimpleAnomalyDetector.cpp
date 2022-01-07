
#include "SimpleAnomalyDetector.h"

/// Constructor.
SimpleAnomalyDetector::SimpleAnomalyDetector() {
	// TODO Auto-generated constructor stub
    cf = vector<correlatedFeatures>();
}

/// Destructor.
SimpleAnomalyDetector::~SimpleAnomalyDetector() {
	// TODO Auto-generated destructor stub
}

/// Process given database, to generate relations between features (Learning phase).
/// \param ts given database.
void SimpleAnomalyDetector::learnNormal(const TimeSeries& ts){
    // we need an array of feature values arrays.
    float fvalues[ts.GetColumnAmount()][ts.GetRowAmount()];
    for (int i = 0; i < ts.GetColumnAmount(); ++i) {
        for (int j = 0; j < ts.GetRowAmount(); ++j) {
            fvalues[i][j] = ts.GetValue(i,j);
        }
    }
    // find all correlations in default threshold.
    for (int first = 0; first < ts.GetColumnAmount(); ++first) {
        float correlation = 0;
        int highId = -1;
        for (int second = first+1; second < ts.GetColumnAmount(); ++second) {
            float current_correlation = pearson(fvalues[first],fvalues[second],ts.GetRowAmount());
            if(current_correlation > correlation){
                correlation = current_correlation;
                highId = second;
            }
        }
        // we got a correlation so lets add it.
        if(highId != -1) {
        // Need points for lin_reg and threshold calculations.
            Point** points = new Point*[ts.GetRowAmount()];
            for (int i = 0; i < ts.GetRowAmount(); ++i) {
                points[i] = new Point(fvalues[first][i],fvalues[highId][i]);
            }
            // linear regression calculation.
            learnHelper(ts,first,highId,correlation,points);

            // cleanup the points.
            for (int i = 0; i < ts.GetRowAmount(); ++i) {
                delete points[i];
            }
            delete[] points;
        }
    }
}
/// Helper function which deals with finding corrections.
/// \param ts
/// \param first
/// \param second
/// \param correlation
/// \param points
void SimpleAnomalyDetector::learnHelper(const TimeSeries& ts,int first, int second, float correlation, Point** points){
    correlatedFeatures current = correlatedFeatures();
    current.feature1 = ts.GetFeatureName(first);
    current.feature2 = ts.GetFeatureName(second);
    current.corrlation = correlation;
    current.lin_reg = linear_reg(points,ts.GetRowAmount());
    current.threshold = findThreshold(points,ts.GetRowAmount(),current) * 1.1;
    if(current.threshold < minThreshold){
        this->cf.push_back(current);
    }
}

/// Detect anomaly data points from given database, on already learnt correlations between features (Detect phase).
/// \param ts given database.
/// \return anomaly reports.
vector<AnomalyReport> SimpleAnomalyDetector::detect(const TimeSeries& ts){
    vector<AnomalyReport> reports;
    // run over all rows, and each row we run over all correlations and test the data on them.
    for (int i = 0; i < ts.GetRowAmount(); ++i) {
        for (int j = 0; j < cf.size(); ++j) {
            int first = ts.GetFeatureIndex(cf[j].feature1);
            int second = ts.GetFeatureIndex(cf[j].feature2);
            float firstValue = ts.GetValue(first,i);
            float secondValue = ts.GetValue(second,i);
            if(isAnomaly(firstValue,secondValue,cf[j])) {
                // create anomaly report for data points, which overreach the threshold.
                reports.push_back(AnomalyReport(cf[j].feature1 + "-" + cf[j].feature2,i+1));
            }
        }
    }
    return reports;
}
/// Check if given values are detected as an anomaly.
/// \param firstValue
/// \param secondValue
/// \param c current.
/// \return true if found anomaly.
bool SimpleAnomalyDetector::isAnomaly(float firstValue, float secondValue, correlatedFeatures cf){
    float distance = abs(secondValue - cf.lin_reg.f(firstValue));
    return (distance > cf.threshold);
}
/// calculated threshold for given [cf].
/// \param points
/// \param rowCount
/// \param cf
/// \return threshold.
float SimpleAnomalyDetector::findThreshold(Point** points,int rowCount, correlatedFeatures cf){
    float threshold = 0;
    for (int i = 0; i < rowCount; ++i) {
        float curr = abs(points[i]->y - cf.lin_reg.f(points[i]->x));
        if(curr > threshold)
            threshold = curr;
    }
    return threshold;
}