#include <cmath>

#include "lib/easybmp/EasyBMP.h"

#include "lib/header.h"

inline double f(double x, double y) {
    return ( 5000-0.005*(x*x+y*y+x*y)+12.5*(x+y) ) * exp( -abs(0.000001*(x*x+y*y)-0.0015*(x+y)+0.7) );
}

static constexpr size_t kN = 1600;

static void draw(double h, const string& filename) {
    BMP bmp;
    bmp.SetSize(kN, kN);
    for (size_t i = 0; i < kN; ++i) {
        for (size_t j = 0; j < kN; ++j) {
            if (f(i, j) < h) {
                bmp.SetPixel(i, j, RGBApixel::kBlack);
            } else {
                bmp.SetPixel(i, j, RGBApixel::kWhite);
            }
        }
    }
    bmp.SetPixel(200, 200, RGBApixel::kRed);
    bmp.SetPixel(1400, 1400, RGBApixel::kRed);
    bmp.WriteToFileChecked(filename.c_str());
}

int main() {
    cerr << f(200, 200) << " " << f(1400, 1400) << endl;
    draw(f(200, 200), "a.bmp");
    draw(f(1400, 1400), "b.bmp");
    draw(10000, "c.bmp");
    draw(10400, "d.bmp");
    draw(10396, "e.bmp");
    return 0;
}
