// Detect shotshots (splits) templates in videos or list of images.
// Please read the wiki for information and build instructions.

#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include "repere.h"

namespace amu {
	
// This function returns the cumulate gradient of sobel filters 
    int CumulateGradient(const cv::Mat& image, cv::Mat& horizontal, cv::Mat& vertical) {
        cv::Mat gray, dx, dy;
        cv::cvtColor(image, gray, CV_BGR2GRAY);
        cv::Sobel(gray, dx, CV_16S, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT );
        cv::Sobel(gray, dy, CV_16S, 0, 1, 3, 1, 0, cv::BORDER_DEFAULT );
        horizontal += dx;
        vertical += dy;
        return 1;
    }
    
// This function plots matched templates with horizontal and vertical gradients
    int MatchTemplate(cv::Mat& image, cv::Mat& horizontal, cv::Mat& vertical, std::vector<amu::Split>& templates) {
		   
        horizontal = cv::abs(horizontal);
        vertical = cv::abs(vertical);
        
        horizontal.convertTo(horizontal, CV_8U);
        vertical.convertTo(vertical, CV_8U);

        for(int i = 0; i < templates.size(); i++) {
            int argmax = -1;
            double max = 0;
            for(int iteration = 0; iteration < 100; iteration++) {
                const amu::Split& split = templates[i];
                double sum = 0; // sum of gradients over rectangles
                double norm = 0;
                double min_segment = 1; // segment with minimal gradient
                for(int j = 0; j < split.subshots.size(); j++) {
                    amu::SubShot subshot = split.subshots[j];

                    // randomily move bounds
                    subshot.x += rand() % 3 - 1;
                    subshot.y += rand() % 3 - 1;
                    subshot.width += rand() % 3 - 1;
                    subshot.height += rand() % 3 - 1;

                    double sum1 = 0, sum2 = 0, sum3 = 0, sum4 = 0;
                    for(int x = subshot.x; x < subshot.x + subshot.width; x++) {
                        sum += vertical.at<uchar>(subshot.y, x);
                        sum += vertical.at<uchar>(subshot.y + subshot.height, x);
                        sum1 += vertical.at<uchar>(subshot.y, x);
                        sum -= horizontal.at<uchar>(subshot.y, x);
                        sum -= horizontal.at<uchar>(subshot.y + subshot.height, x);
                        sum1 -= horizontal.at<uchar>(subshot.y, x);
                        sum2 -= horizontal.at<uchar>(subshot.y + subshot.height, x);
                    }
                    for(int y = subshot.y; y < subshot.y + subshot.height; y++) {
                        sum += horizontal.at<uchar>(y, subshot.x);
                        sum += horizontal.at<uchar>(y, subshot.x + subshot.width);
                        sum3 += horizontal.at<uchar>(y, subshot.x);
                        sum4 += horizontal.at<uchar>(y, subshot.x + subshot.width);
                        sum -= vertical.at<uchar>(y, subshot.x);
                        sum -= vertical.at<uchar>(y, subshot.x + subshot.width);
                        sum3 -= vertical.at<uchar>(y, subshot.x);
                        sum4 -= vertical.at<uchar>(y, subshot.x + subshot.width);
                    }
                    norm += subshot.width * 2 + subshot.height * 2;
                    sum1 /= 256 * subshot.width;
                    sum2 /= 256 * subshot.width;
                    sum3 /= 256 * subshot.height;
                    sum4 /= 256 * subshot.height;
                    if(sum1 < min_segment) min_segment = sum1;
                    if(sum2 < min_segment) min_segment = sum2;
                    if(sum3 < min_segment) min_segment = sum3;
                    if(sum4 < min_segment) min_segment = sum4;
                }
                sum /= 256 * norm;
                if(sum > max) { // && min_segment > 0.1) {
                    max = sum;
                    argmax = i;
                }
            }
            
            if(argmax != -1 && max >= 0.45) {
				
				std::cout << " split_id = "<<argmax << " split_score = "<< max<<std::endl;
				// idea: display masked horizontal and vertical
				// use vertical as penalty on horizontals and vice versa
				for(int i = 0; i < templates[argmax].subshots.size(); i++) {
					const amu::SubShot subshot = templates[argmax].subshots[i];
					std::stringstream name;
					name << i;
					cv::rectangle(image, cv::Rect(subshot.x, subshot.y, subshot.width, subshot.height), cv::Scalar(0, 0, 255), 2);
					cv::imshow("splits", image);
					cv::waitKey(0);
				}
			}
        }
        std::cout << "\n";
        return 0;
    }
}

// Main function: Takes the shot boundaries detections and shows template split candidates
// templates : text file of split templates for 1024x576 frames
//             split_name number_split coor_x coor_y width height coor_x coor_y width height ... coor_x coor_y width hight
// prints numframe middle shot 
// if split candidates then prints split_ids and split_scores
int main(int argc, char** argv) {

    std::cerr<<"*********** Templates split detection ***********"<<std::endl;	
	// need split template file and shot boudaries detection outputs	
    amu::CommandLine options(argv, "[options]\n");
    options.AddUsage("  --templates <template-file>       subshot template definition file\n");
    options.AddUsage("  --shots <shots-file>              shot segmentation\n");

	//scale split templates coordinates 
    double scale = options.Read("--scale", 1.0); // from video.Configure()
    std::string templateFile = options.Get<std::string>("--templates", "");
    std::string shotFile = options.Get<std::string>("--shots", "");
	
	//load video
    amu::VideoReader video;
    if(!video.Configure(options)) return 1;
    if(options.Size() != 0 || templateFile == "" || shotFile == "") options.Usage();
    
	// load templates
    std::fstream templateStream(templateFile.c_str());
    if(!templateStream) {
        std::cerr << "ERROR: cannot read template file \"" << templateFile << "\"\n";
        return 1;
    }
    std::vector<amu::Split> templates;
    amu::Split::ParseTemplates(templateStream, templates, scale);
    
	// load shot boundaries file
    std::fstream shotStream(shotFile.c_str());
    if(!shotStream) {
        std::cerr << "ERROR: cannot read shots file \"" << shotFile << "\"\n";
        return 1;
    }
    std::vector<amu::ShotSegment> shots;
    amu::ShotSegment::ParseShotSegmentation(shotStream, shots);


    cv::Mat image;
    cv::Mat resized;
            
    for(size_t shot = 0; shot < shots.size(); shot++) {
        video.Seek(shots[shot].frame);
        video.ReadFrame(image);
        // resize image to 1024x576
        cv::resize(image, resized,cv::Size(1024,576));
            
        cv::Mat horizontal = cv::Mat::zeros(resized.rows, resized.cols, CV_16S);
        cv::Mat vertical = cv::Mat::zeros(resized.rows, resized.cols, CV_16S);
        cv::Mat frame;
        video.SeekTime(shots[shot].startTime);
        int numFrames = 0;

        amu::CumulateGradient(resized, horizontal, vertical);
        numFrames++;
     
        horizontal /= numFrames;
        vertical /= numFrames;

        std::cout << "shot_id = "<<shot <<"   Middle_frame = "<<shots[shot].frame;// << " ";
        amu::MatchTemplate(resized, horizontal, vertical, templates);
    }
    return 0;
}
