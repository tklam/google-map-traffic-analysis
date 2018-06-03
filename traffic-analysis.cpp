#include <opencv2/opencv.hpp>
#include <iostream>
#include <algorithm>
#include <cstdio>

using namespace std;

// Remarks;
// for 32-bit images:
// GIMP HSV ranges:     H: 0-360, S: 0-100, V: 0-100
// OpenCV HSV ranges:   H: 0-360, S: 0-255, V: 0-255
//
// for 8-bit images: (using this)
// GIMP HSV ranges:     H: 0-360, S: 0-100, V: 0-100
// OpenCV HSV ranges:   H: 0-180, S: 0-255, V: 0-255

// Usage:
// 1. adjust the zoom level of the map to focus the area of interest
// 2. screen capture the map as a png file
// 3. ./traffic-analysis <the map image>
// * need to turn the parameters 

static int gimp_hue_to_opencv_hue(int hue) {
    return (int)(hue / 360.0f * 180.0f);
}

static int gimp_sat_to_opencv_sat(int sat) {
    return (int)(sat / 100.0f * 255.0f);
}

static int gimp_val_to_opencv_val(int val) {
    return (int)(val/ 100.0f * 255.0f);
}

void detect_traffic_lines( vector<cv::Mat> hsvChannels, 
        int gimpHue, int gimpSat, int gimpVal, 
        int gimpHueRangeLower, int gimpHueRangeUpper, 
        int gimpSatRange, int gimpValRange, 
        vector<cv::Vec4i> & lines) {
    const int hueValue = gimp_hue_to_opencv_hue(gimpHue); 
    const int hueRangeLower = gimp_hue_to_opencv_hue(gimpHueRangeLower);
    const int hueRangeUpper = gimp_hue_to_opencv_hue(gimpHueRangeUpper);
    const int satValue = gimp_sat_to_opencv_sat(gimpSat); 
    const int satRange = gimp_sat_to_opencv_sat(gimpSatRange); 
    const int valValue = gimp_val_to_opencv_val(gimpVal);
    const int valRange = gimp_val_to_opencv_val(gimpValRange);

    // in OpenCV ranges:
    int hueLower = hueValue - hueRangeLower;
    int hueUpper = hueValue + hueRangeUpper;
    int satLower = max(satValue - satRange, 0);
    int satUpper = min(satValue + satRange, 255);
    int valLower = max(valValue - valRange, 0);
    int valUpper = min(valValue + valRange, 255);

    // Note: Hue is circular. OpenCV will help warping the value automatically.

    cv::Mat hueMask;
    cv::Mat saturationMask;
    cv::Mat valueMask;

    cv::inRange(hsvChannels[0], hueLower, hueUpper, hueMask);
    cv::inRange(hsvChannels[1], satLower, satUpper, saturationMask);
    cv::inRange(hsvChannels[2], valLower, valUpper, valueMask);

    hueMask = (hueMask & saturationMask) & valueMask;

    // perform line detection
    cv::HoughLinesP(hueMask, lines, 1, CV_PI / 360, 100, 10, 100);
}

int main(int argc, char* argv[])
{
    cv::Mat input = cv::imread(argv[1]);

    // convert to HSV color space
    cv::Mat hsvImage;
    cv::cvtColor(input, hsvImage, CV_BGR2HSV);

    // split the channels
    vector<cv::Mat> hsvChannels;
    cv::split(hsvImage, hsvChannels); // [hue, saturation, value]

    // traffic code detection
    vector<cv::Vec4i> veryGreenLines;
    vector<cv::Vec4i> greenLines;
    vector<cv::Vec4i> redLines;
    vector<cv::Vec4i> veryRedLines;
    detect_traffic_lines(hsvChannels, 90, 60, 79, 29, 50, 30, 30, veryGreenLines);
    detect_traffic_lines(hsvChannels, 30, 99, 94, 14, 30, 30, 30, greenLines);
    detect_traffic_lines(hsvChannels, 0, 100, 90, 15, 15, 30, 30, redLines); 
    detect_traffic_lines(hsvChannels, 0, 88, 62, 15, 15, 15, 15, veryRedLines);

    printf("Detected: \n \
            %ld very green lines, %ld green lines, \n \
            %ld very red lines, %ld red lines\n",
            veryGreenLines.size(), greenLines.size(), 
            veryRedLines.size(), redLines.size());

    // decide the level of traffic jam by finding the major type of lines

    // draw and show the detected lines
    // user can type a key to close the result
    { // this block can be removed
    for (unsigned int i = 0; i < veryGreenLines.size(); ++i) {
        cv::line(input, 
                cv::Point(veryGreenLines[i][0], veryGreenLines[i][1]), 
                cv::Point(veryGreenLines[i][2], veryGreenLines[i][3]), 
                cv::Scalar(0, 255, 0), 5);
    }
    for (unsigned int i = 0; i < greenLines.size(); ++i) {
        cv::line(input, 
                cv::Point(greenLines[i][0], greenLines[i][1]), 
                cv::Point(greenLines[i][2], greenLines[i][3]), 
                cv::Scalar(0, 255, 0), 5);
    }
    for (unsigned int i = 0; i < redLines.size(); ++i) {
        cv::line(input, 
                cv::Point(redLines[i][0], redLines[i][1]), 
                cv::Point(redLines[i][2], redLines[i][3]), 
                cv::Scalar(0, 0, 255), 5);
    }
    for (unsigned int i = 0; i < veryRedLines.size(); ++i) {
        cv::line(input, 
                cv::Point(veryRedLines[i][0], veryRedLines[i][1]), 
                cv::Point(veryRedLines[i][2], veryRedLines[i][3]), 
                cv::Scalar(0, 0, 255), 5);
    }

    cv::imshow("input", input); // draw the
    cv::waitKey(0);
    }
    return 0;
}

// vim: set ts=4 sw=4 tw=80 spell:
