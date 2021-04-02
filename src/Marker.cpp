#include "Marker.h"

using namespace cv;

void CMarker::markFaces(Mat& img, const std::vector<ptr_face_t>& vFaces)
{
    for (auto face : vFaces){
        // ------ PUT YOUR CODE HERE -------
        // 1. Draw all faces using face->getArea();
        // 2. Print the text using face->getText();
        // 3. Print face id using face->getId();
        Point center(face->getArea().x + face->getArea().width/2, face->getArea().y + face->getArea().height/2);
        ellipse(img, center, Size(face->getArea().width/2, face->getArea().height/2 ), 0, 0, 360, Scalar(255, 0, 255), 4);
    }
}

void CMarker::markPoints(Mat& img, const std::vector<Point2f>& vPoints, Scalar color)
{
    for (auto& point : vPoints)
        circle(img, point, 3, color, 2);
}

void CMarker::markVecOFF(Mat& img, Mat& mask,  const std::vector<std::tuple<Point2f, Point2f>>& flowPoints)
{
    // ------ PUT YOUR CODE HERE -------
    std::vector<Scalar> colors;
    RNG rand;
    
    for(int i = 0; i < 100; i++){
        int r = rand.uniform(0, 256);
        int g = rand.uniform(0, 256);
        int b = rand.uniform(0, 256);
        colors.push_back(Scalar(r,g,b));
      }
    int j = 0;
      for(const auto &i : flowPoints){
      line(mask, std::get<1>(i), std::get<0>(i), colors[j], 2);
      circle(img, std::get<1>(i), 5, colors[j], -2);
      j++;
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
