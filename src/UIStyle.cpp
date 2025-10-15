#include "UIStyle.hpp"

UIStyle MakeClassicStyle() {
    UIStyle s;
    s.radiusSm = 6;
    s.radiusMd = 10;
    s.radiusLg = 16;
    s.borderThin = 1;
    s.borderThick = 2;
    s.padSm = 6;
    s.padMd = 10;
    s.padLg = 16;
    return s;
}

UIStyle MakeMinimalStyle() {
    UIStyle s;
    s.radiusSm = 4;
    s.radiusMd = 6;
    s.radiusLg = 10;
    s.borderThin = 1;
    s.borderThick = 1;
    s.padSm = 4;
    s.padMd = 8;
    s.padLg = 12;
    return s;
}
