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

#include"ac.h"

void ac::ExpandSquareSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "ExpandSquareSubFilter")
        return;
    
    static int start_x = frame.cols/2;
    static int stop_x = frame.cols/2;
    static int speed = frame.cols/24;
    cv::Mat frame_copy = frame.clone();
    cv::Mat output;
    CallFilter(subfilter, frame_copy);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = start_x; i < stop_x; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            pixel = frame_copy.at<cv::Vec3b>(z, i);
        }
    }
    static int dir = 1;
    if(dir == 1) {
        start_x -= speed;
        stop_x += speed;
        if(start_x <= 0  || stop_x > frame.cols-1) {
            dir = 0;
        }
    } else {
        start_x += speed;
        stop_x -= speed;
        if(start_x >= (frame.cols/2)-1  || stop_x <= (frame.cols/2)-1) {
            dir = 1;
        }
    }
}

void ac::ExpandSquareBlendSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "ExpandSquareSubFilter")
        return;
    static int start_x = frame.cols/2;
    static int stop_x = frame.cols/2;
    static int speed = frame.cols/24;
    static double alpha = 1.0, alpha_max = 4.0;
    cv::Mat frame_copy = frame.clone();
    cv::Mat output;
    CallFilter(subfilter, frame_copy);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = start_x; i < stop_x; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = frame_copy.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] = static_cast<unsigned char>(pixel[j]*alpha) ^ static_cast<unsigned char>(pix[j]*alpha);
        }
    }
    static int dir = 1;
    if(dir == 1) {
        start_x -= speed;
        stop_x += speed;
        if(start_x <= 0  || stop_x > frame.cols-1) {
            dir = 0;
        }
    } else {
        start_x += speed;
        stop_x -= speed;
        if(start_x >= (frame.cols/2)-1  || stop_x <= (frame.cols/2)-1) {
            dir = 1;
        }
    }
    static int direction = 1;
    procPos(direction, alpha, alpha_max);
}

void ac::ExpandSquareVerticalSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "ExpandSquareVerticalSubFilter")
        return;
    static int start_x = frame.rows/2;
    static int stop_x = frame.rows/2;
    static int speed = frame.rows/24;
    cv::Mat frame_copy = frame.clone();
    cv::Mat output;
    CallFilter(subfilter, frame_copy);
    for(int z = 0; z < frame.cols; ++z) {
        for(int i = start_x; i < stop_x; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(i, z);
            pixel = frame_copy.at<cv::Vec3b>(i, z);
        }
    }
    static int dir = 1;
    if(dir == 1) {
        start_x -= speed;
        stop_x += speed;
        if(start_x <= 0  || stop_x > frame.rows-1) {
            dir = 0;
        }
    } else {
        start_x += speed;
        stop_x -= speed;
        if(start_x >= (frame.rows/2)-1  || stop_x <= (frame.rows/2)-1) {
            dir = 1;
        }
    }
}

void ac::DarkImageMedianBlend(cv::Mat &frame) {
    if(blend_set == true) {
    	SmoothImageAlphaBlend(frame);
    	cv::Mat frame_copy = frame.clone();
    	setGamma(frame_copy,frame,5);
    	MedianBlend(frame);
    }
}

void ac::GammaDarken5(cv::Mat &frame) {
    cv::Mat frame_copy = frame.clone();
    setGamma(frame_copy, frame, 5);
}

void ac::GammaDarken10(cv::Mat &frame) {
    cv::Mat frame_copy = frame.clone();
    setGamma(frame_copy, frame, 10);
}

void ac::SelfAlphaScaleBlend(cv::Mat &frame) {
    static double alpha = 1.0, alpha_max = 4.0;
    static MatrixCollection<4> collection;
    collection.shiftFrames(frame);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            unsigned int val = 0;
            for(int j = 0; j < 3; ++j) {
                val += static_cast<unsigned char>(pixel[j]*alpha);
                pixel[j] = pixel[j]^val;
            }
            
        }
    }
    static int dir = 1;
    procPos(dir,alpha,alpha_max);
}

void ac::FadeBars(cv::Mat &frame) {
    unsigned char ch[3] = {static_cast<unsigned char>(rand()%255), static_cast<unsigned char>(rand()%255), static_cast<unsigned char>(rand()%255)};
    static double alpha = 1.0, alpha_max = 4.0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((pixel[j]^ch[j])*alpha);
                ++ch[j];
            }
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 5.0, 0.1);
}

void ac::MirrorXorAlpha(cv::Mat &frame) {
    static double alpha[3] = {1.0, 3.0, 1.0}, alpha_max = 3.0;
    static cv::Vec3b color_(rand()%255, rand()%255, rand()%255);
    cv::Mat frame_copy = frame.clone();
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b values[3];
            values[0] = frame_copy.at<cv::Vec3b>(frame.rows-z-1, frame.cols-i-1);
            values[1] = frame_copy.at<cv::Vec3b>(frame.rows-z-1, i);
            values[2] = frame_copy.at<cv::Vec3b>(z, frame.cols-i-1);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = cv::saturate_cast<unsigned char>(static_cast<unsigned char>((pixel[j]*alpha[j])) ^ static_cast<unsigned char>((values[0][j]*alpha[j])) ^ static_cast<unsigned char>((values[1][j]*alpha[j])) ^ static_cast<unsigned char>((values[2][j]*alpha[j])));
                pixel[j] = pixel[j]^color_[j];
            }
        }
    }
    AddInvert(frame);
    static int dir[3] = {1, 0, 1};
    for(int j = 0; j < 3; ++j) {
        if(dir[j] == 1) {
            color_[j] += 5;
        } else if(dir[j] == 0) {
            color_[j] -= 5;
        }
    	procPos(dir[j], alpha[j], alpha_max, 4.0, 0.1);
    }
}

void ac::MirrorEnergizeSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "MirrorEnergizeSubFilter")
        return;
    DarkenImage(frame, 6);
    MirrorXorAlpha(frame);
    EnergizeSubFilter(frame);
}

void ac::StrobeXor(cv::Mat &frame) {
    cv::Vec3b pix(rand()%255, rand()%255, rand()%255);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] = pixel[j]^pix[j];
        }
    }
    AddInvert(frame);
}

void ac::IntertwinedMirror(cv::Mat &frame) {
    
    cv::Mat frame_copy = frame.clone();
    int lines = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b values[3];
            values[0] = frame_copy.at<cv::Vec3b>(frame.rows-z-1, frame.cols-i-1);
            values[1] = frame_copy.at<cv::Vec3b>(frame.rows-z-1, i);
            values[2] = frame_copy.at<cv::Vec3b>(z, frame.cols-i-1);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = pixel[j]^values[lines][j];
            }
        }
        ++lines;
        if(lines > 2)
            lines = 0;
    }
}

void ac::BlurredMirror(cv::Mat &frame) {
    cv::Mat frame_copy = frame.clone();
    GaussianBlur(frame_copy);
    static double alpha = 1.0, alpha_max = 4.0;
    int lines = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b values[3];
            values[0] = frame_copy.at<cv::Vec3b>(frame.rows-z-1, frame.cols-i-1);
            values[1] = frame_copy.at<cv::Vec3b>(frame.rows-z-1, i);
            values[2] = frame_copy.at<cv::Vec3b>(z, frame.cols-i-1);
            for(int j = 0; j < 3; ++j)
                pixel[j] = static_cast<unsigned char>((pixel[j]^values[lines][j])*alpha);
        }
        ++lines;
        if(lines > 2)
            lines = 0;
    }
    MedianBlur(frame);
    static int dir = 1;
    procPos(dir, alpha, alpha_max);
}
