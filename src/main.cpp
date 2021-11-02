#include "types.h"
#include "CameraController.h"
#include "Face.h"
#include "Marker.h"

using namespace std;
CascadeClassifier face_cascade("../../data/haarcascade_frontalface_default.xml");

vector<ptr_face_t> detect_faces(const Mat& img) {
	Mat gray_img;
	vector<Rect> faces;
	cvtColor(img, gray_img, COLOR_BGR2GRAY);
	equalizeHist(gray_img, gray_img);
	face_cascade.detectMultiScale(gray_img, faces, 1.2, 3);
	vector<ptr_face_t> res;
	for (auto face: faces)
		res.push_back(std::make_shared<CFace>(face));
	return res;
}

// Parameters for Shi-Tomasi Corner Detector
int maxCorners = 128;
double qualityLevel = 0.01;
double minDistance = 10;

int main() {
	
	std::vector<Point2f> vPoints;
	std::vector<ptr_face_t> vpFaceses;
	
	namedWindow("Camera");
	
	// mouse callback function which fills vPoints with coordinates of mouse clicks
	setMouseCallback("Camera", [] (int event, int x, int y, int flags, void* userdata) {
		if (userdata && event == EVENT_LBUTTONDOWN) {
			std::vector<Point2f> *pvPoints = (std::vector<Point2f> *) userdata;
			pvPoints->push_back(Point2f(x, y));
		}
	}, (void*) &vPoints);
	
	CCameraCantroller controller(16);

	controller.init();
	controller.start();

	// Setup various variables for Lukas-Kanade method
	Mat old_img, old_grey;
	vector<Point2f> p0, p1;
	vector<float> err;
	vector<uchar> status;
	TermCriteria criteria = TermCriteria((TermCriteria::COUNT) + (TermCriteria::EPS), 10, 0.03);

	// Getting initial old image and its features
	old_img = controller.getFrame();
	goodFeaturesToTrack(old_grey, p0, maxCorners, qualityLevel, minDistance);

	// Main loop
	Mat img, grey, mask;
	float attenuation = 0.5f;
	int frame_counter = 10;
	for(;;) {
		img = controller.getFrame();

		if (!img.empty()) {
			if (mask.empty()) mask = Mat(img.size(), img.type());
			mask.setTo(0);
			cvtColor(img, grey, COLOR_BGR2GRAY);

			// Detecting face and marking it every 10th frame
			if (frame_counter == 10) {
				vpFaceses = detect_faces(img);
				frame_counter = 0;

				// Getting good features to track and visualising
				goodFeaturesToTrack(grey, p0, maxCorners, qualityLevel, minDistance);
				CMarker::markPoints(mask, p0, Scalar(255,255,255));
			}
			frame_counter++;
			CMarker::markFaces(mask, vpFaceses);

			// Calculating optical flow and visualising
			vector<Point2f> p2;
			calcOpticalFlowPyrLK(old_grey, grey, p0, p1, status, err, Size(15,15), 3, criteria);
			CMarker::markVecOFF(mask, p0, p1, status);
			for (uint i = 0; i < p0.size(); i++) {
				if (status[i] == 1) {
					p2.push_back(p1[i]);
				}
			}
			p0 = p2;
			old_grey = grey.clone();
			
			CMarker::markGUI(mask);
			add(img, attenuation * mask, img);
			imshow("Camera", img);
		}
		int key = waitKey(5);
		if (key == 27 || key == 'q') break;
		if (key == 'a') attenuation *= 1.1f;
		if (key == 'z') attenuation *= 0.9f;
	}

	controller.stop();
	return 0;
}
	
