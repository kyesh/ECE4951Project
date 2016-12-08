#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include<string.h>


std::vector<cv::Point> findOpenSpots(cv::Mat& img , int showPhotos, int savePhotos, std::string outdirectory, std::string imgName){

                cv::Mat img_w, img_g, img_gE, img_gD1 ;
                std::vector<cv::Point>  spots; //Create list of squares
                std::vector<std::vector<cv::Point> > contours;
                std::vector<cv::Vec4i> hier;           
 
                //Element for Erosion and Dialation Operator
                int erosion_size = 40;
                cv::Mat elementE = cv::getStructuringElement( cv::MORPH_RECT,
                                                   cv::Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                                   cv::Point( erosion_size, erosion_size ) );
                erosion_size = 5;
                cv::Mat elementD = cv::getStructuringElement( cv::MORPH_RECT,
                                                   cv::Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                                   cv::Point( erosion_size, erosion_size ) );
            



                CreateNessImage(img, img_w, computeWhiteness);
                CreateNessImage(img, img_g, computeGreeness);
                dilate( img_g, img_gD1, elementD );
         	erode( img_gD1, img_gE, elementE );
                findContours(img_gE.clone(), contours, hier, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
                
                //drawContours(img, contours, -1, Scalar( 0, 0, 255) );
                
                cv::Moments M;
                for(int i = 0; i < contours.size(); i++){

		    M = moments(contours[i]);
                    spots.push_back(cv::Point(int(M.m10/M.m00),int(M.m01/M.m00)));
                    //circle(img, Point(int(M.m10/M.m00),int(M.m01/M.m00)), 25, Scalar(255, 0, 0), -1);

                }


                if( showPhotos > 0){
                    cv::namedWindow("Display Image", cv::WINDOW_NORMAL );
                    cv::namedWindow("White Image", cv::WINDOW_NORMAL );
                    cv::namedWindow("Green Image", cv::WINDOW_NORMAL );
                    cv::namedWindow("Eroded Green Image", cv::WINDOW_NORMAL );
                    cv::namedWindow("Dialated1 Green Image", cv::WINDOW_NORMAL );
    
                    cv::imshow("Display Image", img);
                    cv::imshow("White Image", img_w);
                    cv::imshow("Green Image", img_g);
                    cv::imshow("Eroded Green Image", img_gE);
                    cv::imshow("Dialated1 Green Image", img_gD1);
                    
                    cv::waitKey(0);//Wait until user presses key to continue
                }

                if (savePhotos > 0){
                    std::cout << outdirectory + "SomeQualifier_"  + imgName << std::endl;
                    cv::imwrite(outdirectory + "Original_"  + imgName, img);
                    cv::imwrite(outdirectory + "White_"  + imgName, img_w);
                    cv::imwrite(outdirectory + "Green_"  + imgName, img_g);
                    cv::imwrite(outdirectory + "GreenE_"  + imgName, img_gE);
                    cv::imwrite(outdirectory + "GreenD1_"  + imgName, img_gD1);
                }

return spots;


}
