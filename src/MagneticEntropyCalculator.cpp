#include "MagneticEntropyCalculator.h"


MagneticEntropyCalculater::MagneticEntropyCalculater()
{
    mDataFolderPath.reserve(MAX_PATH);
    mDataFolderFullPath.reserve(MAX_PATH);
    mIsDataFolderPathSet = false;
    mIsOutputFolderPathSet = false;
}

bool MagneticEntropyCalculater::selectDataFolder()
{
    BROWSEINFO  binfo;
    LPITEMIDLIST idlist;
    char dataFolderPath[MAX_PATH], dataFolderFullPath[MAX_PATH];

    binfo.hwndOwner         = nullptr;
    binfo.pidlRoot          = nullptr;
    binfo.pszDisplayName    = (LPWSTR)dataFolderPath;
    binfo.lpszTitle         = (LPCWSTR)"M-Tﾃﾞｰﾀが保存されているフォルダを選択してください";
    binfo.ulFlags           = BIF_RETURNONLYFSDIRS; 
    binfo.lpfn              = nullptr;              
    binfo.lParam            = 0;               
    binfo.iImage            = 0;

    mDataFolderPath = dataFolderPath;

    if((idlist=SHBrowseForFolder(&binfo))==nullptr)
    {
        MessageBox(nullptr, (LPCWSTR)"M-Tﾃﾞｰﾀが保存されているフォルダを選択してください", (LPCWSTR)"キャンセル",MB_OK);
        return false;
    }
    else
    {
        SHGetPathFromIDList(idlist, (LPCWSTR)dataFolderFullPath);       
        mDataFolderFullPath = dataFolderFullPath;

        CoTaskMemFree(idlist);               
    }
    mIsDataFolderPathSet = true;
    return true;
}

bool MagneticEntropyCalculater::readAllDataFileName()
{
    do {
        if(!mIsDataFolderPathSet)
        {
            MessageBox(nullptr, (LPCWSTR)"ﾃﾞｰﾀフォルダーが選択されていません", nullptr, MB_OK);
            break;
        }
        char buf[MAX_PATH],curdir[MAX_PATH];
        WIN32_FIND_DATA wfd;
        string filename;

        GetCurrentDirectory(MAX_PATH, (LPCWSTR)curdir);
        SetCurrentDirectory((LPCWSTR)mDataFolderFullPath.c_str());
        HANDLE h;
        h = FindFirstFile((LPCWSTR)"*", &wfd);

        if (h == INVALID_HANDLE_VALUE)
        {
            break;
        }

        if (!(wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))
        {
            if(isValidDataFileName((const char *)wfd.cFileName))
            {
                filename = wfd.cFileName;
                mDataFileNameList.push_back(filename);
            }
        }

        while (1)
        {
            if (FindNextFile(h, &wfd))
            {
                if (!(wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))
                {
                    if(isValidDataFileName((const char *)wfd.cFileName))
                    {
                        filename = wfd.cFileName;
                        mDataFileNameList.push_back(filename);
                    }
                }
            }
            else
            {
                break;
            }
        }

        SetCurrentDirectory((LPCWSTR)curdir);

        if (mDataFileNameList.size() == 0)
        {
            MessageBox(nullptr, (LPCWSTR)"選択フォルダ内に有効なデータが一つも見つかりませんでした", nullptr, MB_OK);
            break;
        }
        else
        {
            string namelist;
            for (int i = 0; i < mDataFileNameList.size(); i++)
            {
                namelist += mDataFileNameList[i] + "\n";
            }
            namelist += "をデータファイルとして使用します";
            MessageBox(nullptr, (LPCWSTR)namelist.c_str(), nullptr, MB_OK);
            return true;
        }
    } while (0);

    return false;
}

void MagneticEntropyCalculater::readMTData(string fileName)
{
    ifstream ifs(mDataFolderFullPath + "\\" + fileName);

    if (!ifs)
    {
        MessageBox(nullptr, (LPCWSTR)"ファイルが開けません", nullptr, MB_OK);
        return;
    }

    string buf;
    size_t pos;
    MTData mtdata;
    bool bFormatError = false;

    mtdata.mMagneticField = atof((fileName.substr(0, fileName.find_last_of("T"))).c_str());
    while (ifs && getline(ifs, buf)) 
    {
        if ((pos = buf.find_first_of("\t")) != string::npos)
        {
            mtdata.mTemperature.push_back(atof((buf.substr(0, pos)).c_str()));
            mtdata.mMagnetization.push_back(atof((buf.substr(pos)).c_str()));
        }
        else if ((pos = buf.find_first_of(",")) != string::npos)
        {
            mtdata.mTemperature.push_back(atof((buf.substr(0, pos)).c_str()));
            mtdata.mMagnetization.push_back(atof((buf.substr(pos)).c_str()));
        }
        else
        {
            bFormatError = true;
        }
    }

    if (bFormatError)
    {
        MessageBox(nullptr, (LPCWSTR)"ﾃﾞｰﾀのフォーマットが正しくありません。各データはタブかカンマで区切ってください", nullptr, MB_OK);
    }

    if (mtdata.mTemperature.size() > 1 && mtdata.mMagnetization.size() > 1)
    {
        mMTDataList.push_back(mtdata);
    }
    else
    {
        string str;
        ostringstream os;
        os << mtdata.mMagneticField;
        str += os.str();
        str += "Tのデータはデータ数が少なすぎるため除外しました";
        MessageBox(nullptr, (LPCWSTR)str.c_str(), nullptr, MB_OK);
    }
}

void MagneticEntropyCalculater::readAllMTData()
{
    mMTDataList.clear();
    for (string filename : mDataFileNameList)
    {
        readMTData(filename);
    }
    sort(mMTDataList.begin(), mMTDataList.end());
}

void MagneticEntropyCalculater::showMTData(double magneticField)
{
    for (MTData mtdata : mMTDataList)
    {
        if (mtdata.mMagneticField == magneticField)
        {
            cout << "********* Magnetic Field : " << magneticField << " ****************" << endl;
            cout << "Temperature" << "\t" << "Magnetization" << "\t" << "Derivative" << endl;
            for (int i = 0; i < mtdata.mDerivative.size(); i++)
            {
                cout << mtdata.mTemperature[i] << "\t\t" << mtdata.mMagnetization[i]  << "\t\t" << mtdata.mDerivative[i] << endl;
            }
            cout << "***************************************************" << endl;
            return;
        }
    }
    cout << magneticField << "Tのデータはありません" << endl;
}

bool MagneticEntropyCalculater::isValidDataFileName(const char *file_name)
{
    string name;
    double magneticField;
    name = file_name;

    name = name.substr(0, name.find_last_of("."));
    regex pattern("^\\d+(\\.\\d+)?[T]$");
    smatch match;

    if (!regex_match(name, match, pattern))
    {
        return false;
    }

    name = name.substr(0, name.find_last_of("T"));
    mMagneticFieldList.push_back(atof(name.c_str()));
    return true;
}

void MagneticEntropyCalculater::calculateDerivative()
{
    for (int j = 0; j < mMTDataList.size(); j++)
    {
        for (int i = 1; i < mMTDataList[j].mTemperature.size(); i++)
        {
            mMTDataList[j].mDerivative.push_back( (mMTDataList[j].mMagnetization[i] - mMTDataList[j].mMagnetization[i-1]) / (mMTDataList[j].mTemperature[i] - mMTDataList[j].mTemperature[i-1]));
        }
    }
}

void MagneticEntropyCalculater::interpolateWithConstantTemperatureInterval(double temperatureInterval)
{
    double minTemperature, maxTemperature, mod, m, d;
    vector<double> newTemperature, newMagnetization, newDerivative;
    int index;

    mTemperatureInterval = temperatureInterval;

    mMinTemperature = DBL_MIN;
    mMaxTemperature = DBL_MAX;

    for (int j = 0; j < mMTDataList.size(); j++)
    {
        newTemperature.clear();
        newMagnetization.clear();
        newDerivative.clear();

        mod = fmod(findMinValue(mMTDataList[j].mTemperature),temperatureInterval);

        if (mod != 0.0)
        {
            minTemperature = mMTDataList[j].mTemperature[0] - mod + temperatureInterval;
        }
        else
        {
            minTemperature = mMTDataList[j].mTemperature[0];
        }

        if (mMinTemperature < minTemperature)
        {
            mMinTemperature = minTemperature;
        }

        mod = fmod(findMaxValue(mMTDataList[j].mTemperature),temperatureInterval);
        maxTemperature = mMTDataList[j].mTemperature[mMTDataList[j].mTemperature.size()-1] - mod;

        if (mMaxTemperature > maxTemperature)
        {
            mMaxTemperature = maxTemperature;
        }

        for (double temperature = minTemperature; temperature <= maxTemperature; temperature += temperatureInterval)
        {
            m = calculateLinearInterpolcationFromAdjacentMagnetizationData(temperature, mMTDataList[j]);
            d = calculateLinearInterpolcationFromAdjacentDerivativeData(temperature, mMTDataList[j]);

            newTemperature.push_back(temperature);
            newMagnetization.push_back(m);
            newDerivative.push_back(d);
            //mMTDataList[j].mDerivative.push_back( (mMTDataList[j].mMagnetization[i] - mMTDataList[j].mMagnetization[i-1]) / (mMTDataList[j].mTemperature[i] - mMTDataList[j].mTemperature[i-1]));

        }

        mMTDataList[j].mTemperature = newTemperature;
        mMTDataList[j].mMagnetization = newMagnetization;
        mMTDataList[j].mDerivative = newDerivative;
    }

    setTemperatureIndexOffset(mMinTemperature);
}

template<typename T> T MagneticEntropyCalculater::findMaxValue(vector<T> &values)
{
    T maxVal;

    if (values.size() == 0)
    {
        return nullptr;
    }

    maxVal = values[0];

    for (int i = 1; i < values.size(); i++)
    {
        if (maxVal < values[i])
        {
            maxVal = values[i];
        }
    }

    return maxVal;
}

template<typename T> T MagneticEntropyCalculater::findMinValue(vector<T> &values)
{
    T minVal;

    if (values.size() == 0)
    {
        return nullptr;
    }

    minVal = values[0];

    for (int i = 1; i < values.size(); i++)
    {
        if (minVal > values[i])
        {
            minVal = values[i];
        }
    }

    return minVal;
}

int MagneticEntropyCalculater::findNearTmperatureIndex(double temperature, MTData& mtdata)
{
    if (mtdata.mTemperature.size() == 0 || mtdata.mTemperature.size() == 1)
    {
        return 0;
    }

    for (int i = 0; i < mtdata.mTemperature.size(); i++)
    {
        if ( (mtdata.mTemperature[i] - temperature) * (mtdata.mTemperature[i+1] - temperature)  <= 0.0)
        {
            return i;
        }
    }

    return -1;
}

double MagneticEntropyCalculater::calculateLinearInterpolcationFromAdjacentMagnetizationData(double temperature, MTData& mtdata)
{
    double ratio;
    int index;

    index = findNearTmperatureIndex(temperature, mtdata);

    if (index + 1 > mtdata.mMagnetization.size() || index < 0)
    {
        return 0.0;
    }
    else if (index + 1 == mtdata.mMagnetization.size())
    {
        return mtdata.mMagnetization[index];
    }
    else
    {
        ratio = (temperature - mtdata.mTemperature[index])/( mtdata.mTemperature[index+1] - mtdata.mTemperature[index]);

        return ( ((1 - ratio) * mtdata.mMagnetization[index]) + (ratio * mtdata.mMagnetization[index + 1]));
    }
}

double MagneticEntropyCalculater::calculateLinearInterpolcationFromAdjacentDerivativeData(double temperature, MTData& mtdata)
{
    double ratio;
    int index;

    index = findNearTmperatureIndex(temperature, mtdata);

    if (index + 1 > mtdata.mDerivative.size() || index < 0)
    {
        return 0.0;
    }
    else if (index + 1 == mtdata.mDerivative.size())
    {
        return mtdata.mDerivative[index];
    }
    else
    {
        ratio = (temperature - mtdata.mTemperature[index])/( mtdata.mTemperature[index+1] - mtdata.mTemperature[index]);

        return ( ((1 - ratio) * mtdata.mDerivative[index]) + (ratio * mtdata.mDerivative[index + 1]));
    }
}

void MagneticEntropyCalculater::setTemperatureIndexOffset(double minTemperature)
{

    for (int i = 0; i < mMTDataList.size(); i++)
    {
        mMTDataList[i].mTemperatureIndexOffset = (int)(-(mMTDataList[i].mTemperature[0] - minTemperature)/mTemperatureInterval);
    }

}

void MagneticEntropyCalculater::calculateMagneticEntropy(double maxField)
{
    STData stdata;
    int i;
    double temperature;

    stdata.mMagneticField = maxField;
    for (i = 0, temperature = mMinTemperature; temperature <= mMaxTemperature; temperature += mTemperatureInterval, i++)
    {
        stdata.mTemperature.push_back(temperature);
        stdata.mEntropy.push_back(integrateWithRespectToFieldAtConstantTemperature(i, maxField));
    }

    /*
    for (i = 0; i < stdata.mTemperature.size(); i++)
    {
        cout << stdata.mTemperature[i] << endl;
    }
    for (i = 0; i < stdata.mTemperature.size(); i++)
    {
        cout << stdata.mEntropy[i] << endl;

    }
    */

    for (int i = 0; i < mSTDataList.size(); i++)
    {
        if (mSTDataList[i].mMagneticField == maxField)
        {
            mSTDataList[i] = stdata;
            return;
        }
    }

    mSTDataList.push_back(stdata);

    return;
}

double MagneticEntropyCalculater::integrateWithRespectToFieldAtConstantTemperature(int index, double maxField)
{
    double entropy = 0.0;
    for (int i = 0; mMTDataList[i+1].mMagneticField <= maxField; i++)
    {
        entropy += 0.5*(mMTDataList[i+1].mDerivative[index + mMTDataList[i+1].mTemperatureIndexOffset] + mMTDataList[i].mDerivative[index + mMTDataList[i].mTemperatureIndexOffset]) * (mMTDataList[i+1].mMagneticField - mMTDataList[i].mMagneticField);
    }

    return entropy;
}

bool MagneticEntropyCalculater::save()
{

    if (!selectOutputFolder())
    {
        return false;
    }

    for (STData stdata : mSTDataList)
    {
        ostringstream os;
        os << stdata.mMagneticField;

        ofstream out(mOutputFolderFullPath + "\\" + "S-T_" + os.str() + "T.txt");
        for (int i = 0; i < stdata.mTemperature.size(); i++)
        {
            out << stdata.mTemperature[i] << "\t" << stdata.mEntropy[i] << endl;
        }
    }
    return true;
}

bool MagneticEntropyCalculater::selectOutputFolder()
{
    BROWSEINFO  binfo;
    LPITEMIDLIST idlist;
    char outputFolderPath[MAX_PATH], outputFolderFullPath[MAX_PATH];

    binfo.hwndOwner         = nullptr;
    binfo.pidlRoot          = nullptr;
    binfo.pszDisplayName    = (LPWSTR)outputFolderPath;
    binfo.lpszTitle         = (LPWSTR)"S-Tﾃﾞｰﾀ保存先フォルダを選択してください";
    binfo.ulFlags           = BIF_RETURNONLYFSDIRS; 
    binfo.lpfn              = nullptr;              
    binfo.lParam            = 0;               
    binfo.iImage            = 0;

    mOutputFolderPath = outputFolderPath;

    if((idlist=SHBrowseForFolder(&binfo))==nullptr)
    {
        return false;
    }
    else
    {
        SHGetPathFromIDList(idlist, (LPWSTR)outputFolderFullPath);       
        mOutputFolderFullPath = outputFolderFullPath;

        CoTaskMemFree(idlist);               
    }
    mIsOutputFolderPathSet = true;
    return true;
}
