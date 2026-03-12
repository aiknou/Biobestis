#pragma once

class EyeDisplayController {
public:
    void init();
    void test();
    void drawBitmap(const unsigned char* bitmap);
    void clear();
};

extern EyeDisplayController eyeCtrl;
