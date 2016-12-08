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

//Stark Mike Code

// helper function:
// finds a cosine of angle between vectors
// from pt0->pt1 and from pt0->pt2
static double angle( Point pt1, Point pt2, Point pt0 )
{
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

// returns sequence of squares detected on the image.
// the sequence is stored in the specified memory storage
static void findSquares( const Mat& image, vector<vector<Point> >& squares )
{
    squares.clear();//clear the squares array

    Mat pyr, timg, gray0(image.size(), CV_8U), gray;

    // down-scale and upscale the image to filter out the noise
    pyrDown(image, pyr, Size(image.cols/2, image.rows/2));
    pyrUp(pyr, timg, image.size());
    vector<vector<Point> > contours;

    // find squares in every color plane of the image
    for( int c = 0; c < 3; c++ )
    {
        int ch[] = {c, 0};
        int N = 11;//Number of levels for edge detection
        int thresh = 50;
        mixChannels(&timg, 1, &gray0, 1, ch, 1);// copies specified channels from input array to output array
	//timg is source array, 1=number of matricies in timg
	//gray0=destination array, 1=number of matricies in gray0

        // try several threshold levels
        for( int l = 0; l < N; l++ )
        {
            // hack: use Canny instead of zero threshold level.
            // Canny helps to catch squares with gradient shading
            if( l == 0 )
            {
                // apply Canny. Take the upper threshold from slider
                // and set the lower to 0 (which forces edges merging)

                Canny(gray0, gray, 0, thresh, 5);//canny finds edges in an image
		//gray0 is source image, gray is is output image, 0, the 0, thresh and 5 is aperature size??

                // dilate canny output to remove potential
                // holes between edge segments
                dilate(gray, gray, Mat(), Point(-1,-1));
            }
            else
            {
                // apply threshold if l!=0:
                //     tgray(x,y) = gray(x,y) < (l+1)*255/N ? 255 : 0
                gray = gray0 >= (l+1)*255/N;
            }

            // find contours and store them all as a list

	    //find contours takes a binary image (gray here), and finds all countours within it.	
	    // outputs it to the countours vector. RETR_list=retrieve all countors without heirarchy
	    //Chain_approx_simple=compress horizontal, vertical, and diagonal segments and leaves only their end points, IE a rectangle is represented by 4 points. 
            findContours(gray, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
	    //remember, countours is not an image, it is a vector which stores every contour(4 points of a rectangle) it sees. 

            vector<Point> approx;

            // test each contour
            for( size_t i = 0; i < contours.size(); i++ )
            {
                // approximate contour with accuracy proportional
                // to the contour perimeter
                approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true);
		//this function takes a contour, and draws it within the arclength? of the countour, true=close polygon

                // square contours should have 4 vertices after approximation
                // they should be ~medium sized, IE have area 100 < area <2000
		//and contour should be convex
                // Note: absolute value of an area is used because
                // area may be positive or negative - in accordance with the
                // contour orientation
                if( approx.size() == 4 &&
                    fabs(contourArea(Mat(approx))) > 100 &&
		    fabs(contourArea(Mat(approx))) < 2000 &&
                    isContourConvex(Mat(approx)) )
                {
                    double maxCosine = 0;

                    for( int j = 2; j < 5; j++ )
                    {
                        // find the maximum cosine of the angle between joint edges
                        double cosine = fabs(angle(approx[j%4], approx[j-2], approx[j-1]));
                        maxCosine = MAX(maxCosine, cosine);
                    }

                    // if cosines of all angles are small
                    // (all angles are ~90 degree) then write quandrange
                    // vertices to resultant sequence
                    //if( maxCosine < 0.3 )
			if( maxCosine < 0.5 )
                        squares.push_back(approx);
                }
            }
        }
    }
}


// the function draws all the squares in the image
static void drawSquares( Mat& image, const vector<vector<Point> >& squares )
{
    for( size_t i = 0; i < squares.size(); i++ )
    {
        const Point* p = &squares[i][0];
        int n = (int)squares[i].size();
        polylines(image, &p, &n, 1, true, Scalar(0,255,0), 3, 16);
    }

    //imshow(wndname, image);
    //imwrite("../savedImages/squareImages.jpg",image);
}


//End Mike Code



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

    cv::Mat img, img_w, img_g, img_gE, img_gD1, img_gD2;
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
                cv::namedWindow("Display Image", cv::WINDOW_NORMAL );
                cv::namedWindow("White Image", cv::WINDOW_NORMAL );
                cv::namedWindow("Green Image", cv::WINDOW_NORMAL );
                cv::namedWindow("Eroded Green Image", cv::WINDOW_NORMAL );
                cv::namedWindow("Dialated1 Green Image", cv::WINDOW_NORMAL );
                cv::namedWindow("Dialated2 Green Image", cv::WINDOW_NORMAL );

                CreateNessImage(img, img_w, computeWhiteness);
                CreateNessImage(img, img_g, computeGreeness);
                dilate( img_g, img_gD1, elementD );
         	erode( img_gD1, img_gE, elementE );
                dilate( img_gE, img_gD2, elementE );
//                findSquares(img, squares);
//                drawSquares(img, squares);

                cv::imshow("Display Image", img);
                cv::imshow("White Image", img_w);
                cv::imshow("Green Image", img_g);
                cv::imshow("Eroded Green Image", img_gE);
                cv::imshow("Dialated1 Green Image", img_gD1);
                cv::imshow("Dialated2 Green Image", img_gD2);


                cout << outdirectory + "SomeQualifier_"  + imgName << endl;
		cv::imwrite(outdirectory + "Original_"  + imgName, img);
                cv::imwrite(outdirectory + "White_"  + imgName, img_w);
                cv::imwrite(outdirectory + "Green_"  + imgName, img_g);
                cv::imwrite(outdirectory + "GreenE_"  + imgName, img_gE);
                cv::imwrite(outdirectory + "GreenD1_"  + imgName, img_gD1);
                cv::imwrite(outdirectory + "GreenD2_"  + imgName, img_gD2);
            
                cv::waitKey(0);//Wait until user presses key to continue
            }
        }
        closedir (dir);
    } else {
        cout << "Could not find dir" << argv[1] << endl;
    }
    

    return 0;
}

