#include <stdio.h>
#include <opencv2/opencv.hpp>
#include<dirent.h>
#include<string.h>

using namespace cv;
using namespace std;

int main(int argc, char** argv )
{
    if ( argc != 3 )
    {
        printf("usage: ECE4951 <Image_Folder_Path> <Output_Folder_Path>\n");
        return -1;
    }

    Mat img;
    DIR *dir;

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
            img = imread(imgPath);
            //Checks if there is valid image data
            if ( !img.data )
            {
                printf("Not a valid image \n");
            } else {
                string outputPath(outdirectory + "SomeQualifier_"  + imgName);
                namedWindow("Display Image", WINDOW_NORMAL );
                imshow("Display Image", img);
                cout << outputPath << endl;
		imwrite(outputPath, img);            
                waitKey(0);//Wait until user presses key to continue
            }
        }
        closedir (dir);
    } else {
        cout << "Could not find dir" << argv[1] << endl;
    }
    

    return 0;
}

