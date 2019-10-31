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

void ac::VideoMirrorAlpha(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        cv::Mat copy1 = frame.clone();
        AlphaBlendDouble(reframe, copy1, frame, 0.5, 0.5);
        MirrorLeftBottomToTop(frame);
    }
    AddInvert(frame);
}

void ac::VideoMirrorLeftRight(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        cv::Mat copy1 = frame.clone();
        static int index = 0;
        if(index == 0) {
            MirrorLeft(reframe);
            MirrorRight(copy1);
        } else {
            MirrorRight(reframe);
            MirrorLeft(copy1);
        }
        index = (index == 0) ? 1 : 0;
        AlphaBlendDouble(reframe, copy1, frame, 0.5, 0.5);
    }
    AddInvert(frame);
}

void ac::VideoFlipFrames(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    cv::Mat vframe;
    cv::Mat copy1 = frame.clone();
    static int index = 0;
    if(VideoFrame(vframe)) {
        cv::Mat flipped;
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        if(index == 0) {
            cv::flip(reframe, flipped, 1);
        } else {
            cv::flip(copy1, flipped, 1);
        }
        index = (index == 0) ? 1 : 0;
        AlphaBlendDouble(reframe, flipped, frame, 0.5, 0.5);
    }
    AddInvert(frame);
}

void ac::VideoImageBlend(cv::Mat &frame) {
    if(v_cap.isOpened() == false || blend_set == false)
        return;
    
    cv::Mat vframe,reimage;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        ac_resize(blend_image, reimage, frame.size());
        auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
            for(int z = offset; z <  offset+size; ++z) {
                for(int i = 0; i < cols; ++i) {
                    cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                    cv::Vec3b pix[3];
                    pix[0] = reframe.at<cv::Vec3b>(z, i);
                    pix[1] = reimage.at<cv::Vec3b>(z, i);
                    for(int j = 0; j < 3; ++j)  {
                        pixel[j] = static_cast<unsigned char>(((pixel[j]^pix[0][j]^pix[1][j])) * 0.8);
                    }
                }
            }
        };
        UseMultipleThreads(frame, getThreadCount(), callback);
        AddInvert(frame);
    }
}

