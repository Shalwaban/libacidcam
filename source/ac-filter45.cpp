
/*
 * Software written by Jared Bruni https://github.com/lostjared
 
 This software is dedicated to all the people that experience mental illness.
 
 Website: http://lostsidedead.com
 YouTube: http://youtube.com/LostSideDead
 Instagram: http://instagram.com/lostsidedead
 Twitter: http://twitter.com/jaredbruni
 Facebook: http://facebook.com/LostSideDead0x
 
 You can use this program free of charge and redistrubute it online as long
 as you do not charge anything for this program. This program is meant to be
 100% free.
 
 BSD 2-Clause License
 
 Copyright (c) 2019, Jared Bruni
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

/*
 //Basic Multithreaded Filter
 auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
 for(int z = offset; z <  offset+size; ++z) {
 for(int i = 0; i < cols; ++i) {
 }
 }
 };
 UseMultipleThreads(frame, getThreadCount(), callback);
 AddInvert(frame);
 
 */

#include "ac.h"

void ac::FloatFadeVertical(cv::Mat &frame) {
    static float index[3] {-1, -1, -1};
    static float speed[3] = {0.1, 0.2, 0.3};
    static float dir[3] = {1,1,1};
    if(index[0] == -1 && index[1] == -1 && index[2] == -1) {
        index[0] = rand()%255;
        index[1] = rand()%255;
        index[2] = rand()%255;
    }
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.2 * pixel[j]) +  (0.8 * (pixel[j]+index[j])));
            }
        }
        for(int j = 0; j < 3; ++j) {
            if(dir[j] == 1) {
                if(index[j] > 255) {
                    dir[j]  = 0;
                } else {
                    index[j] += speed[j];
                }
            }
            else {
                if(index[j] <= 1) {
                    dir[j] = 1;
                } else {
                    index[j] -= speed[j];
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::LineTrails(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    static int index = 0;
    for(int z = 0; z < frame.rows; ++z) {
        bool on = true;
        int r = rand()%20;
        int in = 0;
        for(int i = 0; i < frame.cols; ++i) {
            ++in;
            if(in > r) {
                on = !on;
                r = rand()%20;
                in = 0;
            }
            if(on == true) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
                pixel = pix;
            }
        }
    }
    ++index;
    if(index > collection.size()-1)
        index = 0;
    AddInvert(frame);
}

void ac::SquareShiftDirVertical(cv::Mat &frame) {
    static int off = 0;
    static int frame_height = frame.cols/4;
    cv::Mat copy1 = frame.clone();
    for(int row = 0; row < frame.cols; row += frame_height) {
        off = rand()%frame.rows/64;
        if((rand()%8) > 6) {
            int on = rand()%2;
            for(int i = row; i < (row+frame_height) && (i < frame.cols); ++i) {
                int pos = 0;
                if(on == 0) {
                    for(int z = off; z < frame.rows; ++z) {
                        if(z < frame.rows && i < frame.cols) {
                            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                            cv::Vec3b pix = copy1.at<cv::Vec3b>(z, pos);
                            ++pos;
                            pixel = pix;
                        }
                    }
                } else {
                    pos = frame.cols-1;
                    for(int z = frame.rows-1-off; z > 1; --z) {
                        if(i < frame.cols && z < frame.rows) {
                            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                            cv::Vec3b pix = copy1.at<cv::Vec3b>(z, pos);
                            --pos;
                            pixel = pix;
                        }
                    }
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::StretchLineRow(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    int stretch_x = frame.cols;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            int nw = AC_GetFX(frame.cols, i, stretch_x);
            cv::Vec3b pix = copy1.at<cv::Vec3b>(z, nw);
            pixel = pix;
        }
        if(rand()%2==0) {
            stretch_x -= rand()%12;
            if(stretch_x < frame.cols)
                stretch_x = frame.cols;
        } else {
            stretch_x += rand()%12;
        }
    }
    AddInvert(frame);
}