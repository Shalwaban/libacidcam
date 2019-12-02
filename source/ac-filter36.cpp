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

void ac::XorRow(cv::Mat &frame) {
    static double alpha = 1.0;
    static int dir = 1;
    static double num = 2;
    static int num_dir = 1;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    if((z%static_cast<int>(num)) == 0) {
                        pixel[j] ^= static_cast<unsigned char>(pixel[j]*alpha);
                    } else {
                        pixel[j] ^= static_cast<unsigned char>(0.5 * (pixel[j]*alpha));
                    }
                }
            }
        }
    };
    AlphaMovementMaxMin(num, num_dir, 1.0, 50, 2.0);
    AlphaMovementMaxMin(alpha, dir, 0.001, 255.0, 1.0);
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::UseOldRow(cv::Mat &frame) {
    static MatrixCollection<32> collection1;
    static MatrixCollection<32> collection2;
    collection1.shiftFrames(frame);
    if(collection2.empty()) collection2.shiftFrames(frame);
    
    cv::Mat copy1 = frame.clone();
    
    static int square_max = (frame.rows / collection1.size());
    static int square_size = 25 + (rand()% (square_max - 25));
    int row = 0;
    int off = 0;
    int size_past = 0;
    while(row < frame.rows-1) {
        square_size = 25 + (rand()% (square_max - 25));
        for(int z = row; z < row+square_size; ++z) {
            int val = (rand()%10);
            for(int i = 0; i < frame.cols; ++i) {
                if(i < frame.cols && z < frame.rows) {
                    cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                    if(val > 7) {
                        if(off < (collection2.size()-1)) {
                            cv::Vec3b pix = collection2.frames[off].at<cv::Vec3b>(z, i);
                            for(int j = 0; j < 3; ++j) {
                                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                            }
                        }
                    } else {
                        if(off < (collection1.size()-1)) {
                            cv::Vec3b pix = collection1.frames[off].at<cv::Vec3b>(z, i);
                            for(int j = 0; j < 3; ++j) {
                                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                            }
                        }
                    }
                }
            }
        }
        row += square_size;
        size_past += square_size;
        if(size_past > square_max-1) {
            size_past = 0;
            ++off;
            if(off > (collection1.size()-1))
                break;
        }
    }
    
    static int counter = 0;
    if(++counter > 10) {
        counter = 0;
        collection2.shiftFrames(copy1);
    }
    AddInvert(frame);
}

void ac::UseEveryOtherRow(cv::Mat &frame) {
    static MatrixCollection<32> collection1;
    static MatrixCollection<32> collection2;
    collection1.shiftFrames(frame);
    if(collection2.empty()) collection2.shiftFrames(frame);
    
    cv::Mat copy1 = frame.clone();
    
    static int square_max = (frame.rows / collection1.size());
    static int square_size = 25 + (rand()% (square_max - 25));
    int row = 0;
    int off = 0;
    int size_past = 0;
    while(row < frame.rows-1) {
        square_size = 25 + (rand()% (square_max - 25));
        for(int z = row; z < row+square_size; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                if(i < frame.cols && z < frame.rows) {
                    cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                    if((row%2)==0) {
                        if(off < (collection2.size()-1)) {
                            cv::Vec3b pix = collection2.frames[off].at<cv::Vec3b>(z, i);
                            for(int j = 0; j < 3; ++j) {
                                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                            }
                        }
                    } else {
                        if(off < (collection1.size()-1)) {
                            cv::Vec3b pix = collection1.frames[off].at<cv::Vec3b>(z, i);
                            for(int j = 0; j < 3; ++j) {
                                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                            }
                        }
                    }
                }
            }
        }
        row += square_size;
        size_past += square_size;
        if(size_past > square_max-1) {
            size_past = 0;
            ++off;
            if(off > (collection1.size()-1))
                break;
        }
    }
    static int counter = 0;
    if(++counter > 8) {
        counter = 0;
        collection2.shiftFrames(copy1);
    }
    AddInvert(frame);
}

void ac::UseOffRow(cv::Mat &frame) {
    static MatrixCollection<32> collection1;
    static MatrixCollection<32> collection2;
    collection1.shiftFrames(frame);
    if(collection2.empty()) collection2.shiftFrames(frame);
    
    cv::Mat copy1 = frame.clone();
    
    static int square_max = (frame.rows / collection1.size());
    static int square_size = 25 + (rand()% (square_max - 25));
    int row = 0;
    int off = 0;
    int size_past = 0;
    while(row < frame.rows-1) {
        square_size = 25 + (rand()% (square_max - 25));
        for(int z = row; z < row+square_size; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                if(i < frame.cols && z < frame.rows) {
                    cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                    if((row%4)==0) {
                        if(off < (collection2.size()-1)) {
                            cv::Vec3b pix = collection2.frames[off].at<cv::Vec3b>(z, i);
                            for(int j = 0; j < 3; ++j) {
                                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                            }
                        }
                    } else {
                        if(off < (collection1.size()-1)) {
                            cv::Vec3b pix = collection1.frames[off].at<cv::Vec3b>(z, i);
                            for(int j = 0; j < 3; ++j) {
                                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                            }
                        }
                    }
                }
            }
        }
        row += square_size;
        size_past += square_size;
        if(size_past > square_max-1) {
            size_past = 0;
            ++off;
            if(off > (collection1.size()-1))
                break;
        }
    }
    static int counter = 0;
    if(++counter > 4) {
        counter = 0;
        collection2.shiftFrames(copy1);
    }
    AddInvert(frame);
}
