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

string to_string(int i);
double distance(int x1, int y1, int x2, int y2);


int main(int argc, char** argv )
{
    //Check Arguments
    if ( argc != 5 )
    {
        printf("usage: ECE4951 <Mode>  <Source> <Output_Folder_Path> <Empty_lot_image>\n");
        printf("Mode: 0 = from file, 1 = from camera\n");
        printf("Source: input device number or directory with source photos\n");
		printf("Output Folder Path: Where Output Images are saved\n");
		printf("Empty Lot Image: Name of Image of Empty Lot to find all squares in empty parking lot\n");

        return -1;
    }


	//CV and Vector Declarations
    cv::Mat img,orig_img; // Incoming image, and original parking lot image
    cv::Point entrance;// point where the simulated entrance is
    std::vector<cv::Point> spots;//All spots found in new image
    std::vector<cv::Point> empty_lot_spots;// all spots of original empty image
    string outdirectory(argv[3]);//directory for output images
   
       
    
	
    if(argv[1][0] == '0'){ // if input 0 for finding images instead of using camera
        DIR *dir;
        dir = opendir(argv[2]); //open input photos directory
        struct dirent *ent;
        string indirectory(argv[2]);
        int empty_lot_scan=0;//Flag indicating first photo, which is an empty parking lot. 
        if(dir != NULL){// if the directory has stuff in it
    
            //Loops Through and loads every image in the directory to img.
            while ((ent = readdir (dir)) != NULL) 
            {
				
				int dist_threshold=15;//distance threshold to compare found spots to empty spots
				double best_distance=-1;//holds distance of best open spot from entrance 
				int best_spot_number=-1;//holds best parking spot number
				
				//assemble the images
				string imgName;
				if(empty_lot_scan==0)//if first image, meaning empty parking lot image. 
				{
					string imgName(argv[4]);// set imageName to predefined input from user
					cout << "Empty Parking Lot Image: " << imgName << endl;
					string imgPath(indirectory + imgName);
					cout << imgPath << endl;
					img = cv::imread(imgPath);
					orig_img = cv::imread(imgPath);
				}else{
					string imgName(ent->d_name);// random image from directory
					cout << imgName << endl;
					string imgPath(indirectory + imgName);
					cout << imgPath << endl;
					img = cv::imread(imgPath);
				}	
					
					
					
					//Checks if there is valid image data
					if ( !img.data )
					{
						printf("Not a valid image \n");
					}
					else 
					{
						cout << "Image is " << img.rows << "x" << img.cols << endl;
						spots = findOpenSpots(img , 0, 0, outdirectory, imgName);//The second and third params show and save steps
						
						
						if(empty_lot_scan==0)//if empty lot, find and save every location of empty lot
						{
							
							empty_lot_spots=spots;//copy spots into empty lot spots
							empty_lot_scan=1;//set empty scan flag
							
							cout << empty_lot_spots.size()  <<" Open Spots Found in Empty Lot" << endl;
							
							for(int i = 0; i < empty_lot_spots.size(); i++){
								string out_str= to_string(i);//turn i into a string to pass into putText function
								putText(orig_img, out_str ,  empty_lot_spots[i], FONT_HERSHEY_SIMPLEX, .6, Scalar(0, 150, 255), 1, 8 , false);//draw spot numbers onto image
								circle(orig_img, empty_lot_spots[i], 5, Scalar(0, 0, 255), -1);// draw circles onto original image
								//cout << "Initial Spot: " << i << " x=" << spots[i].x  << " , y=" << spots[i].y  << "\t";//print intialy found spots
							}
							cout << endl << endl;
														
							cv::namedWindow("Empty Lot Spots", cv::WINDOW_NORMAL );
							cv::imshow("Empty Lot Spots", orig_img);
						}
						else//if not empty lot, need to find available spots
						{
							
							//create random entrance location
							entrance.x=rand() % img.rows;
							entrance.y=rand() % img.cols;
							
							putText(img, "Entrance" ,  entrance, FONT_HERSHEY_SIMPLEX, .4, Scalar(150, 150, 255), 1, 8 , false);//draw spot numbers onto image
							circle(img, entrance, 3, Scalar(150, 255, 25), -1);// draw circles onto original image
							
							best_distance=99999; //initiliaze initial distance to infinty
							best_spot_number=-1;//reset best spot number to detect if none are found = full lot
							cout << spots.size()  <<" Open Spots Found" << endl;
							for(int i = 0; i < spots.size(); i++)
							{
								for( int j=0; j < empty_lot_spots.size(); j++)
								{
									if( (abs(spots[i].x - empty_lot_spots[j].x) < dist_threshold) && (abs(spots[i].y - empty_lot_spots[j].y) < dist_threshold) ) // if spot locaiton is withint threshold of original spot location
									{
										string out_str= to_string(j);//turn j into a string for the putTExt function
										circle(img, spots[i], 5, Scalar(255, 0, 0), -1); // draw spot
										putText(img, out_str , spots[i], FONT_HERSHEY_SIMPLEX, .6, Scalar(0, 150, 255), 1, 8 , false);//draw spot numbers onto image
										
										double calc_dist=distance((int)spots[i].x,(int)spots[i].y, entrance.x, entrance.y);//calculate the distance of spot to the entrance
										
										if( calc_dist < best_distance )// if this spot is closer to the entrance
										{
											best_distance = calc_dist;// save distance
											best_spot_number = j;//update best spot number
										}
										
										//cout << "Found Spot " << i << ": x=" << spots[i].x  << " , y=" << spots[i].y;
										//cout << " Relates to Empty Spot " << j << ": x=" << empty_lot_spots[j].x  << " , y=" << empty_lot_spots[j].y;
										//cout << "  Distance= "<< calc_dist << endl;
									}
								}
							}
							
							if(best_spot_number == -1) // if no spots were found
							{
								cout << "No parking spots were found" <<endl;
							}
							else{
								cout << "The closest parking spot is spot # " << best_spot_number << endl << endl;
								circle(img, empty_lot_spots[best_spot_number], 6, Scalar(0, 0, 255), -1);
								//draw a nice circle aroudn that boi
							}
							
							cv::namedWindow("Marked Spots", cv::WINDOW_NORMAL );
							cv::imshow("Marked Spots", img);
						}
						
						cv::waitKey(0);    
					}
            }
            
            closedir (dir);
        } else { // if directory is empty.
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




std::string to_string(int i)
{
	std::stringstream ss;
	ss << i;
	return ss.str();
}

double distance(int x1, int y1, int x2, int y2)
{
	//cout << "x1: " << x1 << " Y1: " << y1 << " x2: " << x2 << " y2: " << y2 <<endl;
	//cout << "x1-x2 = " << x1-x2 << " squared = << " << pow(x1-x2,2) <<endl; 
	// cout << "I am going to return " << sqrt( pow(x1-x2,2) + pow(y1-y2,2) ) << endl;
	return sqrt( pow(x1-x2,2) + pow(y1-y2,2) );
}

