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

    vector<vector<Point> > squares; //Create list of squares

    //Element for Erosion and Dialation Operator
    int erosion_size = 40;
    Mat elementE = getStructuringElement( MORPH_RECT,
                                       Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                       Point( erosion_size, erosion_size ) );
    erosion_size = 5;
    Mat elementD = getStructuringElement( MORPH_RECT,
                                       Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                       Point( erosion_size, erosion_size ) );

    cv::Mat img, img_w, img_g, img_gE, img_gD1, canny, img_gD2;
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
//                string outputPath(outdirectory + "SomeQualifier_"  + imgName);

                //DANGER: Potentail Memory overun should probaly figure out how to delcare this outsied of loop and clear instead of reinitilizing
                vector<vector<Point> > contours;
                vector<Vec4i> hier;

                cv::namedWindow("Display Image", cv::WINDOW_NORMAL );
                cv::namedWindow("White Image", cv::WINDOW_NORMAL );
                cv::namedWindow("Green Image", cv::WINDOW_NORMAL );
                cv::namedWindow("Eroded Green Image", cv::WINDOW_NORMAL );
                cv::namedWindow("Dialated1 Green Image", cv::WINDOW_NORMAL );
//                cv::namedWindow("Dialated2 Green Image", cv::WINDOW_NORMAL );
                cv::namedWindow("Canny", cv::WINDOW_NORMAL );

                CreateNessImage(img, img_w, computeWhiteness);
                CreateNessImage(img, img_g, computeGreeness);
                dilate( img_g, img_gD1, elementD );
         	erode( img_gD1, img_gE, elementE );
                Canny( img_gE, canny, 50, 150, 3 );//Canny is unessary we could use img_gE directly
                findContours(canny.clone(), contours, hier, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

                drawContours(img, contours, -1, Scalar( 0, 0, 255) );

//                dilate( img_gE, img_gD2, elementE );
//                findSquares(img, squares);
//                drawSquares(img, squares);

                cv::imshow("Display Image", img);
                cv::imshow("White Image", img_w);
                cv::imshow("Green Image", img_g);
                cv::imshow("Eroded Green Image", img_gE);
                cv::imshow("Dialated1 Green Image", img_gD1);
                cv::imshow("Canny", canny);

//                cv::imshow("Dialated2 Green Image", img_gD2);


                cout << outdirectory + "SomeQualifier_"  + imgName << endl;
		cv::imwrite(outdirectory + "Original_"  + imgName, img);
                cv::imwrite(outdirectory + "White_"  + imgName, img_w);
                cv::imwrite(outdirectory + "Green_"  + imgName, img_g);
                cv::imwrite(outdirectory + "GreenE_"  + imgName, img_gE);
                cv::imwrite(outdirectory + "GreenD1_"  + imgName, img_gD1);
                cv::imwrite(outdirectory + "Canny_"  + imgName, canny);

//                cv::imwrite(outdirectory + "GreenD2_"  + imgName, img_gD2);
            
                cv::waitKey(0);//Wait until user presses key to continue
            }
        }
        closedir (dir);
    } else {
        cout << "Could not find dir" << argv[1] << endl;
    }
    

    return 0;
}

