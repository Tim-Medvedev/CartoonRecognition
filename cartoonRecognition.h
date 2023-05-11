//cartoonRecognition.h
//Tim Medvedev
//Header file for cartoonRecognition.cpp
#pragma once
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/objdetect.hpp"
#include "opencv2/videoio.hpp"
#include <iostream>
#include <vector>
using namespace std;
using namespace cv;


 int isCartoon(Mat image, double colorThreshold, double colorfulThreshold);
 int reduceColor(const int val);
 float howColorful(Mat image);
 int cartoonEdges(Mat image);
 double cartoonColors(Mat image, int size);
 Mat cartoonFaceDetection(Mat image);