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


void ac::HalfNegateStrobe(cv::Mat &frame) {
    int f_len = frame.cols/2;
    auto neg = [](cv::Vec3b &pixel) {
        for(int j = 0; j < 3; ++j)
            pixel[j] = ~pixel[j];
    };
    static int off = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < f_len; ++i) {
            if(off == 0) {
            	cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                neg(pixel);
            }
            
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
        for(int i = f_len; i < frame.cols; ++i) {
            if(off == 1) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                neg(pixel);
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
    ++off;
    if(off >= 2) {
        off = 0;
    }
}

void ac::MedianBlurXor(cv::Mat &frame) {
    cv::Mat copy_f = frame.clone();
    MedianBlend(frame);
    Negate(frame);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b v = copy_f.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = pixel[j] ^ v[j];
            }
        }
    }
    Negate(frame);
    BlendWithSource(frame);
}

void ac::NegateTrails(cv::Mat &frame) {
    MovementRGBTrails(frame);
    MedianBlur(frame);
    RGBTrailsDark(frame);
    MedianBlur(frame);
    Negate(frame);
}

void ac::RandomGradient(cv::Mat &frame) {
static std::vector<std::string> svGradient { "CosSinMultiply","New Blend","Color Accumlate1", "Color Accumulate2", "Color Accumulate3", "Filter8", "Graident Rainbow","Gradient Rainbow Flash","Outward", "Outward Square","GradientLines","GradientSelf","GradientSelfVertical","GradientDown","GraidentHorizontal","GradientRGB","GradientStripes", "GradientReverse", "GradientReverseBox", "GradientReverseVertical", "GradientNewFilter", "AverageLines", "QuadCosSinMultiply", "GradientColors", "GradientColorsVertical", "GradientXorSelfScale", "GradientLeftRight", "GraidentUpDown", "GradientLeftRightInOut", "GradientUpDownInOut"};
    CallFilter(svGradient[rand()%svGradient.size()], frame);
}
