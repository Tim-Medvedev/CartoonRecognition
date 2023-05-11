#include "cartoonRecognition.h"



//* Unused *
// Function that reduces a pixels color value to one of three fields in between 0 and 255
// Preconditions : Passed a valid integer
// Postconditions : 
//int reduceColor(const int val) {
//	if (val < 64) {
//		return 0;
//	}
//	else if (val > 64 && val < 128) {
//		return 64;
//	}
//	else if (val > 128) {
//		return 255;
//	}
//}

// This function creates a decimal value based on the average differences between r g and b in every pixel
// This value for every pixel is then added up and divided up by the total number of pixels in the image which is 1024 x 1024 after resizing
// This is an attempt to measure how "colorful" an image is on average
// As cartoons have more base shades of colors, they will show a higher average difference between red, green, and blue
// So they will have a higher average value returned
// Preconditions : A valid 1024 x 1024 image is passed in
// Postconditions : A non-negative float is returned that is greater than 0

float howColorful(Mat image) {
	float colorVal = 0.0;
	for (int r = 0; r < image.rows; r++) {
		for (int c = 0; c < image.cols; c++) {
			//saves color from pixel and seperates it into seperate color channels
			Vec3b color = image.at<Vec3b>(r, c);
			int blue = color[0];
			int green = color[1];
			int red = color[2];
			//Measure difference between red and green
			float rg = abs(red - green);
			float rgb = abs((red + green) - blue);
			//add it to the counter of all the pixels
			colorVal = colorVal + rg + rgb;
		}
	}

	//Calculate average by dividing by total number of pixels
	colorVal = colorVal / (1024 * 1024);
	return colorVal;
}


// * ONLY USED FOR OBSERVATION *
// This method attempts to measure the int number of edges in an image using canny edge detection
// Preconditions : A valid image Mat object is passed in
// Postconditions : A non-negative int representing the edgecount of an image is returned

int cartoonEdges(Mat image) {
	//First we can attempt simply measuring the number of edges in the image
	Mat blurResult = image.clone();
	//bilateralFilter(edgeResult, edgeResult, 6, 250, 250, BORDER_DEFAULT);
	Mat grey = blurResult.clone();
	cvtColor(grey, grey, COLOR_BGR2GRAY);

	bilateralFilter(image, grey, 6, 250, 250);
	Mat edges;
	Canny(grey, edges, 100, 200);

	int edgeCount = countNonZero(edges);
	return edgeCount;
}


//This method calculates and compares color histograms for the original given image, and the image after blurring
//This is done in an attempt to measure the gradient changes in an image. Reasoning being that a cartoon would have a lower 
//difference in colors after blurring as the original gradients are larger.
//Preconditions : A valid 1024 x 1024 image is passed in, as well as a size value that determines the number of bins in each histogram
//Postconditons : A double is returned representing the average difference in colors after blurring.

double cartoonColors(Mat image, int size) {
	Mat blurColor = image.clone();
	//Mat lessColor = image.clone();
	
	//Bilateral filtering is used to blur the image slightly
	bilateralFilter(image, blurColor, 6, 250, 250);
	//GaussianBlur(blurColor, blurColor, Size(7, 7), 2.0, 2.0);

	//bucket size calculation
	int bucketSize = 256 / size;
	int dims[] = { size, size, size };
	//histogram size definition and cloning for blurred image histogram
	Mat hist(3, dims, CV_32S, Scalar::all(0));
	Mat blurHist = hist.clone();

	// calculates histogram for original image
	for (int r = 0; r < image.rows; r++) {
		for (int c = 0; c < image.cols; c++) {
			//saves color from pixel and seperates it into seperate color channels
			Vec3b color = image.at<Vec3b>(r, c);
			int blue = color[0];
			int green = color[1];
			int red = color[2];
			int r = red / bucketSize;
			int g = green / bucketSize;
			int b = blue / bucketSize;
			//"votes" for the pixel color in the correct histogram bucket
			hist.at<int>(r, g, b)++;
		}
	}

	//Calculates color histogram for blurred image
	for (int r = 0; r < blurColor.rows; r++) {
		for (int c = 0; c < blurColor.cols; c++) {
			//saves color from pixel and seperates it into seperate color channels
			Vec3b color = blurColor.at<Vec3b>(r, c);
			int blue = color[0];
			int green = color[1];
			int red = color[2];
			int r = red / bucketSize;
			int g = green / bucketSize;
			int b = blue / bucketSize;
			//"votes" for the pixel color in the correct histogram bucket
			blurHist.at<int>(r, g, b)++;
		}
	}

	//double that will store the average differences between the two images
	double difference = 0.0;
	
	//Loops through every bin in the histograms
	for (int b = 0; b < size; b++) {
		for (int g = 0; g < size; g++) {
			for (int r = 0; r < size; r++) {
				//If the bin in the original and blurred images have different numbers
				//Then the percentage difference is calculated between the values
				//and added up to the rolling total.
				if (hist.at<int>(r, g, b) != blurHist.at<int>(r, g, b)) {
					double result = 0.0;
					int var1 = hist.at<int>(r, g, b);
					int var2 = blurHist.at<int>(r, g, b);
					if (var1 > var2) {
						result = (double)var2 / (double)var1;
					} else {
						result = (double)var1 / (double)var2;
					}
					difference = difference + result;
				//If the bin in the original and blurred image have the same number
				//of votes then a 1 is added
				} else {
					difference = difference + 1.0;
				}
			}
		}
	}
		//Divides rolling total by total number of bins for easier calculation and returns
		double result = difference / (pow(size, 3));
		return result;
}
	

// * UNUSED *
// Cartoon face detection using code from the OpenCV HaarCascade tutorial
//Mat cartoonFaceDetection(Mat image) {
//	CascadeClassifier face_cascade;
//	CascadeClassifier eyes_cascade;
//	Mat frame_gray;
//	cvtColor(image, frame_gray, COLOR_BGR2GRAY);
//	equalizeHist(frame_gray, frame_gray);
//	vector<Rect> faces;
//	face_cascade.detectMultiScale(frame_gray, faces);
//	for (size_t i = 0; i < faces.size(); i++)
//	{
//		Point center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);
//		ellipse(image, center, Size(faces[i].width / 2, faces[i].height / 2), 0, 0, 360, Scalar(255, 0, 255), 4);
//		Mat faceROI = frame_gray(faces[i]);
//		//-- In each face, detect eyes
//		std::vector<Rect> eyes;
//		eyes_cascade.detectMultiScale(faceROI, eyes);
//		for (size_t j = 0; j < eyes.size(); j++)
//		{
//			Point eye_center(faces[i].x + eyes[j].x + eyes[j].width / 2, faces[i].y + eyes[j].y + eyes[j].height / 2);
//			int radius = cvRound((eyes[j].width + eyes[j].height) * 0.25);
//			circle(image, eye_center, radius, Scalar(255, 0, 0), 4);
//		}
//	}
//	return image;
//}


// * UNUSED *
// Template matching using OpenCV template matching documentation for cv::matchTemplate();
//void templateMatchEye(Mat image) {
//	Mat animeTemplate = imread("animeEye.jpg");
//	Mat southParkTemplate = imread("southParkEye.jpg");
//	matchTemplate(image, animeTemplate, image, TM_SQDIFF);
//	/*rectangle(image, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(0), 2, 8, 0);
//	minMaxLoc()*/
//}


// This method returns an integer measuring how likely an image is a cartoon based on the given threshold values
// Preconditions : A valid image must be passed in, as well as valid doubles representing the thresholds at which
// an Image is decided to be a cartoon or not
// Postconditions : An integer is returned representing how likely an image is a cartoon, with the minimum being 0 and the maximum being 2

int isCartoon(Mat image, double colorThreshold, double colorfulThreshold) {
	//This integer stores the level of cartoon likelihood, it has a minimum of 0 and a maximum of 2
	int cartoonLevel = 0;

	int edgeCount = cartoonEdges(image);
	cerr << "EDGES HERE" << edgeCount << "\n";

	//Calls howColorful to measure average "colorfulness"
	float colorfulMetric = howColorful(image);
    cerr << "COLOR SIMILARITY HERE " << colorfulMetric << "\n";

	double colorSimilarity = cartoonColors(image, 4);
	cerr << "COLOR VALUE HERE " << colorSimilarity << "\n";
	
	if (colorfulMetric >= 200) {
		//If the colorful metric is found to be over 200 it is very likely to be a cartoon so instantly return a 2.
		return 2;
	} else if (colorfulMetric >= colorfulThreshold) {
		//If the colorful metric is found to be greater than or equal to the threshold then cartoonLevel is increased by 1
		cartoonLevel++;
	}

	//This call creates and compares histograms with 64 bins to determine the gradient changes in the image
	

	// If the colorSimilarity is less than or equal to the given threshold cartoonLevel is increased by 1
	if (colorSimilarity <= colorThreshold) {
		cartoonLevel++;
	} 

	return cartoonLevel;
}

