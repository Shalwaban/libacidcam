

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

void ac::ImageMirror_DownSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "ImageMirror_DownSubFilter")
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    CallFilter(subfilter, reimage);
    cv::Mat copy1 = reimage.clone();
    int halfway = (frame.rows/2);
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < halfway; ++z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = copy1.at<cv::Vec3b>(frame.rows-z-1, i);
            ASSERT(frame.rows-z-1 > 0);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
            }
        }
    }
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = halfway; z < frame.rows; ++z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = copy1.at<cv::Vec3b>(z, i);
            ASSERT(frame.rows-z-1 > 0);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
            }
        }
    }
    AddInvert(frame);
}

void ac::ImageMirrorKaleidoscope(cv::Mat &frame) {
    if(blend_set == false)
        return;
    pushSubFilter(getFilterByName("ColorPulseIncrease"));
    ImageMirror_LeftSubFilter(frame);
    popSubFilter();
    pushSubFilter(getFilterByName("ColorTransition"));
    ImageMirror_UpSubFilter(frame);
    popSubFilter();
    MirrorLeftBottomToTop(frame);
    MedianBlendMultiThread(frame);
}

void ac::ExactImageSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "ExactImageSubFilter")
        return;
    ExactImage(frame);
    CallFilter(subfilter, frame);
}

void ac::AlphaImageSourceScale(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    static double alpha = 1.0;
    static int dir = 1;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b img_pix = reimage.at<cv::Vec3b>(z, i);
                cv::Vec3b opix = ac::orig_frame.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>(((alpha/2) * pixel[j]) + ((alpha/2) * opix[j]) + ((1-alpha) * img_pix[j]));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
    AlphaMovementMaxMin(alpha, dir, 0.005, 1.0, 0.1);
}

void ac::GhostMirrorReversed(cv::Mat &frame) {
    cv::Mat temp = frame.clone();
    cv::Mat reimage;
    cv::flip(temp, reimage, 1);
    static double alpha = 1.0;
    static int dir = 1;
    
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b img_pix = reimage.at<cv::Vec3b>(z, i);
                cv::Vec3b opix = ac::orig_frame.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>(((alpha/2) * pixel[j]) + ((alpha/2) * opix[j]) + ((1-alpha) * img_pix[j]));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    GhostTrails(frame);
    AddInvert(frame);
    AlphaMovementMaxMin(alpha, dir, 0.005, 1.0, 0.1);
}

void ac::GhostMirrorFade(cv::Mat &frame) {
    cv::Mat temp = frame.clone();
    cv::Mat reimage;
    cv::flip(temp, reimage, 1);
    static double alpha = 1.0;
    static int dir = 1;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b img_pix = reimage.at<cv::Vec3b>(z, i);
                cv::Vec3b opix = ac::orig_frame.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>(((alpha/2) * pixel[j]) + ((alpha/2) * opix[j]) + ((1-alpha) * img_pix[j]));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    BlendForwardAndBack64(frame);
    AddInvert(frame);
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
}

void ac::MirrorFadeLeftRight(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    MirrorLeft(copy1);
    MirrorRight(copy2);
    static double alpha = 1.0;
    static int dir = 1;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b copy1_pix = copy1.at<cv::Vec3b>(z, i);
                cv::Vec3b copy2_pix = copy2.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((alpha * copy1_pix[j]) + ((1-alpha) * copy2_pix[j]));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
}

void ac::FadeMirrorSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "FadeMirrorSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    MirrorLeft(copy1);
    CallFilter(subfilter, copy1);
    MirrorRight(copy2);
    CallFilter(subfilter, frame);
    static double alpha = 1.0;
    static int dir = 1;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b copy1_pix = copy1.at<cv::Vec3b>(z, i);
                cv::Vec3b copy2_pix = copy2.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((alpha * copy1_pix[j]) + ((1-alpha) * copy2_pix[j]));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
}

void ac::MirrorFadeUpDown(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    MirrorTopToBottom(copy1);
    MirrorBottomToTop(copy2);
    static double alpha = 1.0;
    static int dir = 1;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b copy1_pix = copy1.at<cv::Vec3b>(z, i);
                cv::Vec3b copy2_pix = copy2.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((alpha * copy1_pix[j]) + ((1-alpha) * copy2_pix[j]));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
}

void ac::DarkTrailsKaleidoscope(cv::Mat &frame) {
    GhostMirrorFade(frame);
    MirrorFadeLeftRight(frame);
    MirrorFadeUpDown(frame);
    MedianBlendDark(frame);
    AddInvert(frame);
}

void ac::MirrorFadeAll(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    static double alpha = 1.0;
    static int dir = 1;
    MirrorFadeLeftRight(copy1);
    MirrorFadeUpDown(copy2);
    AlphaBlendDouble(copy1, copy2, frame, alpha, (1-alpha));
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
    AddInvert(frame);
}

void ac::KaleidoscopeMirrorSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "KaleidoscopeMirrorSubFilter")
        return;
    static double alpha = 1.0;
    static int dir = 1;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    MirrorRightTopToBottom(copy2);
    MirrorLeftBottomToTop(copy1);
    CallFilter(subfilter, copy1);
    CallFilter(subfilter, copy2);
    AlphaBlendDouble(copy1, copy2, frame, alpha, (1-alpha));
    MedianBlendMultiThread(frame);
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
    AddInvert(frame);
}

void ac::ImageKaleidoscopeMirrorAlphaSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "ImageKaleidoscopeMirrorAlphaSubFilter")
        return;
    static double alpha = 1.0;
    static int dir = 1;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone(), copy3;
    pushSubFilter(subfilter);
    ImageMirror_LeftSubFilter(copy1);
    popSubFilter();
    pushSubFilter(subfilter);
    ImageMirror_RightSubFilter(copy2);
    popSubFilter();
    AlphaBlendDouble(copy1, copy2, copy3, alpha,(1-alpha));
    AlphaBlendDouble(ac::orig_frame, copy3, frame, alpha, (1-alpha));
    MedianBlendMultiThread(frame);
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
    AddInvert(frame);
}

void ac::BlendOppositesSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "BlendOppositesSubFilter")
        return;
    
    cv::Mat copy1 = frame.clone();
    cv::Mat copy2 = frame.clone();
    cv::Mat copy3, copy4;
    MirrorLeft(copy1);
    MirrorRight(copy2);
    CallFilter(subfilter, copy1);
    CallFilter(subfilter, copy2);
    static double alpha = 1.0;
    static int dir = 1;
    AlphaBlendDouble(copy1, copy2, copy3, alpha, (1-alpha));
    AlphaBlendDouble(copy1, copy2, copy4, (1-alpha), alpha);
    AlphaBlendDouble(copy3, copy4, frame, 0.5, 0.5);
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
    AddInvert(frame);
}

void ac::AlphaBlendMirrorFade(cv::Mat &frame) {
    static double alpha = 1.0;
    static int dir = 1;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    AlphaBlendImageWithOrigSource(copy1);
    MirrorLeft(copy1);
    AlphaBlendWithSource(copy2);
    AlphaBlendImageWithOrigSource(copy2);
    MirrorRight(copy2);
    AlphaBlendDouble(copy1, copy2, frame, alpha, (1-alpha));
    MedianBlendMultiThread(frame);
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
    AddInvert(frame);
}

void ac::DarkenMedianBlend(cv::Mat &frame) {
    DarkenFilter(frame);
    MedianBlendMultiThread(frame);
    AddInvert(frame);
}

void ac::ReduceMedianBlend(cv::Mat &frame) {
    ReduceBy50(frame);
    MedianBlendMultiThread(frame);
    AddInvert(frame);
}

void ac::DarkColors(cv::Mat &frame) {
    static double alpha = 1.0;
    static int dir = 1;
    cv::Vec3b value(rand()%255, rand()%255, rand()%255);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((alpha * pixel[j]) + ( (value[j]^pixel[j]) ));
                    pixel[j] /= 3;
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
    AddInvert(frame);
}

void ac::DarkenChannelXorNoMedian(cv::Mat &frame) {
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b mulpix = pixel;
                for(int j = 0; j < 3; ++j) {
                    mulpix[j] /= 2;
                    pixel[j] = pixel[j] ^ mulpix[j];
                }
            }
        }
    };
    NoMedianBlurBlendMultiThread(frame);
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::DarkenStrobeMedianBlend(cv::Mat &frame) {
    static int offset = 0;
    int colors[3] = {0};
    InitArrayPosition(colors, offset);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                pixel[colors[0]] /= 2;
                pixel[colors[1]] /= 3;
                pixel[colors[2]] /= 4;
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    ++offset;
    if(offset > 2)
        offset = 0;
    NoMedianBlurBlendMultiThread(frame);
    AddInvert(frame);
}

void ac::DarkenImageAlphaBlend(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    static double alpha = 1.0;
    static int dir = 1;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b pix = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pix[j] /= 3;
                    pixel[j] = ((pixel[j] * alpha) + (pix[j] * (1-alpha)));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
    AddInvert(frame);
}

void ac::MatrixStrobeTrails(cv::Mat &frame) {
    static int col[3] = {0};
    static int index = 0;
    InitArrayPosition(col, index);
    ++index;
    if(index > 2)
        index = 0;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat frames[3];
    frames[0] = collection.frames[1].clone();
    frames[1] = collection.frames[4].clone();
    frames[2] = collection.frames[7].clone();
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    cv::Vec3b pix = frames[j].at<cv::Vec3b>(z, i);
                    pixel[j] = pix[col[j]];
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::MatrixStrobeMedianBlend(cv::Mat &frame) {
    MatrixStrobeTrails(frame);
    NoMedianBlurBlendMultiThread(frame);
}

void ac::ImageEnergyKaleidoscopeSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "ImageEnergyKaleidoscopeSubFilter")
        return;
    cv::Mat copy1 = frame.clone();
    CallFilter(subfilter, copy1);
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat frames[3];
    frames[0] = collection.frames[1].clone();
    frames[1] = reimage.clone();
    frames[2] = copy1.clone();
    static double alpha = 1.0;
    static int dir = 1;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    cv::Vec3b pix = frames[j].at<cv::Vec3b>(z, i);
                    pixel[j] = static_cast<unsigned char>((alpha * pixel[j]) + ((1-alpha) * pix[j]));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    MirrorLeftBottomToTop(frame);
    MedianBlendMultiThread(frame);
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
    AddInvert(frame);
}
