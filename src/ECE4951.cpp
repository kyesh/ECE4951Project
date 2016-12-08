#include <stdio.h>
#include <stdlib.h>

#include <opencv2/opencv.hpp>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"

#include<dirent.h>
#include <iostream>
#include <string>
#include <math.h>

#include"nessFunctions.h"
#include"spotExtractor.h"

using namespace cv; //supectting name space issue with abs
using namespace std;



int main(int argc, char** argv )
{
    //Check Arguments
    if ( argc != 4 )
    {
        printf("usage: ECE4951 <Mode>  <Source> <Output_Folder_Path>\n");
        printf("Mode: 0 = from file, 1 = from camera\n");
        printf("Source: input device number or directory with source photos\n");


        return -1;
    }

    cv::Mat img;
    std::vector<cv::Point> spots;
    string outdirectory(argv[3]);

    if(argv[1][0] == '0'){
        DIR *dir;
        dir = opendir(argv[2]);
        struct dirent *ent;
        string indirectory(argv[2]);
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
                    spots = findOpenSpots(img , 0, 0, outdirectory, imgName);//The second and third params show and save steps
    
                    cout << spots.size()  <<" Open Spots Found" << endl;
                    for(int i = 0; i < spots.size(); i++){
    
                        circle(img, spots[i], 5, Scalar(255, 0, 0), -1);
                        cout << "Spot: " << i << " x=" << spots[i].x  << " , y=" << spots[i].y  << endl;
     
                    }
    
                    cv::namedWindow("Marked Spots", cv::WINDOW_NORMAL );
                    cv::imshow("Marked Spots", img);
                    cv::waitKey(0);
                    
    
                }
            }
            
            closedir (dir);
        } else {
            cout << "Could not find dir" << argv[2] << endl;
        }
    }

    if(argv[1][0] == '1'){
        int cam = argv[2][0] - '0';
        VideoCapture cap(cam); // open the default camera
        if(cap.isOpened()){
    
            //Loops Through and loads every image in the directory to img.
            for(int i = 0; ;i++) {
                string imgName = "cam.png";
                cout << imgName << endl;
                cap >> img;
                //Checks if there is valid image data
                if ( !img.data )
                {
                    printf("Not a valid image \n");
                } else {
                    cout << "Image is " << img.rows << "x" << img.cols << endl;
                    spots = findOpenSpots(img , 0, 0, outdirectory, imgName);//The second and third params show and save steps
    
                    cout << spots.size()  <<" Open Spots Found" << endl;
                    for(int i = 0; i < spots.size(); i++){
    
                        circle(img, spots[i], 5, Scalar(255, 0, 0), -1);
                        cout << "Spot: " << i << " x=" << spots[i].x  << " , y=" << spots[i].y  << endl;
     
                    }
    
                    cv::namedWindow("Marked Spots", cv::WINDOW_NORMAL );
                    cv::imshow("Marked Spots", img);
                    cv::waitKey(0);
                    
    
                }
            }
        } else {
            cout << "Could not find video device" << argv[2] << endl;
        }
    }

    return 0;
}
