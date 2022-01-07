
#include "timeseries.h"

/// Constructor.
/// \param CSVfileName pathfile for CSV to load.
TimeSeries::TimeSeries(const char *CSVfileName) {
    // load file from given pathfile.
    ifstream in(CSVfileName);
    string line;
    // first line has the features' description, so lets extract it, and place into a vector.
    getline(in,line);
    //cout << line << "\n"; // DEBUG line.
    std::size_t max_line = line.size();
    // run over text between ',', and edges, and place them in vector (those are column names).
    std::size_t from = 0;
    std::size_t to = line.find(",");

    while(to < max_line){
        // npos meaning it didn't find (read about substr or find functions).
        if(to == std::string::npos)
            to = max_line;
        // get feature and put in vector.
        string feature = line.substr(from,to-from);
        features.push_back(feature);
        table.insert({feature,vector<float>()});
        // recalculate next feature positions.
        from = to + 1;
        to = line.find(",",from);
    }
    // reach end of line, but didn't add the last feature, lets add it.
    string feature = line.substr(from,to-from);
    // check if one of features has 'Time' in name (npos meaning it didn't find).
    features.push_back(feature);
    table.insert({feature,vector<float>()});

    // run on all other lines, and place each line data into a vector, and time used as a key for map.
    while(getline(in,line)){
        // similar to how the first line extraction works, just with floats instead of strings.
        from = 0;
        to = line.find(",");
        max_line = line.size();
        int count = 0;
        while(to < max_line){
            // didn't find next ',' so we set it to take till the end line.
            if(to == std::string::npos)
                to = max_line;
            // get feature value string and put in vector. (stof - function to convert string to float)
            string val_str = line.substr(from,to-from);
            table.at(features[count]).push_back(stof(val_str));
            // recalculate next feature positons.
            from = to + 1;
            to = line.find(",",from);
            count++;
        }
        // reach end of line, but didn't add the last feature, so we add it now.
        string val_str = line.substr(from,to-from);
        table.at(features[count]).push_back(stof(val_str));
    }
    in.close();
}

/// Destructor.
TimeSeries::~TimeSeries() {
    // table and its members are clearing automatic (no pointers used there).
}

/// Get the name for the features of given column number.
/// \param col column number.
/// \return feature name.
string TimeSeries::GetFeatureName(int col) const throw (out_of_range){
    // if col isn't in range, return null.
    if (col < 0 || col > features.size()-1)
        throw out_of_range("Column value is out of range (need to be between 0 to features amount).");
    else
        return features.at(col);
}
/// Get column number/index for given name.
/// \param str feature name to look for.
/// \return column number/index.
int TimeSeries::GetFeatureIndex(string str) const throw (runtime_error) {
    // search for matching name in vector.
    for (int i = 0; i < features.size(); ++i) {
        if(features.at(i) == str)
            return i;
    }
    // it didn't find, so we throw an exception.
    throw runtime_error("Couldn't find feature with given name.");
}

/// Gets number of columns.
/// \return column amount.
int TimeSeries::GetColumnAmount() const {
    return features.size();
}

/// Gets number of rows.
/// \return row amount.
int TimeSeries::GetRowAmount() const {
    if(GetColumnAmount() == 0)
        return 0;
    return table.at(features[0]).size();
}

/// Get value at given column/feature and row.
/// \param feature_id column number/index.
/// \param row row number/index.
/// \return value.
float TimeSeries::GetValue(int feature_id, int row) const throw(out_of_range,runtime_error) {
    string name = GetFeatureName(feature_id);
    if (row < 0 || row > GetRowAmount())
        throw out_of_range("Row value is out of range (needs to be between 0 to rows amount).");

    return table.at(name)[row];
}
