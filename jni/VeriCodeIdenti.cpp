#include "VeriCodeIdenti.h"
#include <jni.h>
#include <android/log.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include "TrainData.h"

#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, "xiexh3", __VA_ARGS__)
#define SAMPLE_DIM  300
#define MID_DIM 24
#define CHAR_NUM 21

#define VALID_COLOR_VAL 1 //TODO: 255 is better
#define INVALID_COLOR_VAL  0

typedef map<int, string> MapPos2Char;
static MapPos2Char * s_mapPos2Char = 0;;

static void initMapPos2Char(){
    if(s_mapPos2Char)
        return;
    s_mapPos2Char = new MapPos2Char();
    char chars[CHAR_NUM][2]={
        "3","4","5",
        "6","7","8","9","A",
        "D","E","F","H",
        "K","N",
        "P","R","S","T",
        "U","X","Y",
    };
    for(int i=0; i<CHAR_NUM; ++i)
    {
        string tmpChar(chars[i]);
        s_mapPos2Char->insert(std::pair<int,string>(i, tmpChar));
    }
}

static int ch2Pos(string ch){
    initMapPos2Char();
    MapPos2Char::const_iterator it = s_mapPos2Char->begin();
    MapPos2Char::const_iterator end = s_mapPos2Char->end();
    for( ; it != end; ++it) {
        if( it->second == ch)
            return it->first;
    }
    return 0;
}

static string pos2Ch(int pos)
{
    initMapPos2Char();
    MapPos2Char::iterator it = s_mapPos2Char->find(pos);
    if( it != s_mapPos2Char->end())
        return it->second;
    return "0";
}

static string getClosestCh(const Mat& vec)
{
    double min;
    double max;
    Point maxLoc(0,0);
    minMaxLoc(vec, &min, &max, NULL, &maxLoc);
    //LOGV("min:%f, max:%f, maxLoc: %f, %f", min, max, maxLoc.x, maxLoc.y);

    initMapPos2Char();
    MapPos2Char::iterator it = s_mapPos2Char->find(maxLoc.x);
    if( it != s_mapPos2Char->end())
        return it->second;
    return "0";
}



void MyMLP::mySave()
{
#if 0
    CvMat* layer_sizes;
    CvMat* wbuf;
    CvMat* sample_weights;
    double** weights;
    double f_param1, f_param2;
    double min_val, max_val, min_val1, max_val1;
    int activ_func;
    int max_count, max_buf_sz;
    CvANN_MLP_TrainParams params;
    cv::RNG* rng;
#endif

    // save basic data
    CvFileStorage* fs=cvOpenFileStorage("/sdcard/VeriCodeIdenti/train/basic.yaml", 0, CV_STORAGE_WRITE);//打开文件，用来存储  
    cvWriteReal(fs, "f_param1", f_param1);
    cvWriteReal(fs, "f_param2", f_param2);
    cvWriteReal(fs, "min_val", min_val);
    cvWriteReal(fs, "max_val", max_val);
    cvWriteReal(fs, "min_val1", min_val1);
    cvWriteReal(fs, "max_val1", max_val1);
    cvWriteInt(fs, "activ_func", activ_func);
#if 0
    if( activ_func == 0)
        cvWriteString(fs, "activ_func", "CvANN_MLP::IDENTITY");
    else if( activ_func == 1)
        cvWriteString(fs, "activ_func", "CvANN_MLP::SIGMOID_SYM");
    else
        cvWriteString(fs, "activ_func", "CvANN_MLP::GAUSSIAN");
#endif

    cvWriteInt(fs, "max_count", max_count);
    cvWriteInt(fs, "max_buf_sz", max_buf_sz);
    //
    cvWriteInt(fs, "state", rng->state);
    
    //write param
    cvWriteInt(fs, "type", params.term_crit.type);
    cvWriteInt(fs, "max_iter", params.term_crit.max_iter);
    cvWriteReal(fs, "epsilon", params.term_crit.epsilon);

    cvWriteInt(fs, "train_method", params.train_method);
    cvWriteReal(fs, "bp_dw_scale", params.bp_dw_scale);
    cvWriteReal(fs, "bp_moment_scale", params.bp_moment_scale);
    cvWriteReal(fs, "rp_dw0", params.rp_dw0);
    cvWriteReal(fs, "rp_dw_plus", params.rp_dw_plus);
    cvWriteReal(fs, "rp_dw_minus", params.rp_dw_minus);
    cvWriteReal(fs, "rp_dw_min", params.rp_dw_min);
    cvWriteReal(fs, "rp_dw_max", params.rp_dw_max);

    cvReleaseFileStorage(&fs);//释放文件  
    
    // save mat data
    if( wbuf )
    {
        cvSave("/sdcard/VeriCodeIdenti/train/wbuf.yaml", wbuf);
    }
    if( sample_weights)
    {
        cvSave("/sdcard/VeriCodeIdenti/train/sample_weights.yaml", sample_weights);
    }
    if( layer_sizes)
    {
        cvSave("/sdcard/VeriCodeIdenti/train/layer_sizes.yaml", layer_sizes);
    }

}

void MyMLP::hack()
{
}


VeriCodeIdenti* VeriCodeIdenti::m_instance = NULL;

VeriCodeIdenti& VeriCodeIdenti::getInstance() 
{
    if( m_instance == NULL)
        m_instance = new VeriCodeIdenti();
    return *m_instance;
}

VeriCodeIdenti::VeriCodeIdenti():m_mlp(0)
{
}

VeriCodeIdenti::~VeriCodeIdenti()
{
    if(m_mlp)
        delete m_mlp;
}

void VeriCodeIdenti::createMLP()
{
    if(m_mlp)
        return;
    m_mlp = new MyMLP();
    m_mlp->init();
}

Mat* convertToSampleVector(const Mat& mat)
{
    //convert to a vector
    Mat* sampMat = new Mat(1,SAMPLE_DIM, CV_32FC1);
    for(int i=0; i<SAMPLE_DIM; ++i)
        sampMat->at<float>(0,i) = INVALID_COLOR_VAL;

    int count = SAMPLE_DIM;
    int idx = 0;
    for(int i=0; i< mat.rows; ++i) {
        for( int j =0; j< mat.cols; ++j ) {
            if( idx >= count )
                break;
            sampMat->at<float>(0,idx) = mat.at<unsigned char>(i,j);
            ++idx;
        }
        if( idx >= count )
            break;
    }

    return sampMat;
}

string VeriCodeIdenti::identify(Mat& mat) 
{
    //craate mlp
    createMLP();

    //get binary mat
    Mat mat1, mat2, mat3, mat4;
    getBinMat(mat, mat1, mat2, mat3, mat4);

#if 0
    //for test
    int channels = mat1.channels();
    imwrite("/mnt/sdcard/VeriCodeIdenti/1.png", mat1);
    imwrite("/mnt/sdcard/VeriCodeIdenti/2.png", mat2);
    imwrite("/mnt/sdcard/VeriCodeIdenti/3.png", mat3);
    imwrite("/mnt/sdcard/VeriCodeIdenti/4.png", mat4);
#endif
    
    // do identify
    Mat* mats[4];
    mats[0] = &mat1;
    mats[1] = &mat2;
    mats[2] = &mat3;
    mats[3] = &mat4;
    string chs[4];
    for( int i=0; i< 4; ++i) {
        Mat* sample = convertToSampleVector(*mats[i]);
        Mat respond;
        m_mlp->predict(*sample, respond);
        delete sample;
        chs[i] = getClosestCh(respond);
    }

    return chs[0]+chs[1]+chs[2]+chs[3];
}

void VeriCodeIdenti::preprocess(Mat& mat, string savePath, string namePrefix) 
{
    //get binary mat
    Mat mat1, mat2, mat3, mat4;
    getBinMat(mat, mat1, mat2, mat3, mat4);
    int channels = mat1.channels();
    //save char mat
    imwrite(savePath+"/"+namePrefix +"-1.png", mat1);
    imwrite(savePath+"/"+namePrefix +"-2.png", mat2);
    imwrite(savePath+"/"+namePrefix +"-3.png", mat3);
    imwrite(savePath+"/"+namePrefix +"-4.png", mat4);
    return;
}



Mat* VeriCodeIdenti::getLabelVector(string ch)
{
    Mat* mat = new Mat(1,CHAR_NUM, CV_8U);
    for( int i=0; i< CHAR_NUM; ++i)
        mat->at<unsigned char>(0,i)=INVALID_COLOR_VAL;
    int pos = ch2Pos(ch);
    //ASSERT(pos>=0 && pos<CHAR_NUM);
    mat->at<unsigned char>(0, pos) = VALID_COLOR_VAL;
    return mat;
}

void VeriCodeIdenti::addSample(string fullPath, string label)
{
    Mat* lableVec = getLabelVector(label);
    if( !lableVec)
        return;

    m_labels.push_back(lableVec);

    //read sample
    Mat img = imread(fullPath, CV_LOAD_IMAGE_GRAYSCALE);
    //for test
    //imwrite("/mnt/sdcard/VeriCodeIdenti/channels.png", img);
    Mat * sampMat = convertToSampleVector(img);
    int channels = img.channels();
    m_samples.push_back(sampMat);
}

void VeriCodeIdenti::training()
{
#if 0
    //Setup the BPNetwork  
    CvANN_MLP * bp = new CvANN_MLP();   
    // Set up BPNetwork's parameters  
    CvANN_MLP_TrainParams params;  
    params.train_method=CvANN_MLP_TrainParams::BACKPROP;  
    params.bp_dw_scale=0.1;  
    params.bp_moment_scale=0.1;  
    //params.train_method=CvANN_MLP_TrainParams::RPROP;  
    //params.rp_dw0 = 0.1;   
    //params.rp_dw_plus = 1.2;   
    //params.rp_dw_minus = 0.5;  
    //params.rp_dw_min = FLT_EPSILON;   
    //params.rp_dw_max = 50.;  

    // Set up training data  
    unsigned char labels[3][5] = {{0,0,0,0,0},{1,1,1,1,1},{0,0,0,0,0}};  
    Mat labelsMat(3, 5, CV_32FC1, labels);  
    unsigned char trainingData[3][5] = { {1,2,3,4,5},{111,112,113,114,115}, {21,22,23,24,25} };  
    Mat trainingDataMat(3, 5, CV_32FC1, trainingData);  

    Mat layerSizes=(Mat_<int>(1,5) << 5,2,2,2,5);  
    bp->create(layerSizes,CvANN_MLP::SIGMOID_SYM);//CvANN_MLP::SIGMOID_SYM  
    //CvANN_MLP::GAUSSIAN  
    //CvANN_MLP::IDENTITY  
    bp->train(trainingDataMat, labelsMat, Mat(),Mat(), params);
    return;

#else

    if( m_labels.size() == 0 || m_samples.size() ==0)
        return;

    //create myself every time
    if(m_mlp){
        delete m_mlp;
        m_mlp = 0;
    }
    m_mlp = new MyMLP();
    Mat layersMat = (Mat_<int>(1,3) << SAMPLE_DIM, MID_DIM, CHAR_NUM);
    m_mlp->create(layersMat);

    //construct input and out put mat
    int samplesNum = m_samples.size();
    Mat trainingMat(samplesNum, SAMPLE_DIM, CV_32FC1);
    Mat labelsMat(samplesNum, CHAR_NUM, CV_32FC1);
    for( int i=0; i<samplesNum; ++i)
    {
        for( int j =0; j<SAMPLE_DIM; ++j)
            trainingMat.at<float>(i, j) = m_samples[i]->at<float>(0,j);
        for( int j=0; j<CHAR_NUM; ++j)
            labelsMat.at<float>(i,j) = m_labels[i]->at<float>(0,j);
    }

    // release m_lables and m_samples
    for( int i=0; i<samplesNum;++i){
        Mat* smat = m_samples[i];
        delete smat;
        Mat* lmat = m_labels[i];
        delete lmat;
    }
    m_samples.clear();
    m_labels.clear();

    // start train
#if 0
    CvANN_MLP_TrainParams params;  
    params.train_method=CvANN_MLP_TrainParams::BACKPROP;  
    params.bp_dw_scale=0.1;  
    params.bp_moment_scale=0.1; 
    m_mlp->train(trainingMat,labelsMat, Mat(), Mat(), params);
#else
    CvTermCriteria term;
    term.type = CV_TERMCRIT_ITER;
    term.max_iter = 299;
    term.epsilon = 0.01;
    CvANN_MLP_TrainParams param = CvANN_MLP_TrainParams(term, 
            CvANN_MLP_TrainParams::RPROP, // should use backprpop?
            0.01);
    CvMat tm = trainingMat;
    CvMat lm = labelsMat;
    m_mlp->train(&tm,&lm, NULL, NULL, param);
#endif

    //save data to file
    m_mlp->save("/mnt/sdcard/VeriCodeIdenti/train/train.yaml");
    m_mlp->load("/mnt/sdcard/VeriCodeIdenti/train/train.yaml");
    m_mlp->save("/mnt/sdcard/VeriCodeIdenti/train/train.yaml");
    m_mlp->mySave();
    return;
#endif
}

typedef pair<int, float> HistPair;
typedef vector<HistPair> HistVector;
static bool pair_comp_func( HistPair left, HistPair right)
{
    return left.second > right.second;
}

static void printHist(Mat& hist)
{
    LOGV("dims:%d, rows:%d, cols:%d", hist.dims, hist.rows, hist.cols);

    HistVector v;
    float sums = 0;
    for( int i=0; i<hist.rows; ++i){
        v.push_back(HistPair(i, hist.at<float>(i,0)));
        sums += hist.at<float>(i,0);
    }
    LOGV("sums:%f", sums);
    sort(v.begin(), v.end(), pair_comp_func);
    for( int i=0; i<hist.rows; ++i)
    {
        LOGV("%dth = %f",v[i].first, v[i].second);
    }
}

void VeriCodeIdenti::getBinMat(Mat& mat, Mat& m1, Mat& m2, Mat& m3, Mat& m4)
{
#if 0
    LOGV("origial size: rows:%d, cols:%d", mat.rows, mat.cols);
    Mat hist;
    const int dims = 1;
    int channels[dims] = {0,};
    int histSize[dims] = {256,};
    float greyRanges[]={0,256,};
    const float *ranges[dims] = {greyRanges, };
    calcHist(&mat, 1,  channels, Mat(),
            hist, dims, histSize, ranges, true, false);

    printHist(hist);
    // do not use this
    //threshold(mat, mat, 100, 255, THRESH_BINARY);
#endif

    //extract bounding mat
    int back_grnd_ths_st[8][2]={ 
        {0,0}, 
        {0, mat.cols/2},
        {0,mat.cols-1},
        {mat.rows/2, mat.cols-1},
        {mat.rows-1, mat.cols-1},
        {mat.rows-1, mat.cols/2},
        {mat.rows-1, 0},
        {mat.rows/2, 0},
    };
    const int MIN_BACK_GROUND_THRESHOLD = 128;
    const int MAX_BACK_GROUND_THRESHOLD = 170; 
    int back_grnd_thr = 255;
    for( int i=0; i<8; ++i){
        int c = mat.at<unsigned char>(back_grnd_ths_st[i][0],back_grnd_ths_st[i][1]);
        if( c < back_grnd_thr)
            back_grnd_thr = c;
    }
    int BACK_GROUND_THRESHOLD = back_grnd_thr;
    if( BACK_GROUND_THRESHOLD > MAX_BACK_GROUND_THRESHOLD)
        BACK_GROUND_THRESHOLD = MAX_BACK_GROUND_THRESHOLD;
    if( BACK_GROUND_THRESHOLD < MIN_BACK_GROUND_THRESHOLD)
        BACK_GROUND_THRESHOLD = MIN_BACK_GROUND_THRESHOLD;
    //LOGV("BACK_GROUND_THRESHOLD: %d", BACK_GROUND_THRESHOLD);
    const int CONTINUE_VALID = 1;
    //test
    //Mat clonena = mat.clone();
    //threshold(clonena, clonena, BACK_GROUND_THRESHOLD, 255, THRESH_BINARY_INV);
    //imwrite("/mnt/sdcard/VeriCodeIdenti/b.png", clonena);

    //get top start 
    int topStart = 0;
    for( int i=0; i<mat.rows; ++i){
        for( int j=0; j<mat.cols-CONTINUE_VALID+1; ++j){
            if( mat.at<unsigned char>(i,j) < BACK_GROUND_THRESHOLD ) {
                bool find = true;
                for( int k=1; k < CONTINUE_VALID; ++k ){
                    if( mat.at<unsigned char>(i, j+k) >= BACK_GROUND_THRESHOLD ) {
                        find = false;
                        break;
                    }
                }
                if(find){
                    topStart = i;
                    goto LABEL_FIND_TOP_START;
                }
            }
        }
    }
LABEL_FIND_TOP_START:
    //get bottom start
    int bottomStart = mat.rows;
    for( int i = mat.rows-1; i >=0; --i ){
        for( int j=0; j<mat.cols-CONTINUE_VALID+1; ++j){
            if( mat.at<unsigned char>(i,j) < BACK_GROUND_THRESHOLD ) {
                bool find = true;
                for( int k=1; k < CONTINUE_VALID; ++k ){
                    if( mat.at<unsigned char>(i, j+k) >= BACK_GROUND_THRESHOLD ) {
                        find = false;
                        break;
                    }
                }
                if(find){
                    bottomStart = i+1;
                    goto LABEL_FIND_BOTTOM_START;
                }
            }
        }
    }
LABEL_FIND_BOTTOM_START:
    if( bottomStart > mat.rows)
        bottomStart = mat.rows;
    if(bottomStart<topStart)
        bottomStart = topStart;

    // get left start
    int leftstart = 0;
    for( int i=0; i<mat.cols; ++i){
        for( int j=0; j<mat.rows-CONTINUE_VALID+1; ++j){
            if( mat.at<unsigned char>(j, i) < BACK_GROUND_THRESHOLD) {
                bool find = true;
                for( int k=1; k < CONTINUE_VALID; ++k){
                    if( mat.at<unsigned char>(j+k, i) >= BACK_GROUND_THRESHOLD) {
                        find = false;
                        break;
                    }
                }
                if( find){
                    leftstart = i;
                    goto LABEL_FIND_LEFT_START;
                }
            }
        }
    }
LABEL_FIND_LEFT_START:
    if( leftstart > mat.cols-1)
        leftstart = mat.cols-1;

    // get right start
    int rightstart = mat.cols;
    for( int i = mat.cols-1; i >=0; --i){
        for( int j=0; j<mat.rows-CONTINUE_VALID+1; ++j){
            if( mat.at<unsigned char>(j, i) < BACK_GROUND_THRESHOLD) {
                bool find = true;
                for( int k=1; k < CONTINUE_VALID; ++k){
                    if( mat.at<unsigned char>(j+k, i) >= BACK_GROUND_THRESHOLD) {
                        find = false;
                        break;
                    }
                }
                if( find){
                    rightstart = i+1;
                    goto LABEL_FIND_RIGHT_START;
                }
            }
        }
    }
LABEL_FIND_RIGHT_START:
    if( rightstart > mat.cols)
        rightstart = mat.cols;
    if( rightstart < leftstart)
        rightstart = leftstart;

    // get roi mat
    Range rowRange(topStart, bottomStart);
    Range colRange(leftstart, rightstart);
    Mat roiMat = mat(rowRange, colRange);

    //test
    //imwrite("/mnt/sdcard/VeriCodeIdenti/m.png", roiMat);
       
    //get range
    const int CHAR_NUMBER = 4;
    const int COLOR_RANGE = BACK_GROUND_THRESHOLD-1;//TODO: opt it
    int colors[CHAR_NUMBER]={50,50,50,50};
    int startIdx[CHAR_NUMBER] ={0,10,20,30};
    int endIdx[CHAR_NUMBER]={10,20,30,40};
    for( int chidx=0; chidx<CHAR_NUMBER; ++chidx){
        // get hist start
        int s = 0;
        if( chidx > 0){
            s = endIdx[chidx-1];
            for(int cidx=s; cidx<roiMat.cols; ++cidx){
                for(int ridx=0; ridx<roiMat.rows; ++ridx) {
                    if( roiMat.at<unsigned char>(ridx,cidx)<BACK_GROUND_THRESHOLD) {
                        s = cidx;
                        goto LABEL_FIND_S;
                    }
                }
            }
LABEL_FIND_S:
            if(s >= roiMat.cols-1)
                s = roiMat.cols-1;// make -1
        }

        // get hist end
        const int MIN_CH_COL_LEN = 11;//TODO: opt it
        int minEnd = s+ MIN_CH_COL_LEN;
        if( minEnd > roiMat.cols)
            minEnd = roiMat.cols;
        //int Max_Char_Col_Len = roiMat.cols/4;//TODO: opt it
        const int Max_Char_Col_Len = 13;
        int maxEnd = s + Max_Char_Col_Len;
        if( maxEnd < minEnd)
            maxEnd = minEnd;
        if( maxEnd > roiMat.cols)
            maxEnd = roiMat.cols;
        bool findSep = false;
        int sepPos = minEnd;
        for(int cidx=minEnd; cidx < maxEnd; ++cidx){
            bool colnone = true;

            for(int ridx=0; ridx< roiMat.rows; ++ridx){
                if( roiMat.at<unsigned char>(ridx, cidx) < BACK_GROUND_THRESHOLD){
                    colnone = false;
                    break;
                }
            }

            if(colnone == true){
                findSep = true;
                sepPos = cidx;
                break;
            }
        }

        int el = roiMat.cols/4;//TODO:may be 1/5 is good
        int e = s+el;
        if( e > roiMat.cols)
            e = roiMat.cols;
        if(findSep)
            e = sepPos;

        // get hist
        Range rrange(0,roiMat.rows);
        Range crange(s,e);
        Mat submat = roiMat(rrange, crange);
        Mat subhist;
        const int dims = 1;
        int channels[dims] = {0,};
        int histSize[dims] = {BACK_GROUND_THRESHOLD,};
        float greyRanges[]={0,BACK_GROUND_THRESHOLD-1,};
        const float *ranges[dims] = {greyRanges, };
        calcHist(&submat, 1,  channels, Mat(), subhist, dims, histSize, ranges, true, false);

        //printHist(subhist);
        HistVector v;
        float sums = 0;
        for( int i=0; i<subhist.rows; ++i){//TODO:is it rows
            v.push_back(HistPair(i, subhist.at<float>(i,0)));
            sums += subhist.at<float>(i,0);
        }

        sort(v.begin(), v.end(), pair_comp_func);
        const int HIST_TOP_NUM = 12;//TODO: opt it
        //assert HIST_TOP_NUM < BACK_GROUND_THRESHOLD
        //assert HIST_COUNT_MAX < BACK_GROUND_THRESHOLD
        int his_color[HIST_TOP_NUM];
        float his_acc[HIST_TOP_NUM];
        for(int i=0; i<HIST_TOP_NUM; ++i) {
            his_color[i] = v[i].first;
            his_acc[i]=0;
            int min = his_color[i] - COLOR_RANGE;
            int max = his_color[i] + COLOR_RANGE;
            if( max >= BACK_GROUND_THRESHOLD )
                max = BACK_GROUND_THRESHOLD -1;
            assert(min <= max);
            for(int j=0; j<v.size(); ++j) {
                if( v[j].first >= min && v[j].first <= max){
                    his_acc[i] += v[j].second;
                }
            }
        }

        int max_color = his_color[0];
        float max_color_val = his_acc[0];
        for( int i=0; i<HIST_TOP_NUM; ++i){
            if( his_acc[i] > max_color_val){
                max_color = his_color[i];
                max_color_val = his_acc[i];
            }
        }

        //final get the the color
        colors[chidx] = max_color;

        //get color range
        //TODO:

        // get start
        int low_b_color = colors[chidx] - COLOR_RANGE;
        int up_b_color = colors[chidx] + COLOR_RANGE;
        if( up_b_color >= BACK_GROUND_THRESHOLD)
            up_b_color = BACK_GROUND_THRESHOLD -1;
        assert(low_b_color <= up_b_color);
        int col_s = 0;
        if( chidx > 0){
            col_s = endIdx[chidx-1];
            for( int cidx = col_s; cidx<roiMat.cols; ++cidx){
                for( int ridx =0; ridx<roiMat.rows; ++ridx) {
                    int c = roiMat.at<unsigned char>(ridx, cidx);
                    if( c >= low_b_color && c <= up_b_color){
                        col_s = cidx;
                        goto LABEL_FIND_START_COL;
                    }
                }
            } 
LABEL_FIND_START_COL:
            if( col_s >= roiMat.cols-1)
                col_s = roiMat.cols-1;
        }
        //final get the start
        startIdx[chidx] = col_s;

        // get end
        int min_end_col_idx = startIdx[chidx]+MIN_CH_COL_LEN;
        if( min_end_col_idx > roiMat.cols-1)
            min_end_col_idx = roiMat.cols-1;
        int max_end_col_idx = startIdx[chidx]+Max_Char_Col_Len;
        if( max_end_col_idx > roiMat.cols)
            max_end_col_idx = roiMat.cols;
        bool find_end_col_idx = false;
        int end_col_idx = startIdx[chidx];
        vector<int> in_range_cnt;
        vector<int> conti_cnt;
        for( int cidx = min_end_col_idx; cidx < max_end_col_idx; ++cidx){
            bool all_not_in_range = true;
            int cur_col_cnt = 0;
            int max_conti_cnt =0;
            int cur_conti_cnt =0;
            for( int ridx =0; ridx < roiMat.rows; ++ridx){
                int c = roiMat.at<unsigned char>(ridx, cidx);
                if( c >= low_b_color && c <= up_b_color){
                    all_not_in_range = false;
                    ++cur_col_cnt;
                    ++cur_conti_cnt;
                }else{
                    if( cur_conti_cnt > max_conti_cnt)
                        max_conti_cnt = cur_conti_cnt; 
                    cur_conti_cnt = 0;
                }
            }
            if( all_not_in_range ){
                find_end_col_idx = true;
                end_col_idx = cidx;
                break;
            }

            in_range_cnt.push_back(cur_col_cnt);
            if( cur_conti_cnt > max_conti_cnt)
                max_conti_cnt = cur_conti_cnt;
            conti_cnt.push_back(max_conti_cnt);
        }

        if( find_end_col_idx ){
            // final get it
            endIdx[chidx] = end_col_idx;
        } else if( chidx == CHAR_NUMBER-1)
        {
            // special for last char
            endIdx[chidx] = roiMat.cols-1; 
        } else {
            assert(in_range_cnt.size() == max_end_col_idx-min_end_col_idx);//assert
            assert( conti_cnt.size() == max_end_col_idx-min_end_col_idx);//assert
            const int MIN_CONTI_COUNT = 8;//TODO:opt it
            const int MIN_CONTI_SPAN = 6;//TODO:opt it
            bool find_conti_sep = false;
            int conti_sep = min_end_col_idx;

            int max_conti_start = 0;
            int max_conti_span = 0;
            int cur_conti_start = 0;
            int cur_conti_span = 0;
            for( int i =0; i<conti_cnt.size(); ++i){
                if( conti_cnt[i] >= MIN_CONTI_COUNT ){
                    if( cur_conti_span == 0)
                        cur_conti_start = i;
                    cur_conti_span++;
                } else{
                    if( cur_conti_span > max_conti_span){
                        max_conti_start = cur_conti_start;
                        max_conti_span = cur_conti_span;
                    }
                    cur_conti_span= 0;
                    //cur_conti_start not need to set
                }
            }
            if( cur_conti_span > max_conti_span){
                max_conti_span = cur_conti_span;
                max_conti_start = cur_conti_start;
            }
            if(max_conti_span >= MIN_CONTI_SPAN){
                find_conti_sep = true;
                conti_sep = min_end_col_idx + max_conti_start + max_conti_span/2; 
                if(conti_sep>roiMat.cols)//can this happen?
                    conti_sep = roiMat.cols;
            }
            
            if(find_conti_sep){
                // final get it
                endIdx[chidx] = conti_sep;
            } else {
                if( in_range_cnt.size() > 0 && false) { //TODO:not use it
                    //find min count and cut
                    int min_count_pos = 0;
                    int cur_min_count =  in_range_cnt[0];
                    for( int i=0; i<in_range_cnt.size(); ++i){
                        if( in_range_cnt[i] < cur_min_count ){
                            min_count_pos = i;
                            cur_min_count = in_range_cnt[i];
                        }
                    }
                    // final get it
                    endIdx[chidx] = min_end_col_idx + min_count_pos;
                } else {
                    //hard code it
                    // can this happen?
                    //endIdx[chidx] = startIdx[chidx] + roiMat.cols/4;//TODO:opt it
                     endIdx[chidx] = startIdx[chidx] + 12;//TODO:opt it
                    if( endIdx[chidx] > roiMat.cols)
                        endIdx[chidx] = roiMat.cols;
                }
            }
        }
    }

    //get sub matrix
    Mat* charmats[4];
    charmats[0]= &m1;
    charmats[1]= &m2;
    charmats[2]= &m3;
    charmats[3]= &m4;
    for( int i=0; i<4; ++i){
        Range rowRange(0, roiMat.rows);
        Range colRange(startIdx[i], endIdx[i]);
        *charmats[i] = roiMat(rowRange, colRange);
        Mat& m = *charmats[i];
        int minVal = colors[i] - COLOR_RANGE;
        int maxVal = colors[i] + COLOR_RANGE;
        if( maxVal >= BACK_GROUND_THRESHOLD)
            maxVal = BACK_GROUND_THRESHOLD-1;
        assert(minVal <= maxVal);
        for( int j=0; j < m.rows; ++j){
            for( int k =0; k < m.cols; ++k){
                int val = m.at<unsigned char>(j,k);
                if( val >= minVal && val <= maxVal)
                    m.at<unsigned char>(j,k) = VALID_COLOR_VAL;
                else
                    m.at<unsigned char>(j,k) = INVALID_COLOR_VAL; 
            }
        }
    }

#if 0
    //denoise
    Mat strutMat = (Mat_<unsigned char>(3,3) << 0,1,0,0,1,0,0,1,0);
    dilate(m1,m1, strutMat);
    erode(m1,m1, strutMat);
    dilate(m2,m2, strutMat);
    erode(m2,m2,  strutMat);
    dilate(m3,m3, strutMat);
    erode(m3,m3, strutMat);
    dilate(m4,m4, strutMat);
    erode(m4,m4, strutMat);
#endif

    // for test
    //imwrite("/mnt/sdcard/VeriCodeIdenti/m1.png", m1);
    //imwrite("/mnt/sdcard/VeriCodeIdenti/m2.png", m2);
    //imwrite("/mnt/sdcard/VeriCodeIdenti/m3.png", m3);
    //imwrite("/mnt/sdcard/VeriCodeIdenti/m4.png", m4);

    return;
}

void VeriCodeIdenti::getSingleCharBinMat(Mat& mat, Mat& mat1, Mat& mat2, Mat& mat3, Mat& mat4)
{
    int cols = mat.cols;
    Range rowRange(0, mat.rows);
    Range col1Range(0, mat.cols/4);
    mat1 = mat(rowRange,col1Range);
    Range col2Range(mat.cols/4, mat.cols/2);
    mat2 = mat(rowRange, col2Range);
    Range col3Range(mat.cols/2, mat.cols*3/4);
    mat3 = mat(rowRange, col3Range);
    Range col4Rage(mat.cols*3/4, mat.cols);
    mat4 = mat(rowRange, col4Rage);
    return;
}

void MyMLP::init()
{
#if 0
    Mat layersMat(1,3, CV_32S);
    layersMat.at<int>(0,0)=SAMPLE_DIM;
    layersMat.at<int>(0,1)=30;
    layersMat.at<int>(0,2)=CHAR_NUM;
#endif
    //create
    Mat layersMat = (Mat_<int>(1,3) << SAMPLE_DIM, MID_DIM, CHAR_NUM);
    create(layersMat);

#if 0
    // init basic values
    f_param1 = 6.6666666666666663e-01;
    f_param2 = 1.7159000000000000e+00;
    min_val = -9.4999999999999996e-01;
    max_val = 9.4999999999999996e-01;
    min_val1 = -9.7999999999999998e-01;
    max_val1 = 9.7999999999999998e-01;
    activ_func = SIGMOID_SYM;
    max_count =  1;
    max_buf_sz = 1;
    rng->state = 1;
    // init params and rng
    params.term_crit.type = CV_TERMCRIT_ITER;
    params.term_crit.max_iter = 299;
    params.term_crit.epsilon = 1.1920928955078125e-07;
    params.train_method = 0;
    params.bp_dw_scale = 0;
    params.bp_moment_scale = 0;
    params.rp_dw0 = 0;
    params.rp_dw_plus = 0;
    params.rp_dw_minus = 0;
    params.rp_dw_min = 0;
    params.rp_dw_max = 0;
#endif
    //basci data, auto generated

f_param1= 6.6666666666666663e-01;
f_param2= 1.7159000000000000e+00;
min_val= -9.4999999999999996e-01;
max_val= 9.4999999999999996e-01;
min_val1= -9.7999999999999998e-01;
max_val1= 9.7999999999999998e-01;
activ_func= 1;
max_count= 300;
max_buf_sz= 4096;
rng->state= 398669204;
params.term_crit.type= 3;
params.term_crit.max_iter= 299;
params.term_crit.epsilon= 1.1920928955078125e-07;
params.train_method= 1;
params.bp_dw_scale= 1.0000000000000001e-01;
params.bp_moment_scale= 1.0000000000000001e-01;
params.rp_dw0= 1.0000000000000000e-02;
params.rp_dw_plus= 1.2000000000000000e+00;
params.rp_dw_minus= 5.0000000000000000e-01;
params.rp_dw_min= 0.;
params.rp_dw_max= 50.;

//init matrix
    double* ldata=0;
    int lstep = 0;
    cvGetRawData(layer_sizes, (uchar**)&ldata, &lstep, NULL);
    int layer_data_size = sizeof(layer_data);
    if(lstep != layer_data_size){
        int i =0;//assert
    }
    memcpy(ldata, layer_data, lstep);

    double* wdata = 0;
    int wstep = 0;
    cvGetRawData(wbuf, (uchar**)&wdata, &wstep, NULL);
    int wbuf_data_size = sizeof(wbuf_data);
    if( wbuf_data_size = wstep) {
        int i =0;//assert
    }
    memcpy(wdata, wbuf_data, wstep);

    // weights and sample_weights are not need to init
    
    //remove in future
    //save("/mnt/sdcard/VeriCodeIdenti/train/load.yaml");
}



