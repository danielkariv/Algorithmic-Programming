

#ifndef COMMANDS_H_
#define COMMANDS_H_

#include<iostream>
#include <string.h>

#include <fstream>
#include <vector>
#include "HybridAnomalyDetector.h"

using namespace std;

/// Default DefaultIO (can't be used as is, only inherited classes can be used).
class DefaultIO{
public:
	virtual string read()=0;
	virtual void write(string text)=0;
	virtual void write(float f)=0;
	virtual void read(float* f)=0;
	virtual ~DefaultIO(){}

	// you may add additional methods here

    /// read input and place it in file at 'fileName'.
    void readAndSaveFile(string fileName){
        // open file with fileName
        ofstream out(fileName);
        // read lines till we get "done".
        string s="";
        while((s=read())!="done"){
            out<<s<<endl;
        }
        // close file.
        out.close();
    }
};

/// standardIO using std::cin, and std::cout for read and write.
class standardIO: public DefaultIO{
public:
	standardIO(){

	}
	virtual string read(){
		string text;
		cin >> text;
		return text;
	}
	virtual void write(string text){
		cout << text;
	}
	virtual void write(float f){
		cout << f;
	}
	virtual void read(float* f){
		cin >> *f;
	}
	~standardIO(){
		
	}
};

/// data struct used to save continuous Anomaly Reports.
struct ContinuousAnomalyReport{
    int timeStepStart;
    int timeStepEnd;
    string description;
    bool truePostive; // true when there is a true anomaly happenned in given ContinuousAnomalyReport range.
};

/// data struct used to pass information into and from commends.
struct SharedInformation{
    float threshold;
    vector<AnomalyReport> report;
    vector<ContinuousAnomalyReport> contAnomalyReports;
    int testRowCount;
    SharedInformation(){
        threshold=0.9;
        testRowCount=0;
    }
};

// you may edit this class
/// Default Command (can't be used as is, only inherited classes can be used).
class Command{
protected:
	string description; // command description (used in menu selection).
	DefaultIO* dio;
public:
	Command(DefaultIO* dio, string description):dio(dio),description(description){}
	// added sharedInfo, which has data we can read and change between commands.
    virtual void execute(SharedInformation* sharedinfo)=0;
	virtual ~Command(){}
    string getDescription(){
        return description;
    }
};

// implement here your command classes
/// first Command, allowing uploading Train and Test CSV files.
class UploadCSV : public Command{
public:
    UploadCSV(DefaultIO* dio):Command(dio,"upload a time series csv file"){}
    virtual void execute(SharedInformation* sharedinfo){
        this->dio->write("Please upload your local train CSV file.\n");
        this->dio->readAndSaveFile("anomalyTrain.csv");
        this->dio->write("Upload complete.\n");
        this->dio->write("Please upload your local test CSV file.\n");
        this->dio->readAndSaveFile("anomalyTest.csv");
        this->dio->write("Upload complete.\n");
    }
};

/// second Command, allows changing the correlation threshold ( also called minThreshold in some classes)
class Settings:public Command{
public:
    Settings(DefaultIO* dio):Command(dio,"algorithm settings"){}
    virtual void execute(SharedInformation* sharedinfo){
        // using loop to keep running when value isn't valid.
        bool isDone=false;
        while(!isDone){
            dio->write("The current correlation threshold is ");
            dio->write(sharedinfo->threshold);
            dio->write("\nType a new threshold\n");
            // get input as float.
            float f;
            dio->read(&f);
            // if valid value, change and finish running.
            if(f>0 && f<=1){
                sharedinfo->threshold=f;
                isDone=true;
            }
            else
                dio->write("please choose a value between 0 and 1.\n");
        }
    }
};

/// third Command, allows to learn model and detect for anomalies.
class Detect:public Command{
public:
    Detect(DefaultIO* dio):Command(dio,"detect anomalies"){}
    virtual void execute(SharedInformation* sharedinfo){
        TimeSeries train("anomalyTrain.csv");
        TimeSeries test("anomalyTest.csv");
        sharedinfo->testRowCount = test.GetRowAmount();
        HybridAnomalyDetector ad;
        ad.setMinThreshold(sharedinfo->threshold);
        ad.learnNormal(train);
        sharedinfo->report = ad.detect(test);

        // clear old contAnomalyReports.
        sharedinfo->contAnomalyReports.clear();

        // create ContinuousAnomalyReport reports to use in command UploadAnom
        ContinuousAnomalyReport fr;
        fr.timeStepStart=0;
        fr.timeStepEnd=0;
        fr.description="";
        fr.truePostive=false;
        // run on all Anomaly reports, and find all continuous ones.
        for_each(sharedinfo->report.begin(),sharedinfo->report.end(),[&fr,sharedinfo](AnomalyReport& ar){
            // if we get an AnomalyReport with same desc' and is next timestep.
            if(ar.timeStep==fr.timeStepEnd+1 && ar.description==fr.description)
                fr.timeStepEnd++;
            else{
                // found new anomaly, add last one, and start a new report.
                sharedinfo->contAnomalyReports.push_back(fr);
                fr.timeStepStart=ar.timeStep;
                fr.timeStepEnd=fr.timeStepStart;
                fr.description=ar.description;
            }
        });
        // add last report.
        sharedinfo->contAnomalyReports.push_back(fr);
        sharedinfo->contAnomalyReports.erase(sharedinfo->contAnomalyReports.begin());
        // finish running, we got
        dio->write("anomaly detection complete.\n");
    }
};

/// fourth Command, allows to view all Anomaly reports.
class Results:public Command{
public:
    Results(DefaultIO* dio):Command(dio,"display results"){}
    virtual void execute(SharedInformation* sharedinfo){
        // run over reports and print them.
        for_each(sharedinfo->report.begin(),sharedinfo->report.end(),[this](AnomalyReport& ar){
            dio->write(ar.timeStep);
            dio->write("\t "+ar.description+"\n");
        });
        dio->write("Done.\n");
    }
};

/// fifth Command, allows to upload known anomalies ranges (in CSV), and get True Positive, and False Positive rates.
class UploadAnom:public Command{
public:
    UploadAnom(DefaultIO* dio):Command(dio,"upload anomalies and analyze results"){}

    /// Calculate if report is in given range.
    /// aa - range start, ae - range end, bs - cont anomaly start, be - cont anomaly end
    bool crossSection(int rs,int re,int cas, int cae){
        return (re>=cas && cae>=rs);
    }

    /// Check for given range (start, end) if cont anomaly reports are in range.
    bool IsTruePositive(int start, int end, SharedInformation* sharedinfo){
        for(size_t i=0;i<sharedinfo->contAnomalyReports.size();i++){
            ContinuousAnomalyReport fr=sharedinfo->contAnomalyReports[i];
            if(crossSection(start,end,fr.timeStepStart,fr.timeStepEnd)){
                sharedinfo->contAnomalyReports[i].truePostive=true;
                return true;
            }
        }
        return false;
    }

    virtual void execute(SharedInformation* sharedinfo){
        // reset truePositive values for all reports.
        for(size_t i=0;i<sharedinfo->contAnomalyReports.size();i++){
            sharedinfo->contAnomalyReports[i].truePostive=false;
        }
        // load anomalies ranges CSV, and count True Positive anomalies.
        dio->write("Please upload your local anomalies file.\n");
        string s="";
        float TP=0,sum=0,P=0;
        while((s=dio->read())!="done"){
            size_t t=0;
            // skip ',' in begin.
            for(;s[t]!=',';t++);
            // get two string of start and end numbers.
            string st = s.substr(0,t);
            string en = s.substr(t+1,s.length());
            // convert strings to ints.
            int start = stoi(st);
            int end = stoi(en);
            // look for anomaly in given range, if so, raise counter for True Positive.
            if(IsTruePositive(start, end, sharedinfo))
                TP++;
            sum+=end+1-start;
            P++;
        }
        dio->write("Upload complete.\n");
        // now counting False positive anomalies.
        float FP=0;
        // we tagged all true positive, so we run on all cont anomalies reports, looking for not tagged.
        for(size_t i=0;i<sharedinfo->contAnomalyReports.size();i++)
            if(!sharedinfo->contAnomalyReports[i].truePostive)
                FP++;
        // calculate the rates.
        float N= sharedinfo->testRowCount - sum;
        // Note: the tester seems to want up to 3 digits after dot, so I clamp it using int, and 1000.0f
        float tpr=((int)(1000.0*TP/P))/1000.0f;
        float fpr=((int)(1000.0*FP/N))/1000.0f;
        dio->write("True Positive Rate: ");
        dio->write(tpr);
        dio->write("\nFalse Positive Rate: ");
        dio->write(fpr);
        dio->write("\n");

    }
};

/// sixth Command, allows to stop running and exit the program.
/// Doesn't do anything, the choice to leave is done in main loop when selecting last comment (which is Exit).
class Exit:public Command{
public:
    Exit(DefaultIO* dio):Command(dio,"exit"){}
    virtual void execute(SharedInformation* sharedinfo){
        // the function doesn't do anything, will leave main loop when return.
    }
};
#endif /* COMMANDS_H_ */
