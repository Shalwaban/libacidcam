

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

void ac::ImageReplaceColorIntensitySubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "ImageReplaceColorIntensitySubFilter")
        return;
    static double alpha = 1.0;
    static int dir = 1;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    CallFilter(subfilter, copy1);
    AlphaBlendDouble(copy1, reimage, copy2, alpha, 1-alpha);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b pix[3];
                pix[0] = copy1.at<cv::Vec3b>(z, i);
                pix[1] = copy2.at<cv::Vec3b>(z, i);
                pix[2] = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    if(pixel[j] < 25) {
                        continue;
                    } else if(pixel[j] >= 25 && pixel[j] < 100) {
                        pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[1][j]));
                        
                    } else if(pixel[j] >= 100 && pixel[j] < 175) {
                        pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[2][j]));
                        
                    } else if(pixel[j] >= 175 && pixel[j] <= 255) {
                        pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j][j]));
                    }
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AlphaMovementMaxMin(alpha, dir, 0.1, 1.0, 0.1);
    AddInvert(frame);
}

void ac::ImageReplaceColorIntensitySubFilterInOut(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "ImageReplaceColorIntensitySubFilterInOut")
        return;
    static double alpha = 1.0;
    static int dir = 1;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    CallFilter(subfilter, copy1);
    auto procPixel = [&](cv::Vec3b &pixel, int index, cv::Vec3b *pix) {
        for(int j = 0; j < 3; ++j) {
            switch(index) {
                case 0:
                    pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[1][j]));
                    break;
                case 1:
                    pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[2][j]));
                    break;
                case 2:
                    pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j][j]));
                    break;
            }
        }
    };
    AlphaBlendDouble(copy1, reimage, copy2, alpha, 1-alpha);
    static int pixel_index = 0;
    int arr[3];
    InitArrayPosition(arr, pixel_index);
    
    ++pixel_index;
    if(pixel_index > 2)
        pixel_index = 0;
    
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b pix[3];
                pix[0] = copy1.at<cv::Vec3b>(z, i);
                pix[1] = copy2.at<cv::Vec3b>(z, i);
                pix[2] = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    if(pixel[j] < 25) {
                        continue;
                    } else if(pixel[j] >= 25 && pixel[j] < 100) {
                        procPixel(pixel, arr[0], pix);
                        
                    } else if(pixel[j] >= 100 && pixel[j] < 175) {
                        procPixel(pixel, arr[1], pix);
                        
                    } else if(pixel[j] >= 175 && pixel[j] <= 255) {
                        procPixel(pixel, arr[2], pix);
                    }
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AlphaMovementMaxMin(alpha, dir, 0.1, 1.0, 0.1);
    AddInvert(frame);
}

void ac::ImageFillColor(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    int col[3];
    static int index = 0;
    InitArrayPosition(col, index);
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    ++index;
    if(index > 2)
        index = 0;
    cv::Mat frames[3];
    frames[0] = collection.frames[1].clone();
    frames[1] = collection.frames[4].clone();
    frames[2] = collection.frames[7].clone();
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b img = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    cv::Vec3b pix = frames[j].at<cv::Vec3b>(z, i);
                    if(pixel[j] <= (255/2)) {
                        pixel[j] = pix[j];
                    } else if(pixel[j] >= 255/2) {
                        pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * img[col[j]]));
                    }
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::MultiRandFilter(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone(), copy3 = frame.clone();
    Random_Filter(copy1);
    Random_Filter(copy2);
    Random_Filter(copy3);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b pix[3];
                pix[0] = copy1.at<cv::Vec3b>(z, i);
                pix[1] = copy2.at<cv::Vec3b>(z, i);
                pix[2] = copy3.at<cv::Vec3b>(z, i);
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    if(pixel[j] > 0 && pixel[j] < 50) {
                        pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[0][j]));
                    } else if(pixel[j] >= 50 && pixel[j] < 100) {
                        pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[1][j]));
                    } else if(pixel[j] >= 100 && pixel[j] < 150) {
                        pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[2][j]));
                    } else {
                        pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j][j]));
                    }
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::MultiFilter(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone(), copy3 = frame.clone();
    ColorTransition(copy1);
    MetalMedianBlend(copy2);
    rainbowBlend(copy3);
    static double alpha[4] = {1.0, 0.1, 0.5, 0.7};
    static int dir[4] {rand()%2, rand()%2,rand()%2,rand()%2};
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b pix[3];
                pix[0] = copy1.at<cv::Vec3b>(z, i);
                pix[1] = copy2.at<cv::Vec3b>(z, i);
                pix[2] = copy3.at<cv::Vec3b>(z, i);
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    if(pixel[j] > 0 && pixel[j] < 50) {
                        pixel[j] = static_cast<unsigned char>((alpha[0] * pixel[j]) + ((1-alpha[0]) * pix[0][j]));
                    } else if(pixel[j] >= 50 && pixel[j] < 100) {
                        pixel[j] = static_cast<unsigned char>(((1-alpha[1]) * pixel[j]) + (alpha[1] * pix[1][j]));
                    } else if(pixel[j] >= 100 && pixel[j] < 150) {
                        pixel[j] = static_cast<unsigned char>((alpha[2] * pixel[j]) + ((1-alpha[2]) * pix[2][j]));
                    } else {
                        pixel[j] = static_cast<unsigned char>((alpha[3] * pixel[j]) + ((1-alpha[3]) * pix[j][j]));
                    }
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    for(int i = 0; i < 4; ++i)
        AlphaMovementMaxMin(alpha[i], dir[i], 0.1, 1.0, 0.1);
    AddInvert(frame);
}

void ac::GradientRandom(cv::Mat &frame) {
    static int index = 0;
    static unsigned char val = 0;
    static double alpha = 1.0;
    static int dir = 1;
    int inc = (frame.rows/255)+1;
    for(int i = 0; i < frame.cols; ++i) {
        val = 1;
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            pixel[index] = static_cast<unsigned char>((alpha * pixel[index]) + (0.5 * val));
            if((z%inc) == 0)
                ++val;}
    }
    ++index;
    if(index > 2)
        index = 0;
    AlphaMovementMaxMin(alpha, dir, 0.1, 1.0, 0.1);
}

void ac::LineMedianBlend(cv::Mat &frame) {
    static int index = 0;
    static unsigned char val = 0;
    int inc = (frame.rows/255)+1;
    for(int i = 0; i < frame.cols; ++i) {
        val = rand()%255;
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            pixel[index] = static_cast<unsigned char>((0.5 * pixel[index]) + (0.5 * val));
            if((z%inc) == 0)
                ++val;}
    }
    ++index;
    if(index > 2)
        index = 0;
    MedianBlendMultiThread(frame);
}

void ac::PerfectMedianBlend(cv::Mat &frame) {
    static int index = 0;
    static unsigned char val = 0;
    static double alpha = 1.0;
    static int dir = 1;
    int inc = (frame.rows/255)+1;
    for(int i = 0; i < frame.cols; ++i) {
        val = 1;
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            pixel[index] = static_cast<unsigned char>((alpha * pixel[index]) + ((1-alpha) * val));
            if((z%inc) == 0)
                ++val;
        }
    }
    ++index;
    if(index > 2)
        index = 0;
    AlphaMovementMaxMin(alpha, dir, 0.1, 1.0, 0.1);
    MedianBlendMultiThread(frame);
}

void ac::ImageRowAlphaSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "ImageRowAlphaSubFilter")
        return;
    static int index = 0;
    static unsigned char val = 0;
    static double alpha = 1.0;
    static int dir = 1;
    cv::Mat copy1 = frame.clone();
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    CallFilter(subfilter, copy1);
    static int pos = 0;
    pos = (pos == 0) ? 1 : 0;
    for(int i = 0; i < frame.cols; ++i) {
        val = 1;
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix[2];
            pix[0] = reimage.at<cv::Vec3b>(z, i);
            pix[1] = copy1.at<cv::Vec3b>(z, i);
            pixel[index] = static_cast<unsigned char>((alpha * ((pixel[index] * 0.25) + (0.25 * pix[pos][index]))) + ((1-alpha) * ((0.5 * pix[pos][index]))));
        }
    }
    ++index;
    if(index > 2)
        index = 0;
    AlphaMovementMaxMin(alpha, dir, 0.1, 1.0, 0.1);
    MedianBlendMultiThread(frame);
}

void ac::IndexPixelRowSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "IndexPixelRowSubFilter")
        return;
    static int index = 0;
    cv::Mat copy1 = frame.clone();
    CallFilter(subfilter, copy1);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b src = copy1.at<cv::Vec3b>(z, i);
            src[index] = pixel[index];
            pixel = src;
        }
    }
    ++index;
    if(index > 2)
        index = 0;
}

void ac::IndexSourceRowSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "IndexPixelRowSubFilter")
        return;
    static int index = 0;
    cv::Mat copy1 = frame.clone();
    CallFilter(subfilter, copy1);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b src = copy1.at<cv::Vec3b>(z, i);
            pixel[index] = src[index];
        }
    }
    ++index;
    if(index > 2)
        index = 0;
}
