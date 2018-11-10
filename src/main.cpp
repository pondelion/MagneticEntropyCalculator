#include "MagneticEntropyCalculator.h"


int main(void)
{
    MagneticEntropyCalculater mec;
    do {
        if(!mec.selectDataFolder())  // Open select folder dialog.
        {
            break;
        }
        if(!mec.readAllDataFileName()) // 選択されたフォルダから有効なファイル名のみ保存(有効なファイル名は●T.● 例:0.5T.txt , 3T.csv)
        {
            MessageBox(nullptr, (LPCWSTR)"データファイル名の取得に失敗しました", nullptr, MB_OK);
            break;
        }
        mec.readAllMTData(); //それぞれのファイルからMTデータを読み込む
        mec.calculateDerivative(); //dM/dTを計算
        mec.interpolateWithConstantTemperatureInterval(2.0); //指定した温度間隔でデータを線形補間
        //mec.showMTData(0.01);
        //mec.showMTData(0.02);
        mec.calculateMagneticEntropy(2.0); //引数で指定した磁場の磁気エントロピー変化の温度依存性を計算する
        mec.calculateMagneticEntropy(0.5);

        mec.save(); //calculateMagneticEntropy()で計算した結果をtxtファイルに保存する
    } while (0);
    return 0;
}