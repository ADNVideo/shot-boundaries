// Please read the wiki for information and build instructions.

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "shot-features.h"
#include "classify.h"


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
    options.AddUsage("  --data                                     \n");
    options.AddUsage("  --model                           SVM model\n");

    std::string dataFilename = options.Get<std::string>("--data", "");
    std::string modelFilename = options.Get<std::string>("--model", "");



    amu::LibLinearClassifier classifier(modelFilename);	
	std::vector<amu::Data> data = amu::Data::Load(dataFilename);
    cv::Mat image;

    amu::FeatureExtractor extractor;	
	for (int i =0; i<data.size();i++){
				image = cv::imread(data[i].name, CV_LOAD_IMAGE_COLOR); 
				std::vector<float> features = extractor.Compute(image);
                std::cout << data[i].label <<" "<<classifier.Classify(features) << "\n";
	}
		
    return 0;
}
