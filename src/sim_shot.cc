#include <stdio.h>
#include <stdlib.h> 
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "video.h"
#include "commandline.h"
#include <string>


using namespace std;
using namespace cv;

// distance between to vectors
float d_distance(Mat a, Mat b){
	float result;
	Mat M = a.mul(b);
	float S=0.0;
	for (int i = 0; i < M.rows; i++){ 
		S = S + (float)M.at<float>(i,0);
	}
    return S;
	
}
// cosine distance between to vectors
float cosine_distance(Mat A, Mat B){	
	float S1= d_distance(A,B);
	float S2= d_distance(A,A);
	float S3= d_distance(B,B);
	return (1- S1/sqrt(S2*S3));	
	
}


// concatane 3 matrix
Mat concat(Mat *A, Mat *B, Mat *C){	
	int m =A->rows + B->rows + C->rows;
	Mat_<float> R(m,1);
	for (int i = 0; i < A->rows; i++){ 
		R.row(i).col(0) = (float)A->at<float>(i,0);
	}
	for (int i = 0; i < B->rows; i++){ 
		R.row(i+A->rows).col(0) = (float)B->at<float>(i,0);
	}
	for (int i = 0; i < C->rows; i++){ 
		R.row(i+A->rows+ B->rows).col(0) = (float)C->at<float>(i,0);
	}
	return R;
}

// hsv histogram of an image
Mat hsvHist( Mat hsv ){	
  Mat  dst;
  
  /// Separate the image in 3 places ( v, h and s )
  vector<Mat> vsh_planes, rgb_planes;
  split( hsv, vsh_planes );
  
  Mat h_hsv, s_hsv, v_hsv;
  h_hsv = vsh_planes[0];
  s_hsv = vsh_planes[1];
  v_hsv = vsh_planes[2];

  /// Establish the number of bins
  int histSize = 256;
  float range[] = { 0, 256} ;
  
  int histSize_h = 180;
  float range_h[] = { 0, 180} ;
  
  const float* histRange = { range };
  const float* histRange_h = { range_h };

  bool uniform = true; bool accumulate = false;
  Mat h_hist, s_hist, v_hist;  


  /// Compute the histograms:
  calcHist( &vsh_planes[0], 1, 0, Mat(), h_hist, 1, &histSize_h, &histRange_h, uniform, accumulate );
  calcHist( &vsh_planes[1], 1, 0, Mat(), s_hist, 1, &histSize, &histRange, uniform, accumulate );
  calcHist( &vsh_planes[2], 1, 0, Mat(), v_hist, 1, &histSize, &histRange, uniform, accumulate );
  
   
  /// Normalize the results
  normalize(h_hist, h_hist, 0, 180, NORM_MINMAX, -1, Mat() );
  normalize(s_hist, s_hist, 0, 256, NORM_MINMAX, -1, Mat() );
  normalize(v_hist, v_hist, 0, 256, NORM_MINMAX, -1, Mat() );
  
  Mat R=concat(&h_hist,&s_hist,&v_hist);



  // Draw the histograms for H, S and V
/*
  int hist_w = 512; int hist_h = 400;
  int bin_w = cvRound( (double) hist_w/histSize );
   Mat histImage( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );
  	
  /// Draw for each channel
  for( int i = 1; i < histSize; i++ )
  {
      line( histImage, Point( bin_w*(i-1), hist_h - cvRound(v_hist.at<float>(i-1)) ) ,
                       Point( bin_w*(i), hist_h - cvRound(v_hist.at<float>(i)) ),
                       Scalar( 255, 0, 0), 2, 8, 0  );
      line( histImage, Point( bin_w*(i-1), hist_h - cvRound(s_hist.at<float>(i-1)) ) ,
                       Point( bin_w*(i), hist_h - cvRound(s_hist.at<float>(i)) ),
                       Scalar( 0, 255, 0), 2, 8, 0  );
      line( histImage, Point( bin_w*(i-1), hist_h - cvRound(h_hist.at<float>(i-1)) ) ,
                       Point( bin_w*(i), hist_h - cvRound(h_hist.at<float>(i)) ),
                       Scalar( 0, 0, 255), 2, 8, 0  );
  }
  
  imshow("src", src);
  cvWaitKey(0);
  
  imshow("hsv", s_hsv);
  cvWaitKey(0);
  imshow("hsv", v_hsv);
  cvWaitKey(0);
  imshow("hsv", hsv);
  cvWaitKey(0);

  namedWindow("calcHist Demo", CV_WINDOW_AUTOSIZE );
  imshow("calcHist Demo", histImage );
  
  */

return R;
}


/**
 * @function main
 */
int main( int argc, char **argv)
{
	

	// Usages
    amu::CommandLine options(argv, "[options]\n");
    options.AddUsage("  --shots <shot-file>               list of frame shots\n");
    options.AddUsage("  --output                          similarities \n");
   if(options.Size() == 0){ 
		options.Usage();
		}
    std::string file_liste= options.Get<std::string>("--shots", "");
    std::string output= options.Get<std::string>("--output", "");
    

	vector<string> list;
    list.clear();
    
	fstream fichier(file_liste.c_str());
    if ( !fichier ) {           
        cout << "fichier inexistant";
    } else {                    
        bool continuer = true;  
        while( continuer ) {	
            string ch;          
            fichier >> ch;	    

            if ( ch != "" )  {  
                list.push_back(ch);
				
              }else                  
                continuer = false;  
        }
    }

Mat R1, R2;

int n = list.size();
float D=0;
Mat_<float> Similarity(n,n);
Mat img_rgb1, img_hsv1;			
Mat img_rgb2, img_hsv2;			
			
for (int i=0; i <n; i++){
		
		Similarity.row(i).col(i) = 0;
		img_rgb1 = imread(list.at(i),1);
		cvtColor(img_rgb1,img_hsv1,CV_BGR2HSV);
		
		R1 = hsvHist(img_hsv1);
	for (int j=i+1; j <n; j++){
			img_rgb2 = imread(list.at(j),1);
			
			
			cvtColor(img_rgb2,img_hsv2,CV_BGR2HSV);
		
			R2 = hsvHist(img_hsv2);
			
			D = cosine_distance(R1,R2);
			Similarity.row(i).col(j) = D;
			Similarity.row(j).col(i) = D;
	}
}

  ofstream myfile;
  myfile.open (output.c_str());  
  myfile << Similarity;
  myfile.close();
 


  return 0;
} 
