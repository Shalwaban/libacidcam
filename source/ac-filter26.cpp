
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


void ac::EachFilterSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "EachFilterSubFilter")
        return;
    static int filter_num = 0;
    static int frame_count = 0, seconds = 0;
    if (++frame_count > static_cast<int>(fps)) {
        frame_count = 0;
        ++seconds;
    }
    if(seconds > 4) {
        frame_count = 0;
        seconds = 0;
        if(filter_num > ac::getFilterCount()-2) {
            filter_num = 0;
        } else {
            ++filter_num;
        }
        ac::release_all_objects();
    }
    std::string filter_name = ac::draw_strings[filter_num];
    if(filter_name == "EachFilterSubFilter") {
        if(filter_num < ac::getFilterCount()-2)
            filter_name = ac::draw_strings[++filter_num];
        else {
            filter_num = 0;
            filter_name = ac::draw_strings[filter_num];
        }
    }
    //std::cout << filter_name << ": " << filter_num << "/" << getFilterCount()-2 << "\n";
    pushSubFilter(subfilter);
    CallFilter(filter_name, frame);
    popSubFilter();
}

void ac::EachFilterRandomStartSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "EachFilterRandomStartSubFilter")
        return;
    static int filter_num = rand()%getFilterCount()-3;
    static int frame_count = 0, seconds = 0;
    if (++frame_count > static_cast<int>(fps)) {
        frame_count = 0;
        ++seconds;
    }
    if(seconds > 4) {
        frame_count = 0;
        seconds = 0;
        if(filter_num > ac::getFilterCount()-2) {
            filter_num = 0;
        } else {
            ++filter_num;
            std::cout << ac::draw_strings[filter_num] << ": " << filter_num << "/" << getFilterCount()-2 << "\n";
        }
        //        ac::release_all_objects();
    }
    std::string filter_name = ac::draw_strings[filter_num];
    if(filter_name == "EachFilterRandomStartSubFilter") {
        if(filter_num < ac::getFilterCount()-2)
            filter_name = ac::draw_strings[++filter_num];
        else {
            filter_num = 0;
            filter_name = ac::draw_strings[filter_num];
        }
    }
    //std::cout << filter_name << ": " << filter_num << "/" << getFilterCount()-2 << "\n";
    pushSubFilter(subfilter);
    CallFilter(filter_name, frame);
    popSubFilter();
}

void ac::PixelPsychosis(cv::Mat &frame) {
    static PixelArray2D pix_container;
    static int pix_x = 0, pix_y = 0;
    if(image_matrix_reset == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, 0);
        pix_x = frame.cols;
        pix_y = frame.rows;
    }
    static int speed = 50;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                PixelValues &p = pix_container.pix_values[i][z];
                for(int j = 0; j < 3; ++j) {
                    if(p.dir[j] == 1) {
                        p.add[j] = p.col[j];
                        p.col[j] += speed;
                        if(p.col[j] >= 225) {
                            p.dir[j] = 0;
                            p.col[j] = pixel[j];
                        }
                    } else {
                        p.col[j] -= speed;
                        p.add[j] = p.col[j];
                        if(p.col[j] <= 25) {
                            p.dir[j] = 1;
                            p.col[j] = pixel[j];
                        }
                    }
                    int val = p.col[j]^p.add[j];
                    pixel[j] = static_cast<unsigned char>((pixel[j] * 0.5) + (val * 0.5));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    BlendWithSource25(frame);
    MedianBlendMultiThread(frame);
    AddInvert(frame);
}

void ac::PixelCloud(cv::Mat &frame) {
    static PixelArray2D pix_container;
    static int pix_x = 0, pix_y = 0;
    if(image_matrix_reset == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, 0);
        pix_x = frame.cols;
        pix_y = frame.rows;
    }
    static const int speed = 5;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                PixelValues &p = pix_container.pix_values[i][z];
                for(int j = 0; j < 3; ++j) {
                    if(p.dir[j] == 1) {
                        p.add[j] = p.col[j];
                        p.col[j] += speed;
                        if(p.col[j] >= 255) {
                            p.dir[j] = 0;
                            p.col[j] = pixel[j];
                        }
                    } else {
                        p.col[j] -= speed;
                        p.add[j] = p.col[j];
                        if(p.col[j] <= 25) {
                            p.dir[j] = 1;
                            p.col[j] = pixel[j];
                        }
                    }
                    pixel[j] = static_cast<unsigned char>((pixel[j] * 0.33) + (p.col[j]) * 0.33) + (p.add[j] * 0.33);
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    MedianBlendMultiThread(frame);
    AddInvert(frame);
}

void ac::PiixelXorBlendFrame(cv::Mat &frame) {
    static PixelArray2D pix_container;
    static MatrixCollection<8> collection;
    cv::Mat copy1 = frame.clone();
    Smooth(copy1, &collection);
    static int pix_x = 0, pix_y = 0;
    if(image_matrix_reset == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, 0);
        pix_x = frame.cols;
        pix_y = frame.rows;
    }
    pix_container.insert(copy1);
    static const int speed = 5;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                PixelValues &p = pix_container.pix_values[i][z];
                for(int j = 0; j < 3; ++j) {
                    if(p.dir[j] == 1) {
                        p.add[j] += speed;
                        if(p.add[j] >= 255) {
                            p.dir[j] = 0;
                        }
                    } else {
                        p.add[j] -= speed;
                        if(p.add[j] <= 0) {
                            p.dir[j] = 1;
                        }
                    }
                    int val = p.col[j]^p.add[j];
                    pixel[j] = static_cast<unsigned char>((0.33 * pixel[j]) + (0.33 * val) + (0.33 * p.col[j]));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::PixelImageBlendFrame(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static PixelArray2D pix_container;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    
    static int pix_x = 0, pix_y = 0;
    if(image_matrix_reset == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, 0);
        pix_x = frame.cols;
        pix_y = frame.rows;
    }
    static const int speed = 1;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                PixelValues &p = pix_container.pix_values[i][z];
                cv::Vec3b &ipix = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    if(p.dir[i] == 1) {
                        p.col[j] += speed;
                        if(p.col[j] == ipix[j]) {
                            p.add[j] = p.col[j];
                            p.col[j] = ipix[j];
                        } else {
                            if(p.col[j] >= 255) {
                                p.dir[j] = 0;
                            }
                        }
                    } else {
                        if(p.col[j] == ipix[j]) {
                            p.add[j] = p.col[j];
                            p.col[j] = ipix[j];
                        } else {
                            p.col[j] -= speed;
                            if(p.col[j] <= 1) {
                                p.dir[j] = 1;
                            }
                        }
                    }
                    int val = p.col[j] ^ p.add[j];
                    pixel[j] = static_cast<unsigned char>((0.25 * pixel[j]) + (0.25 * val) +  (0.50 * ipix[j]));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    ColorTransition(frame);
    MedianBlendMultiThread(frame);
    AddInvert(frame);
}

void ac::PixelCollectionRandom(cv::Mat &frame) {
    static constexpr int MAX = 8;
    static MatrixCollection<MAX> collection;
    collection.shiftFrames(frame);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    cv::Vec3b value = collection.frames[rand()%MAX].template at<cv::Vec3b>(z, i);
                    pixel[j] = value[j];
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::PixelCollectionSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "PixelCollectionSubFilter")
        return;
    static constexpr int MAX = 8;
    static MatrixCollection<MAX> collection;
    collection.shiftFrames(frame);
    cv::Mat copyf = frame.clone();
    CallFilter(subfilter, copyf);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b value = collection.frames[rand()%MAX].template at<cv::Vec3b>(z, i);
                cv::Vec3b sub = copyf.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    int off = pixel[j]-value[j];
                    if(!(abs(off) <= 55)) {
                        pixel[j] = sub[j];
                    }
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::PixelCollectionMatrixSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "PixelCollectionMatrixSubFilter")
        return;
    static constexpr int MAX = 8;
    static MatrixCollection<MAX> collection;
    collection.shiftFrames(frame);
    cv::Mat copyf = frame.clone();
    CallFilter(subfilter, copyf);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int v = 1; v < collection.size() && !collection.frames[v].empty(); ++v) {
                    cv::Vec3b value = collection.frames[v].at<cv::Vec3b>(z, i);
                    cv::Vec3b sub = copyf.at<cv::Vec3b>(z, i);
                    for(int j = 0; j < 3; ++j) {
                        int off = pixel[j]-value[j];
                        if(!(abs(off) <= 55)) {
                            pixel[j] = sub[j];
                        }
                    }
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::PixelCollectionIncreaseSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "PixelCollectionIncreaseSubFilter")
        return;
    static constexpr int MAX = 8;
    static double increase = 5;
    static int dir = 1;
    static MatrixCollection<MAX> collection;
    collection.shiftFrames(frame);
    cv::Mat copyf = frame.clone();
    CallFilter(subfilter, copyf);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int v = 1; v < collection.size() && !collection.frames[v].empty(); ++v) {
                    cv::Vec3b value = collection.frames[v].at<cv::Vec3b>(z, i);
                    cv::Vec3b sub = copyf.at<cv::Vec3b>(z, i);
                    for(int j = 0; j < 3; ++j) {
                        int off = pixel[j]-value[j];
                        if(!(abs(off) <= static_cast<int>(increase))) {
                            pixel[j] = sub[j];
                        }
                    }
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AlphaMovementMaxMin(increase, dir, 6.0, 70.0, 6.0);
    AddInvert(frame);
}

void ac::PixelCollectionImageBlend(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static constexpr int MAX = 32;
    static MatrixCollection<MAX> collection;
    collection.shiftFrames(frame);
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    cv::Mat orig = frame.clone();
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int v = 1; v < collection.size() && !collection.frames[v].empty(); ++v) {
                    cv::Vec3b value = collection.frames[v].at<cv::Vec3b>(z, i);
                    cv::Vec3b source = reimage.at<cv::Vec3b>(z, i);
                    cv::Vec3b sub = reimage.template at<cv::Vec3b>(z, i);
                    for(int j = 0; j < 3; ++j) {
                        int off = pixel[j]-value[j];
                        if(!(abs(off) <= 55)) {
                            pixel[j] = sub[j];
                            pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * source[j]));

                        }
                    }
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::ColorPulseIncrease(cv::Mat &frame) {
    static double alpha[3] = {0};
    static double start[3] = {0, 0, 0}, start_init[3] = {5, 5, 5}, start_max[3] = {25, 10, 5};
    static double stop[3] = {0,0,0}, stop_init[3] = {5, 5, 5}, stop_max[3] = {200, 210, 180};
    static double inc = 4.0;
    static int dir[3] = {1, 0, 1};
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] =  static_cast<unsigned char>((pixel[j] * 0.5) + ((alpha[j] * 0.5)));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    VariableScale(alpha,dir,start, start_init, start_max, stop, stop_init, stop_max, inc);
    AddInvert(frame);
}

void ac::PixelCollectionLongMatrixSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "PixelCollectionLongMatrixSubFilter")
        return;
    static constexpr int MAX = 32;
    static MatrixCollection<MAX> collection;
    collection.shiftFrames(frame);
    cv::Mat copyf = frame.clone();
    CallFilter(subfilter, copyf);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int v = 1; v < collection.size() && !collection.frames[v].empty(); ++v) {
                    cv::Vec3b value = collection.frames[v].at<cv::Vec3b>(z, i);
                    cv::Vec3b sub = copyf.at<cv::Vec3b>(z, i);
                    for(int j = 0; j < 3; ++j) {
                        int off = pixel[j]-value[j];
                        if(!(abs(off) <= 55)) {
                            pixel[j] = sub[j];
                        }
                    }
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::ColorPulseImageIncreaseSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "ColorPulseImageIncreaseSubFilter")
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    CallFilter(subfilter, reimage);
    static double alpha[3] = {0};
    static double start[3] = {0, 0, 0}, start_init[3] = {5, 5, 5}, start_max[3] = {25, 10, 5};
    static double stop[3] = {0,0,0}, stop_init[3] = {5, 5, 5}, stop_max[3] = {200, 210, 180};
    static double inc = 4.0;
    static int dir[3] = {1, 0, 1};
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b pix = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] =  static_cast<unsigned char>((pixel[j] * 0.33) + ((alpha[j] * 0.33)) + (0.33 * pix[j]));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    VariableScale(alpha,dir,start, start_init, start_max, stop, stop_init, stop_max, inc);
    AddInvert(frame);
}

void ac::ColorPulseRandom(cv::Mat &frame) {
    static double alpha[3] = {0};
    static double start[3] = {0,0,0}, start_init[3] = {static_cast<double>(rand()%5), static_cast<double>(rand()%5), static_cast<double>(rand()%5)}, start_max[3] = {static_cast<double>(rand()%25), static_cast<double>(rand()%10), static_cast<double>(rand()%5)};
    static double stop[3] = {0,0,0}, stop_init[3] = {static_cast<double>(rand()%5), static_cast<double>(rand()%5), static_cast<double>(rand()%5)}, stop_max[3] = {static_cast<double>(rand()%200), static_cast<double>(rand()%210), static_cast<double>(rand()%180)};
    static double inc = static_cast<double>(rand()%4)+4.0;
    static int dir[3] = {1, 0, 1};
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] =  static_cast<unsigned char>((pixel[j] * 0.5) + ((alpha[j] * 0.5)));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    VariableScale(alpha,dir,start, start_init, start_max, stop, stop_init, stop_max, inc);
    AddInvert(frame);
}

void ac::ColorPulseImage(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    static double scale = 1.0;
    static int dir1 = 1;
    static double alpha[3] = {0};
    static double start[3] = {0, 0, 0}, start_init[3] = {5, 5, 5}, start_max[3] = {25, 10, 5};
    static double stop[3] = {0,0,0}, stop_init[3] = {5, 5, 5}, stop_max[3] = {200, 210, 180};
    static double inc = 4.0;
    static int dir[3] = {1, 0, 1};
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b img = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] =  static_cast<unsigned char>((pixel[j] * 0.33) + ((alpha[j] * 0.33)) + (0.33 * (scale * img[j])));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    VariableScale(alpha,dir,start, start_init, start_max, stop, stop_init, stop_max, inc);
    AlphaMovementMaxMin(scale, dir1, 0.01,5.0, 0.1);
    AddInvert(frame);
}

void ac::ColorPulseAlpha(cv::Mat &frame) {
    static double alpha[3] = {0};
    static double start[3] = {0, 0, 0}, start_init[3] = {5, 5, 5}, start_max[3] = {25, 10, 5};
    static double stop[3] = {0,0,0}, stop_init[3] = {5, 5, 5}, stop_max[3] = {200, 210, 180};
    static double inc = 4.0;
    static int dir[3] = {1, 0, 1};
    static int index = 0;
    int colors[3] = {0};
    static double scale = 1.0;
    static int dir1 = 1;
    InitArrayPosition(colors,index);
    ++index;
    if(index > 2)
        index = 0;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[colors[j]] =  static_cast<unsigned char>((pixel[j] * scale) + ((alpha[j] * 1-scale)));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    VariableScale(alpha,dir,start, start_init, start_max, stop, stop_init, stop_max, inc);
    AlphaMovementMaxMin(scale, dir1, 0.01, 1.0, 0.1);
    AddInvert(frame);
}

void ac::ColorLower75(cv::Mat &frame) {
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>(0.75 * pixel[j]);
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::ColorImageMedianBlend(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat frames[3];
    frames[0] = collection.frames[1].clone();
    frames[1] = collection.frames[3].clone();
    frames[2] = collection.frames[7].clone();
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b cpix[3];
                cpix[0] = frames[0].at<cv::Vec3b>(z, i);
                cpix[1] = frames[1].at<cv::Vec3b>(z, i);
                cpix[2] = frames[2].at<cv::Vec3b>(z, i);
                cv::Vec3b img;
                img = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((pixel[j] * 0.33) + (cpix[j][j] * 0.33) + (img[j] * 0.33));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    MedianBlendMultiThread(frame);
    AddInvert(frame);
}

void ac::ColorDullBlur(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    cv::Mat copy1 = frame.clone();
    Smooth(copy1, &collection);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b pix = copy1.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((0.3 * pixel[j]) + (0.7 * pix[j]));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::ColorImageDull(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    static MatrixCollection<8> collection;
    cv::Mat copy1 = frame.clone();
    Smooth(copy1, &collection);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b pix = reimage.at<cv::Vec3b>(z, i);
                cv::Vec3b cpix = copy1.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((pixel[j] * 0.25) + (pix[j] * 0.25) + (cpix[j] * 0.5));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}
