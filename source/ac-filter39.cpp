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

void ac::Square_Block_Resize_Vert_Video(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        static MatrixCollection<32> collection;
        collection.shiftFrames(frame);
        collection.shiftFrames(reframe);
        static int square_size = 4, square_dir = 1;
        static int index = 0;
        static int dir = 1;
        for(int z = 0; z < frame.rows; z += square_size) {
            for(int i = 0; i < frame.cols; i += square_size) {
                for(int y = 0; y < square_size; ++y) {
                    for(int x = 0; x < square_size; ++x) {
                        if(z+y < (frame.rows-1) && i+x < (frame.cols-1)) {
                            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z+y, i+x);
                            cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z+y, i+x);
                            for(int j = 0; j < 3; ++j) {
                                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                            }
                        }
                    }
                }
            }
            if(dir == 1) {
                ++index;
                if(index > (collection.size()-1)) {
                    index = collection.size()-1;
                    dir = 0;
                }
            } else {
                --index;
                if(index <= 0) {
                    index = 0;
                    dir = 1;
                }
            }
        }
        if(square_dir == 1) {
            square_size += 2;
            if(square_size >= 64) {
                square_size = 64;
                square_dir = 0;
            }
        } else {
            square_size -= 2;
            if(square_size <= 2) {
                square_size = 2;
                square_dir = 1;
            }
        }
    }
    AddInvert(frame);
}


void ac::Square_Block_Resize_Dir(cv::Mat &frame) {
    Square_Block_Resize(frame);
    Square_Block_Resize_Vertical(frame);
    AddInvert(frame);
}

void ac::Square_Block_Resize_All(cv::Mat &frame) {
    Square_Blocks(frame);
    Square_Block_Resize(frame);
    Square_Block_Resize_Vertical(frame);
    Square_Block_Resize_Reset(frame);
    Square_Block_Resize_Vert_Reset(frame);
    AddInvert(frame);
}

void ac::VideoBlendGradient(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        static MatrixCollection<8> collection;
        collection.shiftFrames(frame);
        collection.shiftFrames(reframe);
        Smooth(frame, &collection, false);
        GradientColorBlend(frame);
        GradientColorBlendAll(frame);
        MedianBlendMultiThread(frame);
    }
    AddInvert(frame);
}

void ac::VideoMatrixBlend(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        static MatrixCollection<8> collection1, collection2;
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        collection1.shiftFrames(frame);
        collection2.shiftFrames(reframe);
        cv::Mat frame1[3], frame2[3];
        frame1[0] = collection1.frames[0].clone();
        frame1[1] = collection1.frames[3].clone();
        frame1[2] = collection1.frames[7].clone();
        frame2[0] = collection2.frames[0].clone();
        frame2[1] = collection2.frames[3].clone();
        frame2[2] = collection2.frames[7].clone();
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    cv::Vec3b pix1 = frame1[j].at<cv::Vec3b>(z, i);
                    cv::Vec3b pix2 = frame2[j].at<cv::Vec3b>(z, i);
                    pixel[j] = static_cast<unsigned char>( (0.33 * pixel[j]) + (0.33 * pix1[j]) + (0.33 * pix2[j]));
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::VideoMatrixBlendAlpha(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    static double alpha = 0.33;
    static int dir1 = 1;
    static double palpha = 0.33/2;
    static int dir2 = 1;
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        static MatrixCollection<8> collection1, collection2;
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        collection1.shiftFrames(frame);
        collection2.shiftFrames(reframe);
        cv::Mat frame1[3], frame2[3];
        frame1[0] = collection1.frames[0].clone();
        frame1[1] = collection1.frames[3].clone();
        frame1[2] = collection1.frames[7].clone();
        frame2[0] = collection2.frames[0].clone();
        frame2[1] = collection2.frames[3].clone();
        frame2[2] = collection2.frames[7].clone();
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    cv::Vec3b pix1 = frame1[j].at<cv::Vec3b>(z, i);
                    cv::Vec3b pix2 = frame2[j].at<cv::Vec3b>(z, i);
                    pixel[j] = static_cast<unsigned char>( (palpha * pixel[j]) + (alpha * pix1[j]) + ((0.33-alpha) * pix2[j]));
                }
            }
        }
    }
    AddInvert(frame);
    AlphaMovementMaxMin(alpha, dir1, 0.01, 0.33, 0.1);
    AlphaMovementMaxMin(palpha, dir2, 0.01, 0.33, 0.1);
}

void ac::VideoMatrixBlendAlphaRandom(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    static int lazy = 1;
    static double alpha[3] = {0.33, 0.33, 0.33};
    static int dir[3] = {1,1,1};
    if(reset_alpha == true || lazy == 1) {
        for(int j = 0; j < 3; ++j) {
            alpha[j] = (0.01 * (rand()%33));
            dir[j] = rand()%2;
        }
        lazy = 0;
    }
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        static MatrixCollection<8> collection1, collection2;
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        collection1.shiftFrames(frame);
        collection2.shiftFrames(reframe);
        cv::Mat frame1[3], frame2[3];
        frame1[0] = collection1.frames[0].clone();
        frame1[1] = collection1.frames[3].clone();
        frame1[2] = collection1.frames[7].clone();
        frame2[0] = collection2.frames[0].clone();
        frame2[1] = collection2.frames[3].clone();
        frame2[2] = collection2.frames[7].clone();
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    cv::Vec3b pix1 = frame1[j].at<cv::Vec3b>(z, i);
                    cv::Vec3b pix2 = frame2[j].at<cv::Vec3b>(z, i);
                    pixel[j] = static_cast<unsigned char>((alpha[0] * pixel[j]) + (alpha[1] * pix1[j]) + (alpha[2] * pix2[j]));
                }
            }
        }
    }
    AddInvert(frame);
    for(int j = 0; j < 3; ++j)
        AlphaMovementMaxMin(alpha[j], dir[j], 0.01, 0.33, 0.1);
}

void ac::VideoMatrixSwitch(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    static int index = 0;
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
            for(int z = offset; z <  offset+size; ++z) {
                for(int i = 0; i < cols; ++i) {
                    cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                    cv::Vec3b pix = reframe.at<cv::Vec3b>(z, i);
                    pixel[index] = static_cast<unsigned char>((0.5 * pixel[index] + (0.5 * pix[index])));
                }
            }
        };
        UseMultipleThreads(frame, getThreadCount(), callback);
    }
    AddInvert(frame);
    ++index;
    if(index > 2) {
        index  = 0;
    }
}

void ac::VideoCollectionWave(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    static MatrixCollection<32> collection1, collection2;
    collection1.shiftFrames(frame);
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        collection2.shiftFrames(reframe);
        int row_size = 50;
        int row_index = 0;
        int row_dir = 1, row_size_dir = 1;
        for(int q = 0; q < frame.rows; q += row_size) {
            for(int z = 0; z < q+row_size && z < frame.rows; ++z) {
                for(int i = 0; i < frame.cols; ++i) {
                    cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                    cv::Vec3b pix1 = collection1.frames[row_index].at<cv::Vec3b>(z, i);
                    cv::Vec3b pix2 = collection2.frames[row_index].at<cv::Vec3b>(z, i);
                    for(int j = 0; j < 3; ++j) {
                        pixel[j] = static_cast<unsigned char>((0.3 * pixel[j]) + (0.5 * pix1[j]) + (0.2 * pix2[j]));
                    }
                }
                if(row_dir == 1) {
                    ++row_index;
                    if(row_index > (collection1.size()-1)) {
                        row_index = collection1.size()-1;
                        row_dir = 0;
                    }
                } else {
                    --row_index;
                    if(row_index <= 1) {
                        row_index = 1;
                        row_dir = 1;
                        if(row_size_dir == 1) {
                            row_size += 10;
                            if(row_size > 150) {
                                row_size_dir = 0;
                            }
                        } else {
                            row_size -= 10;
                            if(row_size <= 50) {
                                row_size = 50;
                                row_size_dir = 1;
                            }
                        }
                    }
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::CollectionWave(cv::Mat &frame) {
    static MatrixCollection<24> collection1;
    collection1.shiftFrames(frame);
    static int row_size = 10;
    int row_index = 0;
    int row_dir = 1, row_size_dir = 1;
    for(int q = 0; q < frame.rows; q += row_size) {
        for(int z = 0; z < q+row_size && z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                cv::Vec3b pix1 = collection1.frames[row_index].at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix1[j]));
                }
            }
            if(row_dir == 1) {
                ++row_index;
                if(row_index > (collection1.size()-1)) {
                    row_index = collection1.size()-1;
                    row_dir = 0;
                }
            } else {
                --row_index;
                if(row_index <= 1) {
                    row_index = 1;
                    row_dir = 1;
                    if(row_size_dir == 1) {
                        row_size += 20;
                        if(row_size > 100) {
                            row_size_dir = 0;
                        }
                    } else {
                        row_size -= 20;
                        if(row_size <= 10) {
                            row_size = 50;
                            row_size_dir = 1;
                        }
                    }
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::TremorShake(cv::Mat &frame) {
    static MatrixCollection<24> collection;
    collection.shiftFrames(frame);
    int row_index = 0;
    static int row_size = 10;
    static int row_size_dir = 1;
    int row_y = 0;
    int row_dir = 1;
    for(int q = 0; q < collection.size(); ++q) {
        for(int z = row_y; z < frame.rows && z < row_y+row_size; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                cv::Vec3b pix = collection.frames[row_index].at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                }
            }
        }
        if(row_dir == 1) {
            ++row_index;
            if(row_index > (collection.size()-1)) {
                row_index = collection.size()-1;
                row_dir = 0;
            }
        } else {
            --row_index;
            if(row_index <= 1) {
                row_index = 1;
                row_dir = 1;
            }
        }
        row_y += row_size;
        
        if(row_size_dir == 1) {
            row_size += 10;
            if(row_size > 250) {
                row_size = 250;
                row_size_dir = 0;
            }
        } else {
            row_size -= 10;
            if(row_size <= 1) {
                row_size = 0;
                row_size_dir = 1;
            }
        }
    }
    AddInvert(frame);
}

void ac::PixelateSquares(cv::Mat &frame) {
    int pix_size_w = 5+rand()%50, pix_size_h = 5+rand()%50;
    int num_square_w = frame.cols/pix_size_w;
    int num_square_h = frame.rows/pix_size_h;
    static MatrixCollection<24> collection;
    collection.shiftFrames(frame);
    int index = 0;
    for(int z = 0; z < num_square_h; ++z) {
        for(int i = 0; i < num_square_w; ++i) {
            for(int p = (z*pix_size_h); p < (z*pix_size_h)+pix_size_h && p < frame.rows; ++p) {
                for(int q = (i*pix_size_w); q < (i*pix_size_w)+pix_size_w && p < frame.cols; ++q) {
                    cv::Vec3b &pixel = frame.at<cv::Vec3b>(p, q);
                    cv::Vec3b col = collection.frames[index].at<cv::Vec3b>(p, q);
                    for(int j = 0; j < 3; ++j) {
                        pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * col[j]));
                    }
                }
            }
            ++index;
            if(index > (collection.size()-1))
                index = 0;
        }
    }
    AddInvert(frame);
}

void ac::ColorPixelDoubleXor(cv::Mat &frame) {
    static double value[3] = {1,3,5};
    if(reset_alpha == true) {
        value[0] = 1;
        value[1] = 3;
        value[2] = 5;
    }
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (static_cast<int>(pixel[j]^static_cast<int>(value[j]))));
                }
            }
        }
    };
    for(int j = 0; j < 3; ++j) {
        int r = rand()%100;
        value[j] += 0.3 * r;
    }
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}
