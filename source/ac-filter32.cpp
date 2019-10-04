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

void ac::IncreaseRainbowXorBlend(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat frames[3];
    frames[0] = collection.frames[1].clone();
    frames[1] = collection.frames[4].clone();
    frames[2] = collection.frames[7].clone();
    
    MedianBlendMultiThreadByEight(frame);
    MedianBlendMultiThread_2160p(frames[1]);
    MedianBlendIncrease(frames[2]);
    
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j <3; ++j) {
                    cv::Vec3b pix;
                    pix = frames[j].at<cv::Vec3b>(z, i);
                    pixel[j] = pixel[j]^pix[j];
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::ColorStrobeIndexSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "ColorStrobeIndexSubFilter")
        return;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat frames[3];
    static int offset = 0;
    if(++offset > 2)
        offset = 0;
    frames[0] = collection.frames[1].clone();
    frames[1] = collection.frames[4].clone();
    frames[2] = collection.frames[7].clone();
    CallFilter(subfilter, frames[offset]);
    static double alpha = 1.0;
    static int dir = 1;
    for(int index = 0; index < collection.size(); ++index) {
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    cv::Vec3b pix = frames[j].at<cv::Vec3b>(z, i);
                    pixel[j] = static_cast<unsigned char>((pixel[j] * alpha) + (pix[j] * (1-alpha)));
                }
            }
        }
    }
    MedianBlendMultiThreadByEight(frame);
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
    AddInvert(frame);
}

//void copyMat(const cv::Mat &src, const Point &p, cv::Mat &target, const Rect &rc);

void ac::MatrixCollectionRandomFrames(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    for(int index = 0; index < collection.size(); ++index) {
        int p_x = rand()%(frame.cols-1);
        int p_y = rand()%(frame.rows-1);
        if(p_x < 25)
            p_x = 25;
        if(p_y < 25)
            p_y = 25;
        int w = rand()%(frame.cols-1);
        int h = rand()%(frame.rows-1);
        Point p(0, 0);
        Rect rc(p_x, p_y, w-p_x, h-p_y);
        copyMat(collection.frames[index], p, frame, rc);
    }
    AddInvert(frame);
}

void ac::MatrixCollectionFrames(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    int p_x = 0, p_y = 0;
    for(int index = 0; index < collection.size(); ++index) {
        int p_w = frame.cols-p_x;
        int p_h = frame.rows-p_y;
        Rect rc(p_x, p_y, p_w, p_h);
        copyMat(collection.frames[index], 0, 0, frame, rc);
        p_x += 20;
        p_y += 20;
        if(p_x > frame.cols-20 || p_y > frame.rows-20)
            break;
    }
    VariableRectanglesExtra(frame);
    AddInvert(frame);
}

void ac::MatrixCollectionFramesLeft(cv::Mat &frame) {
    static constexpr int Size = 32;
    static MatrixCollection<Size> collection;
    collection.shiftFrames(frame);
    int s_x = 0;
    int s_w = frame.cols/Size;
    for(int index = 0; index < collection.size(); ++index) {
        Rect rc(s_x, 0, s_w, frame.rows);
        copyMat(collection.frames[index], 0, 0, frame, rc);
        s_x += s_w;
        if(s_x > frame.cols-s_w)
            break;
    }
    AddInvert(frame);
}

void ac::MatrixCollectionFramesMirrorLeft(cv::Mat &frame) {
    MatrixCollectionFramesLeft(frame);
    MirrorLeftBottomToTop(frame);
    AddInvert(frame);
}

void ac::MatrixCollectionFramesTop(cv::Mat &frame) {
    static constexpr int Size = 32;
    static MatrixCollection<Size> collection;
    collection.shiftFrames(frame);
    int s_y = 0;
    int s_h = frame.rows/Size;
    for(int index = 0; index < collection.size(); ++index) {
        Rect rc(0, s_y, frame.cols, s_h);
        copyMat(collection.frames[index], 0, 0, frame, rc);
        s_y += s_h;
        if(s_y > frame.rows-s_h)
            break;
    }
    AddInvert(frame);
}
