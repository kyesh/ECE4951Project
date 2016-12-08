#include <opencv2/opencv.hpp>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"


//Start Ken Code
int computeWhiteness(int b,int g,int r){
    int thresh = 125;
    int mean, bdif, gdif, rdif, returnVal;
    if(b > thresh && g > thresh && r > thresh){
       
      mean = (b + g + r)/3;
      bdif = abs(mean-b);
      gdif = abs(mean-g);
      rdif = abs(mean-r);
      returnVal = (mean - 4*bdif - 4*rdif - 4*bdif);
      if(returnVal < 0){
          return 0;
      } else {
          return returnVal;
      }
 
       
    } else {

       return 0 ; 
       //Not really white probably like a gray at this point

    }
}

int computeGreeness(int b,int g,int r){

      int thresh = 120;
      int returnVal;

      if(g > thresh && b < .9*g && r < g){
       
      returnVal = (g);
      if(returnVal < 0){
          return 0;
      } else{
          return returnVal;
      }
 
       
      } else {

      return 0;
 
      }      
       
}


int CreateNessImage(cv::Mat& src, cv::Mat& dst, int (*nessFunction)(int, int, int))
{
    // accept only char type matrices
    CV_Assert(src.depth() == CV_8U);
    
    dst.create(src.size(), src.depth());

    const int channels = src.channels();
//    switch(channels)
//    {
//    case 1:
//        {
//            MatIterator_<uchar> it, end;
//            for( it = I.begin<uchar>(), end = I.end<uchar>(); it != end; ++it)
//                *it = table[*it];
//            break;
//        }
//    case 3:
//        {
            cv::MatIterator_<cv::Vec3b> it_s, end_s;
            cv::MatIterator_<uchar> it_d, end_d;
            it_d = dst.begin<uchar>();
            for( it_s = src.begin<cv::Vec3b>(), end_s = src.end<cv::Vec3b>(), 
                 it_d = dst.begin<uchar>(), end_d = dst.end<uchar>(); 
                 it_s != end_s && it_d != end_d;
                 ++it_s, ++it_d)
            {
                *it_d = nessFunction( (*it_s)[0] , (*it_s)[1] , (*it_s)[2] );
            }
//        }
//    }

    return 0;
}
