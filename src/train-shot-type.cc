// Please read the wiki for information and build instructions.
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "shot-features.h"
#include "classify.h"
#include <iostream>
#include <string>
#include <fstream>



namespace amu {
    struct Data {
        std::string name;
        int label;
        static std::vector<Data> Load(const std::string& filename) {
            std::vector<Data> output;
            std::ifstream input(filename.c_str());
            if(input) {
                std::string line;
                while(std::getline(input, line)) {
                    std::stringstream reader(line);
					Data detection;
                    reader >> detection.name >> detection.label;
                    output.push_back(detection);
                }
            } else {
                std::cerr << "ERROR: loading \"" << filename << "\"\n";
            }
            return output;
        }
        
	};
}



int main(int argc, char** argv) {
    amu::CommandLine options(argv, "[options]\n");
    options.AddUsage("  --data \n");
    std::string dataFilename = options.Get<std::string>("--data", "");

    std::vector<amu::Data> data = amu::Data::Load(dataFilename);
    cv::Mat image;
    cv::HOGDescriptor hog(cv::Size(128, 64), cv::Size(16,16), cv::Size(8,8), cv::Size(8,8), 9);

    amu::FeatureExtractor extractor;	
	for (int i =0; i<data.size();i++){
				image = cv::imread(data[i].name, CV_LOAD_IMAGE_COLOR); 
				std::vector<float> features = extractor.Compute(image);
                std::cout << data[i].label;
                for(size_t j = 0; j < features.size(); j++) {
                    std::cout << " "<<j+1<<":" << features[j];
                }
                std::cout << "\n";
	
	}
    return 0;
}
