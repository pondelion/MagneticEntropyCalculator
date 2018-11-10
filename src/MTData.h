#include <vector>
using namespace std;


// Temperature vs Magnetization Data
class MTData 
{
public:
    double mMagneticField;
    vector<double> mTemperature;
    vector<double> mMagnetization;
    vector<double> mDerivative;
    int mTemperatureIndexOffset;
};

bool operator<(const MTData& data1, const MTData data2)
{
    return data1.mMagneticField < data2.mMagneticField;
}

bool operator>(const MTData& data1, const MTData data2)
{
    return data1.mMagneticField > data2.mMagneticField;
}
