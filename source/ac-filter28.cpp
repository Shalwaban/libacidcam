

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

void ac::SelfScaleRefined(cv::Mat &frame) {
    static constexpr int SCALE_DEPTH=32;
    static double alpha = 1.0;
    static int dir = 1;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    int total_value = pixel[j] * alpha;
                    pixel[j] = static_cast<unsigned char>((0.25 * total_value) + (0.75 * pixel[j]));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
    AlphaMovementMaxMin(alpha, dir, 0.1, 255/SCALE_DEPTH, 1.0);
}

void ac::MetalMedianBlend(cv::Mat &frame) {
    SelfScaleRefined(frame);
    MedianBlendMultiThread(frame);
}

void ac::SelfScaleRefinedRGB(cv::Mat &frame) {
    static int rand_value = rand()%3;
    static double alpha[3] = {1.0, 255/20, 1.0};
    static int dir[3] = {1, 0, 1};
    static int lazy_init = 0;
    if(lazy_init == 0) {
        lazy_init = 1;
        switch(rand_value) {
            case 0:
                alpha[0] = 1.0;
                alpha[1] = 1.0;
                alpha[2] = 1.0;
                dir[0] = 1;
                dir[1] = 1;
                dir[2] = 1;
                break;
            case 1:
                alpha[0] = 1.0;
                alpha[1] = 255/20;
                alpha[2] = 1.0;
                dir[0] = 1;
                dir[1] = 0;
                dir[2] = 1;
                break;
            case 2:
                alpha[0] = 1.0;
                alpha[1] = 1.0;
                alpha[2] = 255/20;
                dir[0] = 1;
                dir[1] = 1;
                dir[2] = 0;
                break;
        }
    }
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    int total_value = pixel[j] * alpha[j];
                    pixel[j] = static_cast<unsigned char>((0.5 * total_value) + (0.5 * pixel[j]));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
    for(int j = 0; j < 3; ++j)
        AlphaMovementMaxMin(alpha[j], dir[j], 0.1, 255/20, 1.0);
}

void ac::ImageFreezeReleaseRepeat(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static PixelArray2D pix_container;
    static int pix_x = 0, pix_y = 0;
    bool reset = false;
    static int frame_counter = 0;
    if(++frame_counter > static_cast<int>(ac::fps))  {
        frame_counter = 0;
        reset = true;
    }
    if(reset == true || image_matrix_reset == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, 0);
        pix_x = frame.cols;
        pix_y = frame.rows;
    }
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    ColorPulseIncrease(reimage);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                PixelValues &p = pix_container.pix_values[i][z];
                cv::Vec3b img_pix = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    if(p.dir[j] == 1) {
                        ++p.col[j];
                        if(p.col[j] >= 255) {
                            p.dir[j] = 0;
                        }
                    } else {
                        --p.col[j];
                        if(p.col[j] <= 1) {
                            p.dir[j] = 1;
                        }
                    }
                    pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * img_pix[j]));
                    pixel[j] = pixel[j]^p.col[j];
                }
            }
        }
    };
    
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::ImageReplaceColorIntensity(cv::Mat &frame) {
    if(blend_set == false)
        return;
    
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    Smooth(frame, &collection, false);
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    ColorTransition(reimage);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b pix = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    if(pixel[j] > 5 && pixel[j] < 50) {
                        continue;
                    }
                    else if(pixel[j] >= 50 && pixel[j] < 100) {
                        pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                    }
                    else if(pixel[j] >= 100) {
                        pixel[j] =  pix[j];
                    }
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}
