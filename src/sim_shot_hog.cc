#include <stdio.h>
#include <stdlib.h> 
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "video.h"
#include "commandline.h"
#include <string>
#include <opencv2/imgproc/imgproc.hpp>
#include "shot-features.h"
#include "classify.h"


using namespace std;
using namespace cv;

// distance between to vectors
float d_distance(std::vector<float> a, std::vector<float> b){
	float result;
	//std::vector<float> M = a.mul(b);
	float S=0.0;
	for (int i = 0; i < a.size(); i++){ 
		S = S + a[i]* b[i];
	}
    return S;
	
}
// cosine distance between to vectors
float cosine_distance(std::vector<float> A,std::vector<float> B){	
	float S1= d_distance(A,B);
	float S2= d_distance(A,A);
	float S3= d_distance(B,B);
	return (1- S1/sqrt(S2*S3));	
	
}


int main( int argc, char **argv){
	// Usages
    amu::CommandLine options(argv, "[options]\n");
    options.AddUsage("  --shots <shot-file>               list of frame shots\n");
    options.AddUsage("  --output                          similarities \n");
	if(options.Size() == 0) options.Usage();
	
    std::string file_liste= options.Get<std::string>("--shots", "");
    std::string output= options.Get<std::string>("--output", "");
    

	std::vector<string> list;
    
	fstream fichier(file_liste.c_str());
    if ( !fichier ) std::cerr << "fichier inexistant"<<std::endl;
    else {                        
        bool continuer = true;      
        while( continuer ) {	
            string ch;          
            fichier >> ch;	    
            if ( ch != "" )  list.push_back(ch);
            else   continuer = false;	
		}
	}

	int n = list.size();
	float D=0;
	cv::Mat_<float> Distance(n,n);
	cv::Mat image1, image2;			
	cv::HOGDescriptor hog(cv::Size(128, 64), cv::Size(16,16), cv::Size(8,8), cv::Size(8,8), 9);
	amu::FeatureExtractor extractor;		
	for (int i=0; i <n; i++){
			
			Distance.row(i).col(i) = 0;
			image1 = cv::imread(list.at(i), CV_LOAD_IMAGE_COLOR); 
			std::vector<float> features1 = extractor.Compute(image1);
			
			for (int j=i+1; j <n; j++){
				image2 = cv::imread(list.at(j), CV_LOAD_IMAGE_COLOR); 
				std::vector<float> features2 = extractor.Compute(image2);
				D = cosine_distance(features1,features2);
				Distance.row(i).col(j) = D;
				Distance.row(j).col(i) = D;
			}
	}

	 std::vector<float> l1,l2;
	 l1.push_back(1);
	 l1.push_back(1);
	 l2.push_back(0);
	 l2.push_back(1);
	 
	 
	 std::cout<<cosine_distance(l1,l1) <<std::endl;
	 
	  ofstream myfile;
	  myfile.open (output.c_str());  
	  myfile << Distance;
	  myfile.close();
	  return 0;
} 
