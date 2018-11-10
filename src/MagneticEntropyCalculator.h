#include <windows.h>
#include <shlobj.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <regex>
#include "MTData.h"
#include "STData.h"
#pragma comment(lib, "User32.lib")
#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "Ole32.lib")
using namespace std;


class MagneticEntropyCalculater
{

private:
    bool mIsDataFolderPathSet;
    bool mIsOutputFolderPathSet;
    string mDataFolderPath;
    string mDataFolderFullPath;
    string mOutputFolderPath;
    string mOutputFolderFullPath;
    vector<string> mDataFileNameList;
    vector<double> mMagneticFieldList;
    vector<MTData> mMTDataList;
    vector<STdata> mSTDataList;
    double mTemperatureInterval;
    double mMinTemperature;
    double mMaxTemperature;
public:
    MagneticEntropyCalculater();
    bool selectDataFolder();
    bool readAllDataFileName();
    void readAllMTData();
    void showMTData(double magneticField);
    void calculateDerivative();
    void interpolateWithConstantTemperatureInterval(double temperatureInterval);
    void calculateMagneticEntropy(double maxField);
    bool save();
private:

    void readMTData(string filePath);
    bool isValidDataFileName(const char *file_name);
    template<typename T> T findMaxValue(vector<T> &values);
    template<typename T> T findMinValue(vector<T> &values);
    int findNearTmperatureIndex(double temperature, MTData& mtdata);
    double calculateLinearInterpolcationFromAdjacentMagnetizationData(double temperature, MTData& mtdata);
    double calculateLinearInterpolcationFromAdjacentDerivativeData(double temperature, MTData& mtdata);
    void setTemperatureIndexOffset(double minTemperature);
    double integrateWithRespectToFieldAtConstantTemperature(int index, double maxField);
    bool selectOutputFolder();
};
