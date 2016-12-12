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


