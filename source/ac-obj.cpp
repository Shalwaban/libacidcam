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

ac::Point::Point() : x(0), y(0) {}
ac::Point::Point(const ac::Point &p) : x(p.x), y(p.y) {}
ac::Point::Point(int xx, int yy) : x(xx), y(yy) {}

ac::Point &ac::Point::operator=(const ac::Point &p) {
    x = p.x;
    y = p.y;
    return *this;
}

void ac::Point::setPoint(int xx, int yy) {
    x = xx;
    y = yy;
}

ac::Rect::Rect() : x(0), y(0), w(0), h(0) {}
ac::Rect::Rect(const ac::Rect &r) : x(r.x), y(r.y), w(r.w), h(r.h) {}
ac::Rect::Rect(int xx, int yy, int ww, int hh) : x(xx), y(yy), w(ww), h(hh) {}
ac::Rect::Rect(int xx, int yy) : x(xx), y(yy), w(0), h(0) {}
ac::Rect::Rect(int xx, int yy, cv::Size s) : x(xx), y(yy), w(s.width), h(s.height) {}
ac::Rect::Rect(Point pt, int ww, int hh) : x(pt.x), y(pt.y), w(ww), h(hh) {}
ac::Rect::Rect(Point pt, cv::Size s) : x(pt.x), y(pt.y), w(s.width), h(s.height){}

ac::Rect &ac::Rect::operator=(const ac::Rect &r) {
    x = r.x;
    y = r.y;
    w = r.w;
    h = r.h;
    return *this;
}

void ac::Rect::setRect(int xx, int yy, int ww, int hh) {
    x = xx;
    y = yy;
    w = ww;
    h = hh;
}

