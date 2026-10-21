#pragma once
#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include "Image_Class.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


namespace F {

// ============ Filter 1: Greyscale ============
inline void greyscale(Image& image) {
    for (int i = 0; i < image.width; ++i) {
        for (int j = 0; j < image.height; ++j) {
            unsigned int average = 0;
            for (int k = 0; k < 3; ++k) {
                average += image(i, j, k);
            }
            average /= 3;
            image(i, j, 0) = average;
            image(i, j, 1) = average;
            image(i, j, 2) = average;
        }
    }
}

// ============ Filter 2: Black and White ============
inline void blackWhite(Image& image) {
    for (int i = 0; i < image.width; i++) {
        for (int j = 0; j < image.height; j++) {
            unsigned int avg = 0;
            unsigned int x = 0;
            for (int k = 0; k < 3; k++) {
                avg += image(i, j, k);
                x = image(i, j, k) >= 128;
            }
            avg /= 3;

            for (int k = 0; k < 3; k++) {
                image(i, j, k) = avg;
            }

            for (int k = 0; k < 3; k++) {
                if (x == 1) {
                    image.setPixel(i, j, k, 255);
                } else {
                    image.setPixel(i, j, k, 0);
                }
            }
        }
    }
}

// ============ Filter 3: Invert ============
inline void invert(Image& image_to_filter) {
    for (int i = 0; i < image_to_filter.width; i++) {
        for (int j = 0; j < image_to_filter.height; j++) {
            for (int k = 0; k < 3; k++) {
                image_to_filter(i, j, k) = 255 - image_to_filter(i, j, k);
            }
        }
    }
}

// ============ Filter 4: Frame ============
inline void addSimpleFrame(Image& img, int thickness, int r, int g, int b) {
    for (int i = 0; i < img.width; ++i) {
        for (int j = 0; j < img.height; ++j) {
            if (i < thickness || i >= img.width - thickness ||
                j < thickness || j >= img.height - thickness) {
                img(i, j, 0) = r;
                img(i, j, 1) = g;
                img(i, j, 2) = b;
            }
        }
    }
}

inline void addFancyFrame(Image& img, int thickness, int r, int g, int b) {
    int gap = thickness / 2;
    int innerThickness = 2;

    for (int i = 0; i < img.width; ++i) {
        for (int j = 0; j < img.height; ++j) {

            if (i < thickness || i >= img.width - thickness ||
                j < thickness || j >= img.height - thickness) {
                img(i, j, 0) = r;
                img(i, j, 1) = g;
                img(i, j, 2) = b;
            }

            int start = thickness + gap;
            int endW = img.width - (thickness + gap);
            int endH = img.height - (thickness + gap);

            if ((i >= start && i < start + innerThickness) || (i < endW && i >= endW - innerThickness) ||
                (j >= start && j < start + innerThickness) || (j < endH && j >= endH - innerThickness)) {

                if (i >= start && i < endW && j >= start && j < endH) {
                    img(i, j, 0) = 255;
                    img(i, j, 1) = 255;
                    img(i, j, 2) = 255;
                }
            }
        }
    }
}

inline void frame(Image& img, bool fancy, int thickness, int r, int g, int b) {
    if (fancy) addFancyFrame(img, thickness, r, g, b);
    else       addSimpleFrame(img, thickness, r, g, b);
}

// ============ Filter 5: Flip ============
inline void flip(Image& image, bool horizontal) {
    if (horizontal) {
        for (int i = 0; i < image.width / 2; ++i) {
            for (int j = 0; j < image.height; ++j) {
                for (int k = 0; k < 3; ++k) {
                    std::swap(image(i, j, k), image(image.width - 1 - i, j, k));
                }
            }
        }
    } else {
        for (int i = 0; i < image.width; ++i) {
            for (int j = 0; j < image.height / 2; ++j) {
                for (int k = 0; k < 3; ++k) {
                    std::swap(image(i, j, k), image(i, image.height - 1 - j, k));
                }
            }
        }
    }
}

// ============ Filter 6: Rotate ============
inline void rotate(Image& image, int degree) {
    if (degree == 180) {
        for (int i = 0; i < image.width; i++) {
            for (int j = 0; j < image.height / 2; j++) {
                int tempR = image.getPixel(i, j, 0);
                int tempG = image.getPixel(i, j, 1);
                int tempB = image.getPixel(i, j, 2);

                for (int k = 0; k < 3; k++) {
                    image(i, j, k) = image(image.width - 1 - i, image.height - 1 - j, k);
                }

                image(image.width - 1 - i, image.height - 1 - j, 0) = tempR;
                image(image.width - 1 - i, image.height - 1 - j, 1) = tempG;
                image(image.width - 1 - i, image.height - 1 - j, 2) = tempB;
            }
        }
    }
    else if (degree == 90) {
        Image image2(image.height, image.width);

        for (int i = 0; i < image.width; i++) {
            for (int j = 0; j < image.height; j++) {
                int new_i = image.height - 1 - j;
                int new_j = i;
                for (int k = 0; k < 3; k++) {
                    image2(new_i, new_j, k) = image(i, j, k);
                }
            }
        }
        image = image2;
    }
    else if (degree == 270) {
        Image image2(image.height, image.width);

        for (int i = 0; i < image.width; i++) {
            for (int j = 0; j < image.height; j++) {
                int new_i = j;
                int new_j = image.width - 1 - i;
                for (int k = 0; k < 3; k++) {
                    image2(new_i, new_j, k) = image(i, j, k);
                }
            }
        }
        image = image2;
    }
}

// ============ Filter 7: Darken / Lighten ============
inline void brightness(Image& image, double factor) {
    for (int i = 0; i < image.width; i++) {
        for (int j = 0; j < image.height; j++) {
            for (int k = 0; k < 3; k++) {
                if (factor < 1.0) {
                    image(i, j, k) *= factor;
                } else {
                    float changes_to_lighten = image(i, j, k) * (float)factor;
                    image(i, j, k) = std::min(255.0f, changes_to_lighten);
                }
            }
        }
    }
}

// ============ Filter 8: Resize ============
inline void resize(Image& image, int Width, int Height) {
    if (Width <= 0 || Height <= 0) return;

    Image resizedImage(Width, Height);

    for (int y = 0; y < Height; y++) {
        for (int x = 0; x < Width; x++) {
            int oldX = x * image.width / Width;
            int oldY = y * image.height / Height;

            resizedImage(x, y, 0) = image(oldX, oldY, 0);
            resizedImage(x, y, 1) = image(oldX, oldY, 1);
            resizedImage(x, y, 2) = image(oldX, oldY, 2);
        }
    }
    image = resizedImage;
}

// ============ Filter 9: Merge ============
// biggest = true  -> الأبعاد الأكبر (choice 1 في القديم)
// biggest = false -> الأبعاد الأصغر (choice 2 في القديم)
inline void merge(Image& image, Image& image2, bool biggest) {
    if (biggest) {
        int newWidth = std::max(image.width, image2.width);
        int newHeight = std::max(image.height, image2.height);

        Image result(newWidth, newHeight);

        for (int i = 0; i < newWidth; i++) {
            for (int j = 0; j < newHeight; j++) {
                bool inImage1 = (i < image.width && j < image.height);
                bool inImage2 = (i < image2.width && j < image2.height);

                if (inImage1 && inImage2) {
                    for (int k = 0; k < 3; k++) {
                        result(i, j, k) = (image(i, j, k) + image2(i, j, k)) / 2;
                    }
                } else if (inImage1) {
                    for (int k = 0; k < 3; k++) {
                        result(i, j, k) = image(i, j, k);
                    }
                } else if (inImage2) {
                    for (int k = 0; k < 3; k++) {
                        result(i, j, k) = image2(i, j, k);
                    }
                } else {
                    for (int k = 0; k < 3; k++) {
                        result(i, j, k) = 0;
                    }
                }
            }
        }
        image = result;
    } else {
        int newWidth = std::min(image.width, image2.width);
        int newHeight = std::min(image.height, image2.height);

        Image result(newWidth, newHeight);

        for (int i = 0; i < newWidth; i++) {
            for (int j = 0; j < newHeight; j++) {
                for (int k = 0; k < 3; k++) {
                    result(i, j, k) = (image(i, j, k) + image2(i, j, k)) / 2;
                }
            }
        }
        image = result;
    }
}

// ============ Filter 10: Edge Detection ============
inline void edges(Image& image) {
    Image temp(image.width, image.height);

    for (int i = 0; i < temp.width; i++) {
        for (int j = 0; j < temp.height; j++) {
            for (int k = 0; k < 3; k++) {
                temp(i, j, k) = 255;
            }
        }
    }

    for (int i = 0; i < image.width - 1; i++) {
        for (int j = 0; j < image.height - 1; j++) {

            int tempR = image.getPixel(i, j, 0);
            int tempG = image.getPixel(i, j, 1);
            int tempB = image.getPixel(i, j, 2);
            int pixel_avg = (tempR + tempB + tempG) / 3;

            int tempR2 = image.getPixel(i + 1, j, 0);
            int tempG2 = image.getPixel(i + 1, j, 1);
            int tempB2 = image.getPixel(i + 1, j, 2);
            int pixel_avg2 = (tempR2 + tempB2 + tempG2) / 3;

            int tempR3 = image.getPixel(i, j + 1, 0);
            int tempG3 = image.getPixel(i, j + 1, 1);
            int tempB3 = image.getPixel(i, j + 1, 2);
            int pixel_avg3 = (tempR3 + tempB3 + tempG3) / 3;

            int threshold = 40;

            if (std::abs(pixel_avg2 - pixel_avg) >= threshold ||
                std::abs(pixel_avg3 - pixel_avg) >= threshold) {
                for (int k = 0; k < 3; k++) {
                    temp(i, j, k) = 0;
                }
            }
        }
    }
    image = temp;
}

// ============ Filter 11: Crop ============
inline void crop(Image& image_to_filter, int x0, int y0, int w, int h) {
    int x = x0 + 1;
    int y = y0 + 1;

    if (x <= 0 || y <= 0 || x > image_to_filter.width || y > image_to_filter.height) return;
    if (w <= 0 || h <= 0) return;

    if (x + (w - 1) > image_to_filter.width) {
        w = image_to_filter.width - x;
    }
    if (y + (h - 1) > image_to_filter.height) {
        h = image_to_filter.height - y;
    }
    if (w <= 0 || h <= 0) return;

    Image cropped_image(w, h);
    for (int i = x; i < x + w; i++) {
        for (int j = y; j < y + h; j++) {
            for (int k = 0; k < 3; k++) {
                cropped_image(i - x, j - y, k) = image_to_filter(i - 1, j - 1, k);
            }
        }
    }
    image_to_filter = cropped_image;
}

// ============ Filter 12: Blur ============
inline void blur(Image& img, int radius) {
    Image blurred = img;

    for (int i = radius; i < img.width - radius; i++) {
        for (int j = radius; j < img.height - radius; j++) {
            for (int k = 0; k < 3; k++) {
                int sum = 0;
                int count = 0;

                for (int dx = -radius; dx <= radius; dx++) {
                    for (int dy = -radius; dy <= radius; dy++) {
                        sum += img(i + dx, j + dy, k);
                        count++;
                    }
                }
                blurred(i, j, k) = sum / count;
            }
        }
    }
    img = blurred;
}

// ============ Filter 13: Sunlight ============
inline void sunlight(Image& image) {
    for (int i = 0; i < image.width; i++) {
        for (int j = 0; j < image.height; j++) {
            for (int k = 0; k < 3; k++) {

                int Red = image(i, j, k) + 20;
                int Green = image(i, j, k) + 10;
                int Blue = image(i, j, k) - 10;

                Red = std::min(255, Red);
                Green = std::min(255, Green);
                Blue = std::max(0, Blue);

                image(i, j, 0) = Red;
                image(i, j, 1) = Green;
                image(i, j, 2) = Blue;
            }
        }
    }
}

// ============ Filter 14: OldTV ============
inline void oldTV(Image& image) {
    for (int i = 0; i < image.width; i++) {
        for (int j = 0; j < image.height; j++) {
            if (j % 2) {
                for (int k = 0; k < 3; k++) {
                    image(i, j, k) = 0;
                }
            }
        }
    }
}

// ============ Filter 15: Purple ============
inline void purple(Image& image_to_filter) {
    for (int i = 0; i < image_to_filter.width; i++) {
        for (int j = 0; j < image_to_filter.height; j++) {
            image_to_filter(i, j, 1) *= 0.64;
        }
    }
}

// ============ Filter 16: Infrared ============
inline void infrared(Image& img) {
    for (int i = 0; i < img.width; i++) {
        for (int j = 0; j < img.height; j++) {
            img(i, j, 0) = 255;
            img(i, j, 1) = 255 - img(i, j, 1);
            img(i, j, 2) = 255 - img(i, j, 2);
        }
    }
}

// ============ Filter 17: Skew ============
inline void skew(Image& image, double angle) {
    double radian = angle * M_PI / 180.0;
    double tanValue = std::tan(radian) * 0.5;   // Adjusting the skew factor for better visual effect
    int skewness = (int)std::floor(image.height * tanValue);

    int newwidth = image.width + std::abs(skewness);
    int height = image.height;

    Image skewed(newwidth, height);

    for (int x = 0; x < newwidth; x++) {
        for (int y = 0; y < height; y++) {
            for (int k = 0; k < 3; k++) {
                skewed(x, y, k) = 255;
            }
        }
    }

    int base = (skewness < 0) ? -skewness : 0;

    for (int x = 0; x < image.width; x++) {
        for (int y = 0; y < height; y++) {
            int newX = x + (int)std::floor(y * tanValue) + base;
            if (newX >= 0 && newX < newwidth) {
                for (int k = 0; k < 3; k++) {
                    skewed(newX, y, k) = image(x, y, k);
                }
            }
        }
    }

    image = skewed;
}



// ============ Filter 18: Oil Painting ============
inline void oilPainting(Image& image, int radius) {
    const int levels = 20;

    for (int x = 0; x < image.width; x++) {
        for (int y = 0; y < image.height; y++) {

            int count[levels] = {0};
            int sumR[levels] = {0};
            int sumG[levels] = {0};
            int sumB[levels] = {0};

            for (int dx = -radius; dx <= radius; dx++) {
                for (int dy = -radius; dy <= radius; dy++) {
                    int nx = x + dx;
                    int ny = y + dy;

                    if (nx < 0 || nx >= image.width || ny < 0 || ny >= image.height)
                        continue;

                    int r = image.getPixel(nx, ny, 0);
                    int g = image.getPixel(nx, ny, 1);
                    int b = image.getPixel(nx, ny, 2);

                    int intensity = (r + g + b) / 3;
                    int bin = intensity * levels / 256;

                    count[bin]++;
                    sumR[bin] += r;
                    sumG[bin] += g;
                    sumB[bin] += b;
                }
            }

            int best = 0;
            for (int i = 1; i < levels; i++) {
                if (count[i] > count[best])
                    best = i;
            }

            image.setPixel(x, y, 0, sumR[best] / count[best]);
            image.setPixel(x, y, 1, sumG[best] / count[best]);
            image.setPixel(x, y, 2, sumB[best] / count[best]);
        }
    }
}

}