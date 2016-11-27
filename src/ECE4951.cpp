#include <stdio.h>
#include <opencv2/opencv.hpp>
#include<dirent.h>
#include<string.h>

using namespace cv;
using namespace std;

int main(int argc, char** argv )
{
    if ( argc != 2 )
    {
        printf("usage: ECE4951 <Image_Folder_Path>\n");
        return -1;
    }

    Mat img;
    DIR *dir;

    dir = opendir(argv[1]);
    struct dirent *ent;
    string directory(argv[1]);

    if(dir != NULL){

        //Loops Through and loads every image in the directory to img.
        while ((ent = readdir (dir)) != NULL) {
            string imgPath(directory + ent->d_name);
            cout << imgPath << endl;
            img = imread(imgPath);
            if ( !img.data )
            {
                printf("Not a valid image \n");
            } else {
                namedWindow("Display Image", WINDOW_NORMAL );
                imshow("Display Image", img);
            
                waitKey(0);
            }
        }
        closedir (dir);
    } else {
        cout << "Could not find dir" << argv[1] << endl;
    }
    

    return 0;
}

