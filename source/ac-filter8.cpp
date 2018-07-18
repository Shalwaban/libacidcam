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

void ac::MoveUpLeft(cv::Mat &frame) {
    
    static double alpha = 1.0, alpha_max = 7.0;
    
    static cv::Mat old_frame = frame.clone();
    static cv::Size old_size = old_frame.size();
    
    if(reset_alpha == true || frames_released == true || old_size != frame.size()) {
        old_frame = frame.clone();
        old_size = old_frame.size();
    }
    for(int z = 0; z < old_frame.rows-1; ++z) {
        for(int i = 0; i < old_frame.cols-1; ++i) {
            cv::Vec3b &pixel = old_frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = old_frame.at<cv::Vec3b>(z+1, i+1);
            pixel = pix;
        }
    }
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = old_frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] = static_cast<unsigned char>((pixel[j]^pix[j])*alpha);
        }
    }
    
    static int dir = 1;
    procPos(dir, alpha, alpha_max);
}

void ac::RandomStrobe(cv::Mat &frame) {
    static double alpha = 1.0, alpha_max = 7.0;
    static std::vector<std::string> svStrobe{  "StrobeEffect", "Blank", "Type","Random Flash","Strobe Red Then Green Then Blue","Flash Black","FlashWhite","StrobeScan", "RGBFlash", "ReinterpretDouble", "DiamondStrobe", "BitwiseXorStrobe","FlashBlackAndWhite", "StrobeBlend", "FibFlash", "ScaleFlash", "FadeStrobe", "AndStrobe", "AndStrobeScale", "AndPixelStrobe", "AndOrXorStrobe", "AndOrXorStrobeScale", "BrightStrobe", "DarkStrobe", "RandomXorOpposite", "StrobeTransform"};

    cv::Mat old_frame = frame.clone();
    DrawFilter(svStrobe[rand()%svStrobe.size()], old_frame);
    cv::Mat copy = frame.clone();
    AlphaBlend(old_frame, copy, frame, alpha);
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 15, 0.1);
}

void ac::RandomBlur(cv::Mat &frame) {
    std::vector<std::string> svBlur { "GaussianBlur", "Median Blur", "Blur Distortion", "ColorTrails","TrailsFilter", "TrailsFilterIntense", "TrailsFilterSelfAlpha", "TrailsFilterXor","BlurSim", "TrailsInter", "TrailsBlend", "TrailsNegate", "AcidTrails", "HorizontalTrailsInter" ,"Trails", "BlendTrails", "SmoothTrails", "SmoothTrailsSelfAlphaBlend", "SmoothTrailsRainbowBlend", "MedianBlend", "XorTrails", "RainbowTrails", "NegativeTrails", "IntenseTrails", "GaussianBlend", "RandomAmountMedianBlur", "MedianBlendAnimation", "AlphaAcidTrails"};
    static double alpha = 1.0, alpha_max = 7.0;
    cv::Mat old_frame = frame.clone();
    DrawFilter(svBlur[rand()%svBlur.size()], old_frame);
    cv::Mat copy = frame.clone();
    AlphaBlend(old_frame, copy, frame, alpha);
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 15, 0.1);
}


void ac::StuckStrobe(cv::Mat &frame) {
    static double alpha = 1.0, alpha_max = 7.0;
    static cv::Mat old_frame = frame.clone();
    if(reset_alpha == true || frames_released == true || old_frame.size() != frame.size()) {
        old_frame = frame.clone();
    }
    cv::Mat copy = frame.clone();
    AlphaBlend(old_frame, copy, frame, alpha);
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 15, 0.1);
}

void ac::Stuck(cv::Mat &frame) {
    static cv::Mat old_frame = frame.clone();
    if(reset_alpha == true || frames_released == true || old_frame.size() != frame.size()) {
        old_frame = frame.clone();
    }
    Add(frame, old_frame);
}

void ac::OrStrobe(cv::Mat &frame) {
    cv::Mat copy = frame.clone();
    static cv::Mat prev_frame = frame.clone();
    if(prev_frame.size() != frame.size()) {
        prev_frame = frame.clone();
    }
    cv::Vec3b randval(rand()%255, rand()%255, rand()%255);
    static double alpha = 1.0, alpha_max = 7.0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = prev_frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((pixel[j]|randval[j]|pix[j])*alpha);
            }
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max);
    prev_frame = copy;
}

void ac::LagBlend(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    static cv::Size c_size = frame.size();
    // lazy init
    static bool init = false;
    if(c_size != frame.size() || init == false) {
        collection.shiftFrames(frame);
        init = true;
        c_size = frame.size();
    }
    static int frame_count = 0;
    ++frame_count;
    if(frame_count > (ac::fps/8)) {
    	collection.shiftFrames(frame);
        frame_count = 0;
    }
    for(int i = 0; i < collection.size(); ++i) {
        DarkenFilter(frame);
        Add(frame, collection.frames[i]);
    }
}
