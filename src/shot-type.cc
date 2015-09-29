// Please read the wiki for information and build instructions.

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "shot-features.h"
#include "classify.h"

int main(int argc, char** argv) {
    amu::CommandLine options(argv, "[options]\n");
    options.AddUsage("  --shots <shots-file>              shot segmentation (exclusive with --annotations)\n");

    std::string shotFile = options.Get<std::string>("--shots", "");

    amu::VideoReader video;
    if(!video.Configure(options)) return 1;
    //if(options.Size() != 0 || (shotFile != "")) options.Usage();
	
	
    std::map<int, amu::ShotType> shotTypes;
    if(shotFile != "") {
        std::vector<amu::ShotSegment> shots = amu::ShotSegment::Read(shotFile);
        for(size_t i = 0; i < shots.size(); i++) {
            shotTypes[shots[i].frame] = amu::ShotType(video.GetShowName(), shots[i].frame, amu::ShotLabel_Set);
        }
    }

    amu::FeatureExtractor extractor;
    amu::LibLinearClassifier classifier("/home/meriem/work/repere/data/All.model");
	std::vector<std::string> classes;
	classes.push_back("set");
	classes.push_back("report");
	classes.push_back("mixed");
	classes.push_back("other");
	
    cv::Mat image,resized;
	for(std::map<int, amu::ShotType>::const_iterator shot = shotTypes.begin(); shot != shotTypes.end(); shot++) {
        video.Seek(shot->first);
        if(!video.ReadFrame(image) || image.empty()) {
            std::cerr << "ERROR: reading frame " << video.GetIndex() << "\n";
            continue;
        }
		cv::resize(image,resized,cv::Size(1024,576));
		
        std::vector<float> features = extractor.Compute(resized);
		std::cout<<shot->first <<" "<<classifier.Classify(features) << "\n";
		cv::imshow("original", resized);
		cv::waitKey(0);
		cv::destroyWindow("original");
						
						
						
        //std::cout << shot->second.label;
        //for(size_t i = 0; i < features.size(); i++) {
        //    std::cout << " " << i + 1 << ":" << features[i];
        //}
        //std::cout << "\n";
    }
    return 0;
}
