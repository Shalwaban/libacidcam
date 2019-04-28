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

#include "ac.h"

void ac::resizeImageWidth(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static cv::Size cur = frame.size();
    static int resize_x = 0, resize_y = 0;
    static int dir = 1;
    if(resize_x == 0 || resize_y == 0 || cur != frame.size()) {
        resize_x = frame.cols;
        resize_y = frame.rows;
        cur = frame.size();
    }
    cv::Mat copy1;
    cv::resize(blend_image, copy1, cv::Size(resize_x,resize_y));
    
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            int x = (resize_x-(frame.cols))+i;
            int y = (resize_y-(frame.rows))+z;
            ASSERT(x >= 0 && y >= 0 && x < copy1.cols && y < copy1.rows);
            cv::Vec3b pix = copy1.at<cv::Vec3b>(y, x);
            pixel = pix;
        }
    }
    if(dir == 1) {
        resize_x += 50;
        if(resize_x > (frame.cols*6)) {
            dir = 0;
        }
    } else {
        resize_x -= 50;
        if(resize_x <= frame.cols) {
            resize_x = frame.cols;
            resize_y = frame.rows;
            dir = 1;
        }
    }
    AddInvert(frame);
}

void ac::resizeImageHeight(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static cv::Size cur = frame.size();
    static int resize_x = 0, resize_y = 0;
    static int dir = 1;
    if(resize_x == 0 || resize_y == 0 || cur != frame.size()) {
        resize_x = frame.cols;
        resize_y = frame.rows;
        cur = frame.size();
    }
    cv::Mat copy1;
    cv::resize(blend_image, copy1, cv::Size(resize_x,resize_y));
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            int x = (resize_x-(frame.cols))+i;
            int y = (resize_y-(frame.rows))+z;
            ASSERT(x >= 0 && y >= 0 && x < copy1.cols && y < copy1.rows);
            cv::Vec3b pix = copy1.at<cv::Vec3b>(y, x);
            pixel = pix;
        }
    }
    if(dir == 1) {
        resize_y += 50;
        if(resize_y > (frame.rows*6)) {
            dir = 0;
        }
    } else {
        resize_y -= 50;
        if(resize_y <= frame.rows) {
            resize_x = frame.cols;
            resize_y = frame.rows;
            dir = 1;
        }
    }
    AddInvert(frame);
}

void ac::resizeImageWidthAndHeight(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static cv::Size cur = frame.size();
    static int resize_x = 0, resize_y = 0;
    static int dir = 1;
    if(resize_x == 0 || resize_y == 0 || cur != frame.size()) {
        resize_x = frame.cols;
        resize_y = frame.rows;
        cur = frame.size();
    }
    cv::Mat copy1;
    cv::resize(blend_image, copy1, cv::Size(resize_x,resize_y));
    
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            int x = (resize_x-(frame.cols))+i;
            int y = (resize_y-(frame.rows))+z;
            ASSERT(x >= 0 && y >= 0 && x < copy1.cols && y < copy1.rows);
            cv::Vec3b pix = copy1.at<cv::Vec3b>(y, x);
            pixel = pix;
        }
    }
    if(dir == 1) {
        resize_x += 50;
        resize_y += 50;
        if(resize_x >  (frame.cols*6) || resize_y > (frame.rows*6)) {
            dir = 0;
        }
    } else {
        resize_x -= 50;
        resize_y -= 50;
        if(resize_x <= frame.cols || resize_y <= frame.rows) {
            resize_x = frame.cols;
            resize_y = frame.rows;
            dir = 1;
        }
    }
    AddInvert(frame);
}

void ac::resizeImageAndFrameBlend(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat copy1 = frame.clone(), reimage;
    cv::resize(blend_image, reimage, frame.size());
    resizeImageWidthAndHeight(reimage);
    resizeFrameWidthAndHeight(copy1);
    AlphaBlend(copy1, reimage, frame, 0.5);
    AddInvert(frame);
}

void ac::resizeImageWidthAndHeightSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "resizeImageWidthAndHeightSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), reimage, copy3;
    cv::resize(blend_image, reimage, frame.size());
    resizeImageWidthAndHeight(reimage);
    CallFilter(subfilter, reimage);
    resizeFrameWidthAndHeight(copy1);
    AlphaBlendDouble(copy1,reimage,copy3, 0.3, 0.7);
    AlphaBlend(copy3, orig_frame, frame, 0.5);
    AddInvert(frame);
}

void ac::resizeImageFrameWidth(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat copy1 = frame.clone(), reimage;
    cv::resize(blend_image, reimage, frame.size());
    resizeImageWidth(reimage);
    AlphaBlend(copy1, reimage, frame, 0.5);
    AddInvert(frame);
}

void ac::resizeImageFrameHeight(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat copy1 = frame.clone(), reimage;
    cv::resize(blend_image, reimage, frame.size());
    resizeImageHeight(reimage);
    AlphaBlend(copy1, reimage, frame, 0.5);
    AddInvert(frame);
}

void ac::resizeImageFrameWidthAndHeight(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat copy1 = frame.clone(), reimage;
    cv::resize(blend_image, reimage, frame.size());
    resizeImageWidthAndHeight(reimage);
    AlphaBlend(copy1, reimage, frame, 0.5);
    AddInvert(frame);
}

void ac::resizeImageRandom(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static int index = 0, fcount = 0;
    switch(index) {
        case 0:
            resizeImageFrameWidth(frame);
            break;
        case 1:
            resizeImageFrameHeight(frame);
            break;
        case 2:
            resizeImageFrameWidthAndHeight(frame);
            break;
    }
    AddInvert(frame);
    ++fcount;
    int fps = static_cast<int>(ac::fps);
    if((fcount%fps) == 0) {
        ++index;
        if(index > 2)
            index = 0;
    }
    AddInvert(frame);
}

void ac::resizeFrameRandom(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    static int index = 0, fcount = 0;
    switch(index) {
        case 0:
            resizeFrameWidth(copy1);
            break;
        case 1:
            resizeFrameHeight(copy1);
            break;
        case 2:
            resizeFrameWidthAndHeight(copy1);
            break;
    }
    AddInvert(copy1);
    ++fcount;
    int fps = static_cast<int>(ac::fps);
    if((fcount%fps) == 0) {
        ++index;
        if(index > 2)
            index = 0;
    }
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}

void ac::resizeFrameImageFlash(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    int offset = rand()%9;
    switch(offset) {
        case 0:
            resizeFrameWidth(copy1);
            break;
        case 1:
            resizeFrameHeight(copy1);
            break;
        case 2:
            resizeFrameWidthAndHeight(copy1);
            break;
        case 3:
            resizeImageFrameWidth(copy1);
            break;
        case 4:
            resizeImageFrameHeight(copy1);
            break;
        case 5:
            resizeImageFrameWidthAndHeight(copy1);
            break;
        case 6:
            resizeImageWidth(copy1);
            break;
        case 7:
            resizeImageWidth(copy1);
            break;
        case 8:
            resizeImageWidthAndHeight(copy1);
            break;
            
    }
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}

void ac::RandomFlip(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    int rt_val = -1;
    rt_val += rand()%4;
    if(rt_val >= 2) return;
    cv::flip(copy1, frame, rt_val);
    AddInvert(frame);
}

void ac::ColorsFilter(cv::Mat &frame) {
    static std::vector<std::string> filter_array({"ColorExpand", "ColorRange", "ColorTransition","ColorTransitionRandom","ColorXorScale", "Rainbow Blend", "RainbowXorBlend", "ColorChannelMoveUpAndDown"});
    static auto rng = std::default_random_engine{};
    static unsigned int counter = 0;
    if(counter == 0) {
        std::shuffle(filter_array.begin(), filter_array.end(),rng);
    }
    int fps_val = static_cast<unsigned int>(ac::fps);
    static int index = 0;
    CallFilter(filter_array[index], frame);
    ++counter;
    if((counter%fps_val) == 0) {
        ++index;
        if(index > filter_array.size()-1) {
            index = 0;
            std::shuffle(filter_array.begin(), filter_array.end(),rng);
        }
    }
    AddInvert(frame);
}

void ac::MirrorDoubleVision(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    MirrorLeft(copy1);
    MirrorRight(copy2);
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}

void ac::ColorFadeFilter(cv::Mat &frame) {
    static int rgb_start[] = {rand()%255,rand()%255,rand()%255};
    static int speed = 5;
    static int dir[3] = {1, 0, 1};
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] += rgb_start[j];
            }
        }
    }
    for(int j = 0; j < 3; ++j) {
        if(dir[j] == 1) {
            rgb_start[j] += speed;
            if(rgb_start[j] >= 255) {
                dir[j] = 0;
                rgb_start[j] = rand()%255;
            }
        } else if(dir[j] == 0) {
            rgb_start[j] -= speed;
            if(rgb_start[j] <= 0) {
                dir[j] = 1;
            }
        }
    }
    AddInvert(frame);
}

void ac::ColorChannelMoveUpAndDown(cv::Mat &frame) {
    int rgb_values[3] = {rand()%2255, rand()%255, rand()%255};
    int rgb_stop[3] = {rand()%255, rand()%255, rand()%255};
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] += rgb_values[j];
            
        }
    }
    for(int j = 0; j < 3; ++j) {
        if(rgb_values[j] > rgb_stop[j]) {
            --rgb_values[j];
            if(rgb_values[j] == rgb_stop[j]) {
                rgb_stop[j] = rgb_values[j];
                rgb_values[j] = rand()%255;
            }
        } else if(rgb_values[j] < rgb_stop[j]) {
            ++rgb_values[j];
            if(rgb_values[j] == rgb_stop[j]) {
                rgb_stop[j] = rgb_values[j];
                rgb_values[j] = rand()%255;
            }
        }
    }
    AddInvert(frame);
}

void ac::MedianStrobe(cv::Mat &frame) {
    static int index = 0;
    if(index == 0) {
        MedianBlend(frame);
        index = 1;
    } else {
        MedianBlendDark(frame);
        index = 0;
    }
    AddInvert(frame);
}

void ac::DifferenceSubFilter(cv::Mat &frame) {
    if(orig_frame.empty() || orig_frame.size() != frame.size() || subfilter == -1 || draw_strings[subfilter] == "DifferenceSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    CallFilter(subfilter, copy1);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = copy1.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = orig_frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] -= (pix[j]);
        }
    }
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}
