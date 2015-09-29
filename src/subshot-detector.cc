// Detect shotshots (splits) in videos or list of images without split templates.
// Please read the wiki for information and build instructions.
#include <opencv2/opencv.hpp>
#include <iostream>
#include "nbest.h"
#include "repere.h"

inline int column_integral(const cv::Mat& sum, const int x, const int y) {
    return sum.at<int>(y, x + 1) - sum.at<int>(y, x);
}

inline int row_integral(const cv::Mat& sum, const int x, const int y) {
    return sum.at<int>(y + 1, x) - sum.at<int>(y, x);
}

double min(double x1, double x2) {
    return x1 < x2 ? x1 : x2;
}

double min_ratio(double x1, double x2, double x3, double x4) {
    return min(x1, min(x2, min(x3, x4)));
}

double FindRectangles(cv::Mat& hEdges, cv::Mat& vEdges, cv::Mat& image) {
    cv::Mat hSum, vSum;
    int width = image.cols;
    int height = image.rows;
    cv::integral(hEdges, hSum);
    cv::integral(vEdges, vSum);

    amu::StdNBest vertical(10);
    for(int x = 0; x < width; x++) {
        vertical.insertNmax(column_integral(vSum, x, height - 1), x);
    }
    vertical.insertNmax(height, 0);
    vertical.insertNmax(height, width - 1);
    vertical.sortNmax();

    amu::StdNBest horizontal(10);
    for(int y = 0; y < height; y++) {
        horizontal.insertNmax(row_integral(hSum, width - 1, y), y);
    }
    horizontal.insertNmax(width, 0);
    horizontal.insertNmax(width, height - 1);
    horizontal.sortNmax();


    std::vector<int> sorted_vertical;
    for(int i = 0; i < vertical.size(); i++) sorted_vertical.push_back(vertical.get(i));
    std::sort(sorted_vertical.begin(), sorted_vertical.end());

    std::vector<int> sorted_horizontal;
    for(int i = 0; i < horizontal.size(); i++) sorted_horizontal.push_back(horizontal.get(i));
    std::sort(sorted_horizontal.begin(), sorted_horizontal.end());

    for(size_t i = 0; i < sorted_vertical.size(); i++) std::cout << sorted_vertical[i] << " "; std::cout << "\n";
    for(size_t i = 0; i < sorted_horizontal.size(); i++) std::cout << sorted_horizontal[i] << " "; std::cout << "\n";

    double max = 0;
    cv::Rect argmax;
    int numRects = 0;
    for(size_t i = 0; i < sorted_vertical.size() - 1; i++) {
        for(size_t j = i + 1; j < sorted_vertical.size(); j++) {
            for(size_t k = 0; k < sorted_horizontal.size() - 1; k++) {
                for(size_t l = k + 1; l < sorted_horizontal.size(); l++) {
                    int x1 = sorted_vertical[i], x2 = sorted_vertical[j];
                    int y1 = sorted_horizontal[k], y2 = sorted_horizontal[l];
                    int perimeter = 2 * (x2 - x1) + 2 * (y2 - y1);

                    int border1 = row_integral(hSum, x2, y1) - row_integral(hSum, x1, y1);
                    int border2 = row_integral(hSum, x2, y2) - row_integral(hSum, x1, y2);
                    int border3 = column_integral(vSum, x1, y2) - column_integral(vSum, x1, y1);
                    int border4 = column_integral(vSum, x2, y2) - column_integral(vSum, x2, y1);

                    double ratio1 = (double) border1 / (256.0 * (x2 - x1));
                    double ratio2 = (double) border2 / (256.0 * (x2 - x1));
                    double ratio3 = (double) border3 / (256.0 * (y2 - y1));
                    double ratio4 = (double) border4 / (256.0 * (y2 - y1));

                    double ratio = (ratio1 + ratio2 + ratio3 + ratio4) / 4;

                    int area = (x2 - x1) * (y2 - y1);
                        cv::rectangle(image, cv::Rect(x1, y1, x2 - x1 + 1, y2 - y1 + 1), cv::Scalar(0, 255, 0), 1);
                    if(ratio > 0.5 /*&& area > (width / 10) * (height / 10)*/ && min_ratio(ratio1, ratio2, ratio3, ratio4) > 0.5) {
                        numRects++;
                        cv::rectangle(image, cv::Rect(x1, y1, x2 - x1 + 1, y2 - y1 + 1), cv::Scalar(0, 255, 0), 1);
                        //std::cout << border1 / 256.0 << " " << border2 / 256.0 << " " << border3 / 256.0 << " " << border4 / 256.0 << "\n";
                        std::cout << ratio << " " << ratio1 << " "<< ratio2 << " "<< ratio3 << " "<< ratio4 << " "    << cv::Rect(x1, y1, x2 - x1, y2 - y1) << "\n";

                        if(ratio > max) {
                            max = ratio;
                            argmax = cv::Rect(x1, y1, x2 - x1 + 1, y2 - y1 + 1);
                        }
                    }
                }
            }
        }
    }
    std::cout << numRects << " " << max << " " << argmax << "\n";

    cv::imshow("image", image);
    cv::waitKey(0);
    return 0;
}

void DrawRectangles(cv::Mat& image) {
    cv::Mat hsv;
    cv::cvtColor(image, hsv, CV_BGR2HSV);
    std::vector<cv::Mat> channels;
    cv::split(hsv, channels);
    cv::Mat horizontal(image.rows, image.cols, CV_16S);
    cv::Mat vertical(image.rows, image.cols, CV_16S);
    horizontal.setTo(0);
    vertical.setTo(0);

    cv::Mat gray;
    cv::cvtColor(image, gray, CV_BGR2GRAY);
    cv::Sobel(gray, vertical, CV_16S, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT );
    cv::Sobel(gray, horizontal, CV_16S, 0, 1, 3, 1, 0, cv::BORDER_DEFAULT );
    horizontal = cv::abs(horizontal);
    vertical = cv::abs(vertical);
    cv::threshold(horizontal, horizontal, 192, 255, CV_THRESH_BINARY);
    cv::threshold(vertical, vertical, 192, 255, CV_THRESH_BINARY);
	

    
    int width = image.cols;
    int height = image.rows;
    cv::Mat hSum, vSum;
    horizontal.convertTo(horizontal, CV_8U);
    vertical.convertTo(vertical, CV_8U);


    cv::vector<cv::Vec4i> lines;
    // input, output, rho, theta, threshold, min-length, max-gap
      cv::HoughLinesP(horizontal, lines, 1, CV_PI/2, 50, 50, 4);
      for( size_t i = 0; i < lines.size(); i++ )
      {	 
      cv::Vec4i l = lines[i];
      cv::line(image, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0,0,255), 1, CV_AA);
      }


	  cv::vector<cv::Vec4i> lines2;
      cv::HoughLinesP(vertical, lines2, 1, CV_PI, 50, 50, 4);

      for( size_t i = 0; i < lines2.size(); i++ )
      {
      cv::Vec4i l = lines2[i];
      cv::line(image, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0,0,255), 1, CV_AA);
      }
      cv::imshow("image", image);

	cv::waitKey(0);
		FindRectangles(horizontal, vertical, image) ;

}

int main(int argc, char** argv) {

    amu::CommandLine options(argv, "[options]\n");

    amu::VideoReader video;
    if(!video.Configure(options)) {
        return 1;
    }

    if(options.Size() != 0) options.Usage();

    cv::Mat image;

    while(video.HasNext()) {
        video.ReadFrame(image);
        DrawRectangles(image);
    }
}
