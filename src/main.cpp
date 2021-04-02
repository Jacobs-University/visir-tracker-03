#include "types.h"
#include "CameraController.h"
#include "Face.h"
#include "Marker.h"
#include <iostream>
#include <functional>
#include <tuple>

using namespace std;
using namespace cv;

std::vector<ptr_face_t> detectFaces(Mat img);
std::vector<Point2f> featureExtraction(Mat img);
std::vector<Point2f> faceFeatureExtraction(Mat img, std::vector<ptr_face_t> vpFaces);
std::vector<std::tuple<Point2f, Point2f>> calculateOpticalFlow(Mat old_gray, Mat new_img, std::vector<Point2f> vPoint);

int main() {
    

    std::vector<Point2f> vPoints, vFacePoints;
    std::vector<ptr_face_t> vpFaces;
    std::vector<std::tuple<Point2f, Point2f>> opticalFlowPoints, faceOpticalFlowPoints;
    
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
    
    // Main loop
    Mat img, new_img;
    Mat mask, old_gray;
    float attenuation = 0.5f;
    int fcounter = 0;
    for(;;) {
        img = controller.getFrame();

        if (!img.empty()) {
            if (mask.empty()) mask = Mat(img.size(), img.type());
            mask.setTo(0);
            
            if (fcounter % 10 == 0){
               vpFaces = detectFaces(img);
               vPoints = featureExtraction(img);
               vFacePoints = faceFeatureExtraction(img, vpFaces);
            } else{
               new_img = controller.getFrame();
               opticalFlowPoints = calculateOpticalFlow(img, new_img, vPoints);
               faceOpticalFlowPoints = calculateOpticalFlow(img, new_img, vFacePoints);
            }
            
            CMarker::markFaces(img, vpFaces);
            CMarker::markVecOFF(img, mask, opticalFlowPoints);
            CMarker::markVecOFF(img, mask, faceOpticalFlowPoints);
            CMarker::markGUI(mask);
            CMarker::markPoints(img, vPoints);
            CMarker::markPoints(img, vFacePoints);

            add(img, attenuation * mask, img);

            imshow("Camera", img);
        }
        int key = waitKey(5);
        if (key == 27 || key == 'q') break;
        if (key == 'a') attenuation *= 1.1f;
        if (key == 'z') attenuation *= 0.9f;
        fcounter++;
    }

    controller.stop();
    return 0;
}


std::vector<ptr_face_t> detectFaces(Mat img)
{
    String face_cascade_name;
    CascadeClassifier face_cascade;
    Mat frame_gray;
    std::vector<ptr_face_t> vpFaces;
    
    face_cascade_name = samples::findFile("../opencv4/haarcascades/haarcascade_frontalface_alt.xml");

    face_cascade.load(face_cascade_name);
    cvtColor(img, frame_gray, COLOR_BGR2GRAY);
    equalizeHist(frame_gray, frame_gray);

    std::vector<Rect> faces;
    face_cascade.detectMultiScale(frame_gray, faces);

    for (auto face : faces)
    {
       CFace* vpFace = new CFace(face);
       vpFaces.emplace_back(vpFace);
    }
    
    return vpFaces;
}



std::vector<Point2f> featureExtraction(Mat img)
{
    Mat img_gray;
    std::vector<Point2f> corners;
    cvtColor(img, img_gray, COLOR_BGR2GRAY);
    goodFeaturesToTrack(img_gray, corners, 100, 0.01, 7, Mat(), 7, false, 0.04);
    return corners;
}

std::vector<Point2f> faceFeatureExtraction(Mat img, std::vector<ptr_face_t> vpFaces)
{
    Mat img_gray;
    std::vector<Point2f> vPoints;
    cvtColor(img, img_gray, COLOR_BGR2GRAY);
    for(auto face: vpFaces)
    {
       std::vector<Point2f> corners;
       Mat mask = Mat::zeros(img.size(), CV_8UC1);
           Mat roi(mask, face->getArea());
           roi = Scalar(255, 255, 255);
       goodFeaturesToTrack(img_gray, corners, 23, 0.01, 10, mask, 3, false, 0.04);

       for(auto corner: corners)
       {
         vPoints.push_back(corner);
       }
       
    }
    return vPoints;
}

std::vector<std::tuple<Point2f, Point2f>>  calculateOpticalFlow(Mat img, Mat new_img, std::vector<Point2f> vPoints)
{
     Mat old_gray;
     cvtColor(img, old_gray, COLOR_BGR2GRAY);
     std::vector<Point2f> vPoints_new, vPoints_temp;
    
     Mat img_gray;
     cvtColor(new_img, img_gray, COLOR_BGR2GRAY);
    
     std::vector<uchar> status;
     std::vector<float> err;
     TermCriteria criteria = TermCriteria((TermCriteria::COUNT) + (TermCriteria::EPS), 10, 0.03);

     if(vPoints.size() != 0) //handle the case when vPoints is empty so the code doesn't break
     {
     calcOpticalFlowPyrLK(old_gray, img_gray, vPoints, vPoints_new, status, err, Size(15,15), 2, criteria);
     }
     std::vector<std::tuple<Point2f, Point2f>>  good_points;
     

     for(uint i = 0; i < vPoints.size(); i++)
     {
        if(status[i] == 1)
        {
      std::tuple<Point2f, Point2f> value = std::make_tuple(vPoints[i], vPoints_new[i]);
      good_points.push_back(value);
      vPoints_temp.push_back(vPoints_new[i]);
        }
     }
     return good_points;
}
