#include <stdio.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include<dirent.h>
#include<string.h>

using namespace cv; //supectting name space issue with abs
using namespace std;

int computeWhiteness(int b,int g,int r){
    int thresh = 125;
    int mean, bdif, gdif, rdif, returnVal;
    if(b > thresh && g > thresh && r > thresh){
       
      mean = (b + g + r)/3;
      bdif = abs(mean-b);
      gdif = abs(mean-g);
      rdif = abs(mean-r);
      returnVal = (mean - bdif - rdif -bdif);
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

      int thresh = 80;
      int returnVal;

      if(g > thresh){
       
      returnVal = (g - 2*(b+r));
      if(returnVal < 0){
          return 0;
      } else{
          return returnVal;
      }
 
       
      } else {

      return 0;
 
      }      
       
}


int CreateNessImage(Mat& src, Mat& dst, int (*nessFunction)(int, int, int))
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
            MatIterator_<Vec3b> it_s, end_s;
            MatIterator_<uchar> it_d, end_d;
            it_d = dst.begin<uchar>();
            for( it_s = src.begin<Vec3b>(), end_s = src.end<Vec3b>(), 
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

int main(int argc, char** argv )
{
    //Check Arguments
    if ( argc != 3 )
    {
        printf("usage: ECE4951 <Image_Folder_Path> <Output_Folder_Path>\n");
        return -1;
    }

    cv::Mat img;
    DIR *dir;

    dir = opendir(argv[1]);
    struct dirent *ent;
    string indirectory(argv[1]);
    string outdirectory(argv[2]);
    cout << "Before Table Creation" << endl;
    //Create Tables. Compute values upfront so you don't need to compute them on every itteration.
//    uchar whitePassFilter[256][256][256]; //Apparently these exceed the stack memory need to try something using CV Mat or malloc latter
//    uchar greenPassFilter[256][256][256];
/*    cout << "Before loops" << endl;
    for(int b = 0; b < 256; b++){

        for(int g = 0; g < 256; g++){

            for(int r = 0 ; r < 256; r++){

//                whitePassFilter[b][g][r] = (uchar)computeWhiteness(b,g,r);
//                greenPassFilter[b][g][r] = (uchar)computeGreeness(b,g,r);


            }


        }

    }*/
    cout << "After loops" << endl;
    if(dir != NULL){

        //Loops Through and loads every image in the directory to img.
        while ((ent = readdir (dir)) != NULL) {
            string imgName(ent->d_name);
            string imgPath(indirectory + imgName);
            cout << imgPath << endl;
            cout << img.depth() << endl;
            img = cv::imread(imgPath);
            //Checks if there is valid image data
            if ( !img.data )
            {
                printf("Not a valid image \n");
            } else {
                string outputPath(outdirectory + "SomeQualifier_"  + imgName);
                cv::namedWindow("Display Image", cv::WINDOW_NORMAL );
                cv::imshow("Display Image", img);
                cout << outputPath << endl;
		cv::imwrite(outputPath, img);            
                cv::waitKey(0);//Wait until user presses key to continue
            }
        }
        closedir (dir);
    } else {
        cout << "Could not find dir" << argv[1] << endl;
    }
    

    return 0;
}

