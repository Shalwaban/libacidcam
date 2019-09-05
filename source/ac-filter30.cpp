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

#include"ac.h"

void ac::StrangeGlitch64(cv::Mat &frame) {
    static MatrixCollection<64> collection;
    collection.shiftFrames(frame);
    cv::Mat copy1 = frame.clone();
    for(int q = 0; q < collection.size(); ++q) {
        cv::Mat &copy_frame = collection.frames[q];
        for(int z = 0; z < copy_frame.rows; ++z) {
            for(int i = 0; i < copy_frame.cols; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                cv::Vec3b pix = copy_frame.at<cv::Vec3b>(z, i);
                if(colorBounds(pixel, pix, cv::Vec3b(30, 30, 30), cv::Vec3b(30, 30, 30))) {
                    for(int j = 0; j < 3; ++j) {
                        pixel[j] = pix[j];
                    }
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::StrangeGlitch16(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    collection.shiftFrames(frame);
    cv::Mat copy1 = frame.clone();
    for(int q = 0; q < collection.size(); ++q) {
        cv::Mat &copy_frame = collection.frames[q];
        for(int z = 0; z < copy_frame.rows; ++z) {
            for(int i = 0; i < copy_frame.cols; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                cv::Vec3b pix = copy_frame.at<cv::Vec3b>(z, i);
                if(colorBounds(pixel, pix, cv::Vec3b(30, 30, 30), cv::Vec3b(30, 30, 30))) {
                    for(int j = 0; j < 3; ++j) {
                        pixel[j] = pix[j];
                    }
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::CollectionMatrixOutline(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    ColorTransition(frame);
    collection.shiftFrames(frame);
    cv::Mat copy1 = frame.clone();
    MedianBlendIncrease(copy1);
    static int val = 4;
    cv::Mat &copy_frame = collection.frames[val];
    for(int z = 0; z < copy_frame.rows; ++z) {
        for(int i = 0; i < copy_frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = copy_frame.at<cv::Vec3b>(z, i);
            if(colorBounds(pixel, pix, cv::Vec3b(50, 50, 50), cv::Vec3b(50, 50, 50))) {
                pixel = cv::Vec3b(0,0,0);
            } else {
                pixel = copy1.at<cv::Vec3b>(z, i);
            }
        }
    }
    AddInvert(frame);
}

void ac::CollectionMatrixSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "CollectionMatrixSubFilter")
        return;
    static MatrixCollection<8> collection;
    CallFilter(subfilter, frame);
    collection.shiftFrames(frame);
    cv::Mat copy1 = frame.clone();
    MedianBlendIncrease(copy1);
    static int val = 4;
    cv::Mat &copy_frame = collection.frames[val];
    for(int z = 0; z < copy_frame.rows; ++z) {
        for(int i = 0; i < copy_frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = copy_frame.at<cv::Vec3b>(z, i);
            if(colorBounds(pixel, pix, cv::Vec3b(50, 50, 50), cv::Vec3b(50, 50, 50))) {
                pixel = cv::Vec3b(0,0,0);
            } else {
                pixel = copy1.at<cv::Vec3b>(z, i);
            }
        }
    }
    AddInvert(frame);
}

