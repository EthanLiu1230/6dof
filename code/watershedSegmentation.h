#if !defined WATERSHS
#define WATERSHS

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;

class WatershedSegmenter {

  private:

	  cv::Mat markers;

  public:
      Mat createMarkerImage(Mat &imgBinary, int nErode=3, int nDilate=3) {
          Mat fg;
          cv::erode(imgBinary, fg, Mat(), Point(-1,-1), nErode);//之前是4
          Mat bg;
          cv::dilate(imgBinary, bg, Mat(), Point(-1,-1), nDilate);
          cv::threshold(bg, bg, 1, 128, cv::THRESH_BINARY_INV);
          return fg + bg;
      }

	  void setMarkers(const cv::Mat& markerImage) {
		// Convert to image of ints
		markerImage.convertTo(markers,CV_32S);
	  }

	  cv::Mat process(const cv::Mat &image) {

		// Apply watershed
		cv::watershed(image,markers);

		return markers;
	  }

	  // Return result in the form of an image
	  cv::Mat getSegmentation() {
		  
		cv::Mat tmp;
		// all segment with label higher than 255
		// will be assigned value 255
		markers.convertTo(tmp,CV_8U);

		return tmp;
	  }

	  // Return watershed in the form of an image
	  cv::Mat getWatersheds() {
	
		cv::Mat tmp;
		markers.convertTo(tmp,CV_8U,255,255);

		return tmp;
	  }
};


#endif
