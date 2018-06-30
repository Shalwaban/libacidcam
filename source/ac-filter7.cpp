/* Acid Cam Functions for OpenCV
 * written by Jared Bruni https://github.com/lostjared
 
 This software is dedicated to all the people that struggle with mental illness.
 
 Website: http://lostsidedead.com
 YouTube: http://youtube.com/LostSideDead
 Instagram: http://instagram.com/jaredbruni
 Twitter: http://twitter.com/jaredbruni
 Facebook: http://facebook.com/LostSideDead0x
 
 You can use this program free of charge and redistrubute as long
 as you do not charge anything for this program. This program is 100%
 Free.
 
 BSD 2-Clause License
 
 Copyright (c) 2018, Jared Bruni
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 
 * Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.
 
 * Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
 */

#include "ac.h"


void ac::SlideUpDown(cv::Mat &frame) {
    static const int speed = 40;
    static int start_1 = 0, start_2 = frame.rows-1;
    static int direction_1 = 1, direction_2 = 0;
    static double alpha = 1.0, alpha_max = 3.0;
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < start_1; ++z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>(pixel[j]*alpha);
            }
        }
        for(int z =(frame.rows-1); z > start_2; --z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] = static_cast<unsigned char>(pixel[j]*alpha);
            
        }
    }
    if(direction_1 == 1) {
        start_1 += speed;
        if(start_1 > (frame.rows-1)) {
            direction_1 = 0;
            start_1 = (frame.rows-1);
        }
    } else {
        start_1 -= speed;
        if(start_1 <= 0) {
            direction_1 = 1;
            start_1 = 0;
        }
    }
    if(direction_2 == 1) {
        start_2 += speed;
        if(start_2 >= (frame.rows-1)) {
            direction_2 = 0;
            start_2 = (frame.rows-1);
        }
    } else {
        start_2 -= speed;
        if(start_2 <= 0) {
            direction_2 = 1;
            start_2 = 0;
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 9.0, 0.005);
    AddInvert(frame);
}

void ac::SlideUpDownXor(cv::Mat &frame) {
    static const int speed = 40;
    static int start_1 = 0, start_2 = frame.rows-1;
    static int direction_1 = 1, direction_2 = 0;
    static double alpha = 1.0, alpha_max = 3.0;
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < start_1; ++z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] ^= static_cast<unsigned char>(pixel[j]*alpha);
            }
        }
        for(int z =(frame.rows-1); z > start_2; --z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] ^= static_cast<unsigned char>(pixel[j]*alpha);
            
        }
    }
    if(direction_1 == 1) {
        start_1 += speed;
        if(start_1 > (frame.rows-1)) {
            direction_1 = 0;
            start_1 = (frame.rows-1);
        }
    } else {
        start_1 -= speed;
        if(start_1 <= 0) {
            direction_1 = 1;
            start_1 = 0;
        }
    }
    if(direction_2 == 1) {
        start_2 += speed;
        if(start_2 >= (frame.rows-1)) {
            direction_2 = 0;
            start_2 = (frame.rows-1);
        }
    } else {
        start_2 -= speed;
        if(start_2 <= 0) {
            direction_2 = 1;
            start_2 = 0;
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 9.0, 0.005);
    AddInvert(frame);
}

void ac::SlideUpDownRandom(cv::Mat &frame) {
    DrawFunction f1, f2;
	static int index[2];
    f1 = getRandomFilter(index[0]);
    f2 = getRandomFilter(index[1]);
    if(ac::draw_strings[index[0]] == "SlideUpDownRandom") return;
    if(ac::draw_strings[index[1]] == "SlideUpDownRandom") return;
    cv::Mat frames[2];
    frames[0] = frame.clone();
    frames[1] = frame.clone();
    f1(frames[0]);
    f2(frames[1]);
    static const int speed = 40;
    static int start_1 = 0, start_2 = frame.rows-1;
    static int direction_1 = 1, direction_2 = 0;
    static double alpha = 1.0, alpha_max = 3.0;
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < start_1; ++z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = frames[0].at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] ^= static_cast<unsigned char>(pix[j]*alpha);
            }
        }
        for(int z =(frame.rows-1); z > start_2; --z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = frames[1].at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] ^= static_cast<unsigned char>(pix[j]*alpha);
            
        }
    }
    if(direction_1 == 1) {
        start_1 += speed;
        if(start_1 > (frame.rows-1)) {
            direction_1 = 0;
            start_1 = (frame.rows-1);
        }
    } else {
        start_1 -= speed;
        if(start_1 <= 0) {
            direction_1 = 1;
            start_1 = 0;
        }
    }
    if(direction_2 == 1) {
        start_2 += speed;
        if(start_2 >= (frame.rows-1)) {
            direction_2 = 0;
            start_2 = (frame.rows-1);
        }
    } else {
        start_2 -= speed;
        if(start_2 <= 0) {
            direction_2 = 1;
            start_2 = 0;
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 9.0, 0.005);
    AddInvert(frame);
}

void ac::SlideSubFilter(cv::Mat &frame) {
    static const int speed = 40;
    static int start_1 = 0, start_2 = frame.cols-1;
    static int direction_1 = 1, direction_2 = 0;
    static double alpha = 1.0, alpha_max = 7.0;
    if(subfilter == -1 || ac::draw_strings[subfilter] == "SlideSubFilter") return;
    cv::Mat frame_x;
    frame_x = frame.clone();
    if(ac::subfilter != -1) {
        ac::draw_func[ac::subfilter](frame_x);
    } else return;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < start_1; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = frame_x.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] ^= static_cast<unsigned char>(pix[j]*alpha);
            }
        }
        for(int i =(frame.cols-1); i > start_2; --i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = frame_x.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] ^= static_cast<unsigned char>(pix[j]*alpha);
        }
    }
    if(direction_1 == 1) {
        start_1 += speed;
        if(start_1 > (frame.cols-1)) {
            direction_1 = 0;
            start_1 = (frame.cols-1);
        }
    } else {
        start_1 -= speed;
        if(start_1 <= 0) {
            direction_1 = 1;
            start_1 = 0;
        }
    }
    if(direction_2 == 1) {
        start_2 += speed;
        if(start_2 >= (frame.cols-1)) {
            direction_2 = 0;
            start_2 = (frame.cols-1);
        }
    } else {
        start_2 -= speed;
        if(start_2 <= 0) {
            direction_2 = 1;
            start_2 = 0;
        }
    }
    
    static int dir = 1;
    procPos(dir, alpha, alpha_max);
    AddInvert(frame);
}

void ac::SlideSubUpDownFilter(cv::Mat &frame) {
    static const int speed = 40;
    static int start_1 = 0, start_2 = frame.rows-1;
    static int direction_1 = 1, direction_2 = 0;
    static double alpha = 1.0, alpha_max = 3.0;
    if(subfilter == -1 || ac::draw_strings[subfilter] == "SlideSubUpDownFilter") return;
    cv::Mat frame_x;
    frame_x = frame.clone();
    if(ac::subfilter != -1) {
        ac::draw_func[ac::subfilter](frame_x);
    } else return;
    
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < start_1; ++z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = frame_x.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] ^= static_cast<unsigned char>(pix[j]*alpha);
            }
        }
        for(int z =(frame.rows-1); z > start_2; --z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = frame_x.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] ^= static_cast<unsigned char>(pix[j]*alpha);
            
        }
    }
    if(direction_1 == 1) {
        start_1 += speed;
        if(start_1 > (frame.rows-1)) {
            direction_1 = 0;
            start_1 = (frame.rows-1);
        }
    } else {
        start_1 -= speed;
        if(start_1 <= 0) {
            direction_1 = 1;
            start_1 = 0;
        }
    }
    if(direction_2 == 1) {
        start_2 += speed;
        if(start_2 >= (frame.rows-1)) {
            direction_2 = 0;
            start_2 = (frame.rows-1);
        }
    } else {
        start_2 -= speed;
        if(start_2 <= 0) {
            direction_2 = 1;
            start_2 = 0;
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 9.0, 0.005);
    AddInvert(frame);
}

