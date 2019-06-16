
// add CXXFLAG  -I/usr/local/include/acidcam
// Using CallFilter

#include"ac.h"
#include<iostream>
#include<string>

int main(int argc, char **argv) {
    ac::init(); // init libacidcam
    cv::VideoCapture cap(0);
    if(!cap.isOpened()) {
        std::cerr << "Error opening webcam...\n";
        exit(EXIT_FAILURE);
    }
    cv::namedWindow("Example1");
    
    int index = 0;
    // program loop
    while(1){
        cv::Mat frame;
        if(cap.read(frame)) {
            ac::CallFilter(index, frame);
            cv::imshow("Example1", frame);
        } else
            break;
        int key;
        key = cv::waitKey(50);
        if(key == 27) {
            exit(EXIT_SUCCESS);
        }
        ++index;
        if(index > 10)
            index = 0;
    }
    return 0;
}

