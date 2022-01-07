

#ifndef TIMESERIES_H_
#define TIMESERIES_H_
#include <map>
#include <fstream>
#include <iostream>
#include <vector>
#include<iterator>
#include <stdexcept>

using namespace std;

class TimeSeries{
private:
    vector<string> features;
    map<string, vector<float>> table;
public:
	TimeSeries(const char* CSVfileName);
    ~TimeSeries();
    string GetFeatureName(int col) const throw (out_of_range);
    int GetFeatureIndex(string str) const throw (runtime_error);
    int GetColumnAmount() const;
    int GetRowAmount() const;
    float GetValue(int feature_id, int row) const throw(out_of_range,runtime_error);
};



#endif /* TIMESERIES_H_ */
