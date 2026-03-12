#pragma once

class DataDisplayController {
public:
    void init();
    void test();
    void drawCenteredText(const char* text, int textSize);
    void drawTextWithProgress(const char* text, int progress);
    void clear();
};

extern DataDisplayController dataCtrl;
