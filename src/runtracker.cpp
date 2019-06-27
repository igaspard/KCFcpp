#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#include <opencv2/opencv.hpp>

#include "kcftracker.hpp"

#include <dirent.h>

using namespace std;
using namespace cv;

bool init_success = false;
void Callback(int event, int x, int y, int flags, void* userdata) {
    static int count = 0;
    if(count == 2) {
        if(!init_success)
            init_success = true;
        return;
    }

    if(event == EVENT_LBUTTONDOWN) {
        Rect& data = *(Rect*)(userdata);
        if(count == 0) {
            data.x = x;
            data.y = y;
        }
        else if(count == 1) {
            data.width = x - data.x;
            data.height = y - data.y;
        }
        ++count;
    }
}

int main(int argc, char* argv[]){
    // cout<< "argc: " << argc << endl;
	// if (argc > 7) return -1;
	// if (argc > 5) return -1;

	bool HOG = true;
	bool FIXEDWINDOW = false;
	bool MULTISCALE = true;
	bool LAB = false;

	// for(int i = 0; i < argc; i++){
	// 	if ( strcmp (argv[i], "hog") == 0 )
	// 		HOG = true;
	// 	if ( strcmp (argv[i], "fixed_window") == 0 )
	// 		FIXEDWINDOW = true;
	// 	if ( strcmp (argv[i], "singlescale") == 0 )
	// 		MULTISCALE = false;
	// 	if ( strcmp (argv[i], "lab") == 0 ){
	// 		LAB = true;
	// 		HOG = true;
	// 	}
	// 	if ( strcmp (argv[i], "gray") == 0 )
	// 		HOG = false;
	// }
	
	// Create KCFTracker object
	KCFTracker tracker(HOG, FIXEDWINDOW, MULTISCALE, LAB);

	// Frame readed
	Mat frame;

	// Tracker results
	Rect result;

	// Frame counter
	int nFrames = 0;

    VideoCapture cap;
    if (argc == 2) {
        cap.open(argv[1]);
    }
    else {
        cap.open(0);
    }
    if(!cap.isOpened())
        return -1;

    namedWindow("Image");
    Rect init_rect;
    // setMouseCallback("Image", Callback, &init_rect);
    init_rect.x = 428;
    init_rect.y = 272;
    init_rect.width = 209;
    init_rect.height = 273;
    // cout << "init_rect: (" << init_rect.x << ", " << init_rect.y
    // << "), " << init_rect.width << ", " << init_rect.height << endl;
    
    Size videoSize = Size((int)cap.get(CV_CAP_PROP_FRAME_WIDTH),(int)cap.get(CV_CAP_PROP_FRAME_HEIGHT));
    VideoWriter writer;
    writer.open("output.mp4", cap.get(CV_CAP_PROP_FOURCC), cap.get(CV_CAP_PROP_FPS), videoSize);
    init_success = true;
    while (1) {
        cap >> frame;
        if (frame.empty()) {
			cout << "Frame empty, exit...\n";
			break;
		}

        if(init_success) {
            cout << "traker init" << endl;
            tracker.init(init_rect,frame);
            rectangle(frame, init_rect, Scalar( 0, 255, 255 ), 1, 8 );
            ++nFrames;
            init_success = false;
        }
        else if(nFrames > 0) {
            cout << "traker update" << endl;
            result = tracker.update(frame);
            rectangle( frame, Point( result.x, result.y ), Point( result.x+result.width, result.y+result.height), Scalar( 0, 255, 255 ), 1, 8 );
        }

//		// Read each frame from the list
//		frame = imread(frameName, CV_LOAD_IMAGE_COLOR);

//		// First frame, give the groundtruth to the tracker
//		if (nFrames == 0) {
//			tracker.init( Rect(xMin, yMin, width, height), frame );
//			rectangle( frame, Point( xMin, yMin ), Point( xMin+width, yMin+height), Scalar( 0, 255, 255 ), 1, 8 );
//			resultsFile << xMin << "," << yMin << "," << width << "," << height << endl;
//		}
//		// Update
//		else{
//			result = tracker.update(frame);
//			rectangle( frame, Point( result.x, result.y ), Point( result.x+result.width, result.y+result.height), Scalar( 0, 255, 255 ), 1, 8 );
//			resultsFile << result.x << "," << result.y << "," << result.width << "," << result.height << endl;
//		}

//		nFrames++;
        writer << frame;
        // imshow("Image", frame);
        // waitKey(1);
	}

    return 0;
}
