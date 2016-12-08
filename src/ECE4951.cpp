#include <stdio.h>
#include <stdlib.h>

#include <opencv2/opencv.hpp>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"

#include<dirent.h>
#include<string.h>
#include <math.h>

#include"nessFunctions.h"
#include"spotExtractor.h"

using namespace cv; //supectting name space issue with abs
using namespace std;



int main(int argc, char** argv )
{
    //Check Arguments
    if ( argc != 3 )
    {
        printf("usage: ECE4951 <Image_Folder_Path> <Output_Folder_Path>\n");
        return -1;
    }

    DIR *dir;
    cv::Mat img;
    std::vector<cv::Point> spots;

    dir = opendir(argv[1]);
    struct dirent *ent;
    string indirectory(argv[1]);
    string outdirectory(argv[2]);
    if(dir != NULL){

        //Loops Through and loads every image in the directory to img.
        while ((ent = readdir (dir)) != NULL) {
            string imgName(ent->d_name);
            string imgPath(indirectory + imgName);
            cout << imgPath << endl;
            img = cv::imread(imgPath);
            //Checks if there is valid image data
            if ( !img.data )
            {
                printf("Not a valid image \n");
            } else {
                cout << "Image is " << img.rows << "x" << img.cols << endl;
                spots = findOpenSpots(img , 0, 1, outdirectory, imgName);

                cout << spots.size()  <<" Open Spots Found" << endl;
                for(int i = 0; i < spots.size(); i++){

                    circle(img, spots[i], 25, Scalar(255, 0, 0), -1);
                    cout << "Spot: " << i << " x=" << spots[i].x  << " , y=" << spots[i].y  << endl;

                }

                

            }
        }
        closedir (dir);
    } else {
        cout << "Could not find dir" << argv[1] << endl;
    }
    

    return 0;
}

