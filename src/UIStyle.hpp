#pragma once

struct UIStyle {
    int radiusSm   = 6;
    int radiusMd   = 10;
    int radiusLg   = 16;
    int borderThin = 1;
    int borderThick= 2;
    int padSm      = 6;
    int padMd      = 10;
    int padLg      = 16;
};

UIStyle MakeClassicStyle();
UIStyle MakeMinimalStyle();
