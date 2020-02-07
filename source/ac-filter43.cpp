
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

void ac::VideoImageBlendAlpha(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        static double alpha[3] = {0.1, 0.2, 0.3};
        static int dir[3] = {1,1,1};
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                cv::Vec3b pix1 = reframe.at<cv::Vec3b>(z, i);
                cv::Vec3b pix2 = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((alpha[0] * pixel[j]) + (alpha[1] * pix1[j]) + (alpha[2] * pix2[j]));
                }
            }
        }
        for(int j = 0; j < 3; ++j)
            AlphaMovementMaxMin(alpha[j], dir[j], 0.01, 0.4, 0.1);
    }
    
    AddInvert(frame);
}

void ac::IntertwineCols640(cv::Mat &frame) {
    cv::Mat sizef;
    ac_resize(frame, sizef, cv::Size(640, 360));
    static MatrixCollection<640> collection;
    IntertwineCols(sizef, &collection, 1);
    ac_resize(sizef, frame, frame.size());
    AddInvert(frame);
}

void ac::HorizontalOffsetLess_SubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "HorizontalOffsetLess_SubFilter")
        return;
    cv::Mat frame_copy = frame.clone();
    CallFilter(subfilter, frame_copy);
    static int offset_y = (rand()%(frame.rows));
    bool on = false;
    static int counter = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            if(on == true) {
                cv::Vec3b pix = frame_copy.at<cv::Vec3b>(z, i);
                pixel = pix;
            }
        }
        ++counter;
        if((counter%(offset_y+1)==0)) {
            int val = rand()%40;
            if(val == 0) {
                on = !on;
            }
            offset_y = rand()%(frame.rows);
            counter = 0;
        }
    }
    AddInvert(frame);
}

void ac::VerticalOffsetLess_SubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "VerticalOffsetLess_SubFilter")
        return;
    cv::Mat frame_copy = frame.clone();
    CallFilter(subfilter, frame_copy);
    static int offset_y = (rand()%(frame.rows));
    bool on = false;
    static int counter = 0;
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            if(on == true) {
                cv::Vec3b pix = frame_copy.at<cv::Vec3b>(z, i);
                pixel = pix;
            }
        }
        ++counter;
        if((counter%(offset_y+1)==0)) {
            int val = rand()%40;
            if(val == 0) {
                on = !on;
            }
            offset_y = rand()%(frame.rows);
            counter = 0;
        }
    }
    AddInvert(frame);
}

void ac::SquareOffsetLess_SubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "SquareOffsetLess_SubFilter")
        return;
    cv::Mat frame_copy = frame.clone();
    CallFilter(subfilter, frame_copy);
    static int offset_y = (rand()%(frame.rows));
    bool on = false;
    static int counter = 0;
    int start_y = rand()%frame.rows;
    int stop_y = rand()%frame.rows;
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = start_y; z < stop_y; ++z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            if(on == true) {
                cv::Vec3b pix = frame_copy.at<cv::Vec3b>(z, i);
                pixel = pix;
            }
        }
        ++counter;
        if((counter%(offset_y+1)==0)) {
            int val = rand()%40;
            if(val == 0) {
                on = !on;
            }
            offset_y = rand()%(frame.rows);
            counter = 0;
        }
    }
    AddInvert(frame);
}

void ac::SquareOffset_SubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "SquareOffset_SubFilter")
        return;
    cv::Mat frame_copy = frame.clone();
    CallFilter(subfilter, frame_copy);
    static int offset_y = (rand()%(frame.rows));
    bool on = true;
    static int counter = 0;
    int start_x = rand()%frame.cols;
    int stop_x = rand()%frame.cols;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = start_x; i < stop_x; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            if(on == true) {
                cv::Vec3b pix = frame_copy.at<cv::Vec3b>(z, i);
                pixel = pix;
            }
        }
        ++counter;
        if((counter%(offset_y+1)==0)) {
            if(on == false && (rand()%250)==0) {
                on = !on;
            } else {
                on = !on;
            }
            offset_y = rand()%(frame.rows);
            counter = 0;
        }
    }
    AddInvert(frame);
}

void ac::PrevFrameNotEqual(cv::Mat &frame) {
    static cv::Mat prev = frame.clone();
    if((prev.size() != frame.size()) || (reset_alpha == true)) {
        prev = frame.clone();
    }
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b p = prev.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                if(pixel[j] >= p[j] && pixel[j] <= p[j]+getPixelCollection())
                    pixel[j] = p[j];
            }
        }
    }
    prev = frame.clone();
    AddInvert(frame);
}

void ac::MirrorDelayLeft(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat &copy = collection.frames[7];
    MirrorLeft(frame);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols/2; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = copy.at<cv::Vec3b>(z, i);
            pixel = pix;
        }
    }
    AddInvert(frame);
}
void ac::MirrorDelayRight(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat &copy = collection.frames[7];
    MirrorRight(frame);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = frame.cols/2; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = copy.at<cv::Vec3b>(z, i);
            pixel = pix;
        }
    }
    AddInvert(frame);
}

void ac::ApplyColorRange(cv::Mat &frame) {
    if(getColorRangeEnabled()==true) {
        applyColorRange(frame);
    }
}
void ac::IntertwineAlphaBlend(cv::Mat &frame) {
    cv::Mat sizef;
    cv::Mat copy1 = frame.clone();
    ac_resize(frame, sizef, cv::Size(640, 360));
    static MatrixCollection<360> collection;
    IntertwineRows(sizef, &collection, 1);
    ac_resize(sizef, frame, frame.size());
    ac_resize(copy1, sizef, cv::Size(640, 360));
    static MatrixCollection<640> collection2;
    IntertwineCols(sizef, &collection2, 1);
    ac_resize(sizef, copy1, copy1.size());
    cv::Mat out;
    AlphaBlend(frame, copy1, out, 0.5);
    frame = out.clone();
    AddInvert(frame);
}

void ac::BlackLines(cv::Mat &frame) {
    static int depth = rand()%50;
    cv::Mat copy1 = frame.clone();
    static int counter = 0;
    static int counter_max = rand()%70;
    static int frame_counter = 0;
    static bool on = true;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b d;
            if(i > depth && i+depth < frame.cols)
                d = copy1.at<cv::Vec3b>(z, i+depth);
            else
                d = cv::Vec3b(0, 0, 0);
            if(on == true) {
                if(i+depth < frame.cols) {
                    for(int j = 0; j < 3; ++j) {
                        pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * d[j]));
                    }
                } else {
                    pixel = cv::Vec3b(0, 0, 0);
                }
            }
            if(i < depth) {
                d = copy1.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * d[j]));
                }
            }
        }
        if(frame_counter == 0) {
            ++counter;
            if(counter > counter_max) {
                counter = 0;
                depth = rand()%50;
                counter_max = rand()%70;
                on = !on;
            }
        }
    }
    ++frame_counter;
    if(frame_counter > 1) {
        frame_counter = 0;
    }
    AddInvert(frame);
}

void ac::MedianBlendMultiThread2(cv::Mat &frame) {
    static MatrixCollection<2> collection;
    int r = 1+rand()%2;
    for(int i = 0; i < r; ++i)
        MedianBlur(frame);
    collection.shiftFrames(frame);
    static auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Scalar value;
                for(int j = 0; j < collection.size(); ++j) {
                    cv::Vec3b pixel = collection.frames[j].at<cv::Vec3b>(z, i);
                    for(int q = 0; q < 3; ++q) {
                        value[q] += pixel[q];
                    }
                }
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    int val = 1+static_cast<int>(value[j]);
                    pixel[j] = static_cast<unsigned char>(pixel[j] ^ val);
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::MedianBlendMultiThread2_ColorChange(cv::Mat &frame) {
    ColorAddBlend(frame);
    ColorTransition(frame);
    MedianBlendMultiThread2(frame);
    AddInvert(frame);
}

void ac::VariablesExtraHorizontal(cv::Mat &frame) {
    int total_lines = frame.cols-2;
    int current_line = 0;
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    static int wait = 0;
    ++wait;
    if(wait > getVariableWait()) {
        wait = 0;
    } else {
        return;
    }
    
    while(current_line < total_lines) {
        int rand_height = 10+rand()%490;
        if(current_line+rand_height > total_lines)
            rand_height = total_lines-current_line;
        int rand_frame = rand()%(collection.size()-1);
        for(int i = current_line; i < current_line+rand_height; ++i) {
            if(current_line > total_lines)
                break;
            for(int z = 0; z < frame.rows-1; ++z) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                cv::Vec3b pix = collection.frames[rand_frame].at<cv::Vec3b>(z, i);
                pixel = pix;
            }
        }
        current_line += rand_height;
    }
    AddInvert(frame);
}

void ac::ChannelSortDelay(cv::Mat &frame) {
    static cv::Mat copy1 = frame.clone();
    if(copy1.size() != frame.size() || reset_alpha == true)
        copy1 = frame.clone();
    cv::Mat cp = frame.clone();
    static int wait = 0;
    ++wait;
    if(wait > getVariableWait()) {
        ChannelSort_NoBlend_Descending(cp);
        MedianBlendMultiThread(cp);
        copy1 = cp.clone();
        wait = 0;
    }
    cv::Mat out;
    AlphaBlendDouble(copy1, frame, out, 0.3, 0.7);
    frame = out.clone();
    AddInvert(frame);
}

void ac::DizzyMode(cv::Mat &frame) {
    static cv::Mat copy1 = frame.clone();
    if(copy1.size() != frame.size() || reset_alpha == true)
        copy1 = frame.clone();
    cv::Mat cp = frame.clone();
    static int wait = 0;
    ++wait;
    if(wait > getVariableWait()) {
        copy1 = cp.clone();
        wait = 0;
    }
    cv::Mat out;
    AlphaBlendDouble(copy1, frame, out, 0.5, 0.5);
    frame = out.clone();
    AddInvert(frame);
}
