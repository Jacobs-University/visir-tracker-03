#include "Marker.h"

void CMarker::markFaces(Mat& img, const std::vector<ptr_face_t>& vFaces)
{
	for (auto face : vFaces) {
		rectangle(img, face->getArea(), CV_RGB(0, 255, 0));
	}
}

void CMarker::markPoints(Mat& img, const std::vector<Point2f>& vPoints, Scalar color)
{
	for (auto& point : vPoints) 
		circle(img, point, 3, color, 2);
}

void CMarker::markVecOFF(Mat& mask, const std::vector<Point2f>& prev, const std::vector<Point2f>& curr, const std::vector<uchar> status)
{
	for (uint i = 0; i < prev.size(); i++) {
		if (status[i] == 1) {
			line(mask, prev[i], curr[i], Scalar(255,0,0), 2);
		}
	}
}

void CMarker::markGUI(Mat& img)
{
	// ------ PUT YOUR CODE HERE -------
	// Implement yout ouw GUI
	// Show fps
	for (int y = 0; y < img.rows; y++) {
		Vec3b* ptr = img.ptr<Vec3b>(y);
		for (int x = 0; x < img.cols; x++) {
			float k = static_cast<float>(x) / img.cols;
			ptr[x] = Vec3b(k * 255, 0, 255 - k * 255);
		}
	}
	circle(img, Point(img.cols / 2, img.rows / 2), 50, CV_RGB(100, 255, 100), 5);
	GaussianBlur(img, img, Size(17, 17), 50);
	putText(img, "HCI", Point(100, 100), FONT_HERSHEY_SIMPLEX, 2, CV_RGB(255, 255, 255), 5);
}
