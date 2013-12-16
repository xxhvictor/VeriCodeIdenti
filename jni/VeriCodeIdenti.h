#ifndef _VERI_CODE_IDENTI_H_
#define _VERI_CODE_IDENTI_H_
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/ml/ml.hpp>

using namespace std;
using namespace cv;

class MyMLP: public CvANN_MLP
{
    public:
        void mySave();
        void init();
        void hack();
};

class VeriCodeIdenti
{
    public:
        static VeriCodeIdenti& getInstance();
        string identify(Mat& mat);
        void preprocess(Mat& mat, string savePath, string namePrefix);
        void addSample(string fullPath, string label);
        void training();

    private:
        void getBinMat(Mat& mat, Mat& m1, Mat& m2, Mat& m3, Mat& m4);
        void getSingleCharBinMat(Mat& mat, Mat& mat1, Mat& mat2, Mat& mat3, Mat& mat4);
        void initMLP();
        Mat* getLabelVector(string ch);
        VeriCodeIdenti();
        ~VeriCodeIdenti();
        void createMLP();
    private:
        static VeriCodeIdenti* m_instance;
        MyMLP* m_mlp;
        vector<Mat*> m_samples;
        vector<Mat*> m_labels;

};
#endif
