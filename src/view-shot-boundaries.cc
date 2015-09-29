// View Shot boundary detection results 
// Please read the wiki for information and build instructions.

#include <iostream>
#include <algorithm>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "video.h"


// This function plots 4 frames before and after the specified frame number (or time)
void ShowTiles(amu::VideoReader& video, double frame, bool isTime = false) {
    if(isTime) {
        video.SeekTime(frame);
        if(fabs(frame - video.GetTime()) > 1) {
            std::cerr << "WARNING: Could not seek to time " << frame << "\n";
            return;
        }
        frame = video.GetIndex() - 1;
    }
    if(frame > 3) video.Seek((int) (frame - 4));
    if(fabs(frame - video.GetIndex()) > 25) {
        std::cerr << "WARNING: Could not seek to frame " << frame << "\n";
        return;
    }
    cv::Mat current;
    cv::Size size2 = video.GetSize() ;
    cv::Size size;
    size.height =size2.height/4;
    size.width =size2.width/4;
    cv::Mat image(size * 3, CV_8UC3);
    for(int x = 0; x < 3; x++) {
        for(int y = 0; y < 3; y++) {
            cv::Mat target(image, cv::Range(x * size.height, (x + 1) * size.height), cv::Range(y * size.width, (y + 1) * size.width));
            if(video.ReadFrame(current)) {
                cv::resize(current, target, size);
            } else {
                target.setTo(0);
            }
            if((int) frame == video.GetIndex()) {
                cv::rectangle(image, cv::Rect(y * size.width, x * size.height, size.width, size.height), cv::Scalar(0, 0, 255));
            }
        }
    }
    cv::imshow("shot boundary viewer", image);
    cv::waitKey(0);
    cv::destroyWindow("shot boundary viewer");
}

// Main function: Takes a frame number or time,
// shows the four frames before and after the specifed frame :
int main(int argc, char** argv) {
    std::cerr<<"*********** View shot boundaries ***********"<<std::endl;	
    amu::CommandLine options(argv, "[options]\n");
    // if specified time instead of a frame number
    options.AddUsage("  --time                            specify that shot boundaries are in seconds\n");
    bool isTime = options.IsSet("--time");
    
	//load video
    amu::VideoReader video;
    if(!video.Configure(options)) return 1;
    if(options.Size() != 0) options.Usage();

    std::string line;
	std::cout << "Please enter frame_number or time" << "\n";

    while(std::getline(std::cin, line)) {
		std::cout << "Please enter frame_number or time" << "\n";
        std::stringstream reader(line);
        double frame; double similarity;
        reader >> frame >> similarity;
        // show frames
        ShowTiles(video, frame, isTime);
    }

    return 0;
}
