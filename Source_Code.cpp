
/*
Project Discription:
This project was required by Dr.Mohammed EL-Ramly for assignment one for Object Oriented Programming course (CS213).
It is for image processing which the user can upload any image using its file name with extensions like :(.jpg, .bmp, .png, ....etc)
and the user can save the image filtered with a customized file name and extension.
The available filters are:
1 --> Greyscale
2 --> Black & White
3 --> Invert Image
4 --> Frame
5 --> Flip
6 --> Rotate
7 --> Darken / Lighten
8 --> Resize
9 --> Merge Images
10 --> Detect Edges
11 --> Crop
12 --> Blur
13 --> Sunlight
14 --> OldTV
15 --> Purple
16 --> Infrared
17 --> Image Skewing
18 --> Oil Painting

This project is made by:

* Main 16 Filters:
Mahmoud Waleed Saad => ID:20250807 => Filters 1, 5, 9, 13

Ahmed Ismail Omran => ID:20251006 => Filters 2, 6, 10, 14

Ahmed Walid Badr => ID:20251035 => Filters 3, 7, 11, 15

Mohammed Mokhtar Mohammed => ID:20251350 => Filters 4, 8, 12, 16

* Bonus:
Filters 17, 18 => All Team

GUI => All Team

*/





#include <iostream>
#include <string>
#include "Image_Class.h"
using namespace std;


// ============= GreyScale: Filter 1 =============

void applyGreyscale(Image& image) {
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


// =============== Black and White: Filter 2 ================

void applyBlackandWhite(Image& image) {
    for (int i=0; i<image.width; i++) {

        for (int j=0; j<image.height; j++ ) {

            unsigned int avg = 0;
            unsigned int x;
            for (int k=0; k<3; k++) {
                avg += image(i, j, k);
                x = image(i, j, k)>=128;
            }
            avg/=3;

            for (int k=0; k<3;k++) {
                image(i, j, k)=avg;
            }

            for (int k=0; k<3; k++) {
                if (x==1) {
                    image.setPixel(i, j, k, 255);
                }
                else {
                    image.setPixel(i, j, k, 0);
                }
            }
        }
    }
}


// ============= Invert: Filter 3 =============

void Invert_filter(Image &image_to_filter)
{
    for (int i=0;i<image_to_filter.width;i++)
    {
        for (int j=0;j<image_to_filter.height;j++)
        {
            for (int k=0;k<3;k++)
            {
                image_to_filter(i,j,k)=255-image_to_filter(i,j,k);
            }
        }
    }
}
// ============= Frame: Filter 4 =============

void addSimpleFrame(Image& img) {
    int thickness;
    cout << "Enter frame thickness: ";
    cin >> thickness;

    int r, g, b;
    cin >> r >> g >> b;

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

void addFancyFrame(Image& img) {
    int thickness;
    cout << "Enter frame thickness: ";
    cin >> thickness;

    int r, g, b;
    cin >> r >> g >> b;

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

void Frame_Filter(Image &img)
{
    cout << "Choose Frame Type:" <<'\n' ;
    cout << "1. Simple Frame" << '\n' ;
    cout << "2. Fancy/Decorative Frame" << '\n' ;
    cout << "Enter choice (1 or 2): ";

    Frame_Choice:
    int choice;
    cin >> choice;

    if (choice == 1) {
        addSimpleFrame(img);
    } else if (choice == 2) {
        addFancyFrame(img);
    } else {
        cout << "Invalid choice!" << '\n' ;
        goto Frame_Choice;
    }
}


// ============= Flip: Filter 5 =============

void applyFlip(Image &image) {
    int flip_choice;
    cout << "Enter 1 for horizontal flip or 2 for vertical flip: ";
    cin >> flip_choice;

    if (flip_choice == 1) { // Horizontal Flip
        for (int i = 0; i < image.width / 2; ++i) {
            for (int j = 0; j < image.height; ++j) {
                for (int k = 0; k < 3; ++k) {
                    swap(image(i, j, k), image(image.width - 1 - i, j, k));
                }
            }
        }
    } else if (flip_choice == 2) { // Vertical Flip
        for (int i = 0; i < image.width; ++i) {
            for (int j = 0; j < image.height / 2; ++j) {
                for (int k = 0; k < 3; ++k) {
                    swap(image(i, j, k), image(i, image.height - 1 - j, k));
                }
            }
        }
    } else {
        cout << "Invalid choice! Please enter either 1 or 2.\n";
    }
}


// ============= Rotate Filter: Filter 6 =============

void applyRotation(Image &image) {


    int degree;
    cout << "Enter the degree of rotation (90, 180, 270):" <<'\n';
    cin >> degree;

    if (degree == 180) {
        for (int i=0;i<image.width;i++)
        {
            for (int j=0;j<image.height/2;j++)
            {
                int tempR = image.getPixel(i, j, 0);
                int tempG = image.getPixel(i, j, 1);
                int tempB = image.getPixel(i, j, 2);

                for (int k=0;k<3;k++) {
                    image(i, j, k) = image(image.width-1-i, image.height-1-j, k);
                }

                image(image.width-1-i, image.height-1-j, 0) = tempR;
                image(image.width-1-i, image.height-1-j, 1) = tempG;
                image(image.width-1-i, image.height-1-j, 2) = tempB;

            }
        }
    }


    else if (degree == 90) {
        Image image2(image.height, image.width);

        for (int i=0;i<image.width;i++)
        {
            for (int j=0;j<image.height;j++)
            {

                int new_i = image.height-1-j ;
                int new_j = i;

                for (int k=0; k<3; k++) {
                    image2(new_i, new_j, k) = image(i, j, k);
                }

            }
        }

        image = image2;

    }
    else if (degree == 270) {
        Image image2 (image.height, image.width);

        for (int i=0 ; i<image.width; i++) {
            for (int j=0; j<image.height; j++) {

                int new_i = j;
                int new_j = image.width-1-i;

                for (int k=0; k<3; k++) {
                    image2(new_i, new_j, k) = image(i, j, k);
                }

            }

        }


        image = image2;

    }
    else {
        cout << "Error! please choose one of these degrees (90, 180, 270)" << '\n';
    }

}


// ============= Darken/Lighten: Filter 7 =============

void Darkening_filter(Image &image_to_filter)
{
    cout<<"Enter Darkening Level from 0 to 100: ";
    int level;
    cin>>level;
    cout<<'\n';

    for (int i=0;i<image_to_filter.width;i++)
    {
        for (int j=0;j<image_to_filter.height;j++)
        {
            for (int k=0;k<3;k++)
            {
                image_to_filter(i,j,k)*=1-(level/100.0f);
            }
        }
    }
}


void Lightening_filter(Image &image_to_filter)
{
    cout<<"Enter Lightening Level from 0 to 100: ";
    int level;
    cin>>level;
    cout<<'\n';

    for (int i=0;i<image_to_filter.width;i++)
    {
        for (int j=0;j<image_to_filter.height;j++)
        {
            for (int k=0;k<3;k++)
            {
                float changes_to_lighten=image_to_filter(i,j,k)*(1+(level/100.0f));
                image_to_filter(i,j,k)=min(255.0f,changes_to_lighten);
            }
        }
    }
}


void Darken_Lighten_filter(Image &image_to_filter)
{
    dark_or_light:
    cout<<"Type D to Darken & L to Lighten: ";
    string Brightness_Type;
    cin>>Brightness_Type;
    cout<<'\n';

    if (Brightness_Type[0] == 'D' || Brightness_Type[0] == 'd') Darkening_filter(image_to_filter);
    else if (Brightness_Type[0] == 'L' || Brightness_Type[0] == 'l') Lightening_filter(image_to_filter);
    else goto dark_or_light;

}


// ============ Resize: Filter 8 ============

void resizeFilter(Image& image) {
    int Width, Height;
    cout << "Enter new width: ";
    cin >> Width;
    cout << "Enter new height: ";
    cin >> Height;

    if (Width <= 0 || Height <= 0) {
        cout << "Fail!" << endl;
        return;
    }

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
    cout << "Image resized successfully!" << endl;
}


// ============ Merge: Filter 9 ============

void merge(Image &image) {

    string filename2;
    cout << "Pls enter the second image name to merge with the first image: ";
    cin >> filename2;

    Image image2(filename2);

    if (image2.channels < 3) {
        cout << "The second image must have at least 3 channels (RGB) to merge." << endl;
        return;
    }

    int choice;
    cout << "1 --> Merge Using the biggest dimensions\n";
    cout << "2 --> Merge Using the smallest dimensions\n";
    cout << "Enter your choice: ";
    cin >> choice;

    if (choice == 1) {
        int newWidth = max(image.width,image2.width);
        int newHeight = max(image.height,image2.height);

        Image result(newWidth, newHeight);

        for (int i = 0; i< newWidth;i++) {
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
                }
            }
        }
        image = result;
    } else if (choice == 2) {
        int newWidth = min(image.width,image2.width);
        int newHeight = min(image.height,image2.height);

        Image result(newWidth, newHeight);

        for (int i = 0; i< newWidth;i++) {
            for (int j = 0; j < newHeight; j++) {
                for (int k = 0; k < 3; k++) {
                    result(i, j, k) = (image(i, j, k) + image2(i, j, k)) / 2;
                }
            }
        }
        image = result;
    } else {
        cout << "Invalid choice. Please select either 1 or 2." << endl;
    }
}


// ============ Edge Detection: Filter 10 ============

void EdgeDetection(Image &image) {
    // applyGreyscale(image);

    Image temp(image.width, image.height);

    // Initialize the temp image all pixels= white
    for (int i=0; i<temp.width; i++) {
        for (int j=0; j<temp.height; j++) {
            for (int k=0; k<3; k++) {
                temp(i, j, k) = 255;
            }
        }
    }


    // Detect the edge in the original image:
    for (int i=0; i<image.width-1; i++ ) {
        for (int j=0; j<image.height-1; j++) {

            int tempR = image.getPixel(i, j, 0);
            int tempG = image.getPixel(i, j, 1);
            int tempB = image.getPixel(i, j, 2);

            int pixel_avg = (tempR + tempB + tempG) /3;

            int tempR2 = image.getPixel(i+1, j, 0);
            int tempG2 = image.getPixel(i+1, j, 1);
            int tempB2 = image.getPixel(i+1, j, 2);


            int pixel_avg2 = (tempR2 + tempB2 + tempG2) /3;


            int tempR3 = image.getPixel(i, j+1, 0);
            int tempG3 = image.getPixel(i, j+1, 1);
            int tempB3 = image.getPixel(i, j+1, 2);

            int pixel_avg3 = (tempR3 + tempB3 + tempG3) /3;


            int threshold = 40;

            if (abs(pixel_avg2 - pixel_avg) >= threshold || abs(pixel_avg3 - pixel_avg) >= threshold) {
                for (int k=0; k<3; k++) {
                    temp(i, j, k)  = 0;
                }
            }
        }
    }


    image = temp;
}


// ============ Crop: Filter 11 ============

void Crop_filter(Image &image_to_filter)
{
    cout<<"Enter Crop starting point (x,y) respectively: ";
    starting_point:
    int x,y;
    cin>>x>>y;
    cout<<'\n';
    if (x<=0 || y<=0 || x > image_to_filter.width || y > image_to_filter.height)
    {
        cout<<"Enter valid starting coordinate: ";
        goto starting_point;
    }

    cout<<"Enter the width & height of the rectangle to crop: ";
    cropping_rectangle:
    int w,h;
    cin>>w>>h;
    cout<<'\n';
    if (w<=0 || h<=0) // w=0 & h=0 means there is no pixels ---> no image
    {
        cout<<"Enter valid width and height: ";
        goto cropping_rectangle;
    }
    if (x+(w-1) > image_to_filter.width) // We have the width w = (pixel x) + (w-1 pixels)
    {
        w=image_to_filter.width-x; // [---------x--------------TotalWidth-----w] so make w the distance between x & TotalWidth
    }
    if (y+(h-1) > image_to_filter.height) // We have the height h = (pixel y) + (h-1 pixels)
    {
        h=image_to_filter.height-y; // [------y---------TotalHeight----h] so make h the distance between y & TotalHeight
    }


    Image cropped_image(w,h);
    for (int i=x;i<x+w;i++) // x here is 1 based
    {
        for (int j=y;j<y+h;j++) // y here is 1 based
        {
            for (int k=0;k<3;k++) cropped_image(i-x,j-y,k)=image_to_filter(i-1,j-1,k);
        }
    }
    image_to_filter=cropped_image;
}


// ============ Sunlight: Filter 13 ============

void Sunlight(Image &image) {
    for (int i = 0; i < image.width ;i++) {
        for (int j = 0; j < image.height; j++) {
            for (int k = 0; k < 3 ; k++) {

                int Red = image(i,j,k) + 20;
                int Green = image(i,j,k) + 10;
                int Blue = image(i,j,k) - 10;

                Red = min(255, Red);
                Green = min(255, Green);
                Blue = max(0, Blue);

                image(i,j,0) = Red;
                image(i,j,1) = Green;
                image(i,j,2) = Blue;
            }
        }
    }
}


// ============ OldTV: Filter 14 ============

void OldTV(Image &image)
{
    for (int i=0; i<image.width; i++) {
        for (int j=0; j<image.height; j++) {
            if (j%2) {
                for (int k=0; k<3; k++) {
                    image(i, j, k) = 0;
                }
            }
        }
    }
}


// ============ Purple: Filter 15 ============

void Purple_Filter(Image &image_to_filter)
{
    for (int i=0;i<image_to_filter.width;i++)
    {
        for (int j=0;j<image_to_filter.height;j++)
        {
            image_to_filter(i,j,1)*=0.64; //Purple is the combination of Red and Blue so keep Green zero and the degrees of purple would form
        }
    }
}


// ============ Infrared: Filter 16 ============

void applyInfraredFilter(Image& img) {
    for (int i = 0; i < img.width; i++) {
        for (int j = 0; j < img.height; j++) {
            img(i, j, 0) = 255;

            img(i, j, 1) = 255 - img(i, j, 1);

            img(i, j, 2) = 255 - img(i, j, 2);
        }
    }
}


// ============ Skew: Filter 17 ============

void Skewing_Pixels(Image& image) {
    cout<<"Enter skew angle: ";
    double angle;
    cin>>angle;
    double radian = angle * M_PI / 180.0;
    double tanValue = tan(radian)  * 0.5; // Adjusting the skew factor for better visual effect
    int skewness = floor(image.height * tanValue);

    int newwidth = image.width + abs(skewness);
    int height = image.height;

    Image skewed(newwidth, height);

    for (int x = 0; x < newwidth; x++) {
        for (int y = 0; y < height; y++) {
            for (int k = 0; k < 3; k++) {
                skewed(x, y, k) = 255; // Initialize to White
            }
        }
    }

    for (int y = 0; y < height; y++) {
        int shift = static_cast<int>((image.height - 1 - y) * tanValue);
        for (int x = 0; x < image.width; x++) {
            for (int k = 0; k < 3; k++) {
                skewed(x + shift, y, k) = image(x, y, k);
            }
        }
    }

    image = skewed;
}


// ============ Oil Painting: Filter 18 ============

void oilPainting(Image &image) {
    int radius = 4;
    int levels = 20;

    for (int x = 0; x < image.width; x++) {
        for (int y = 0; y < image.height; y++) {

            int count[20] = {0};
            int sumR[20] = {0};
            int sumG[20] = {0};
            int sumB[20] = {0};

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




// ============ Inputting Image File ============
    void load_image(Image &image)
{
    string filename;
    Enter_Image:
    cout << "Enter the image filename - No Spaces: ";
    cin.ignore();
    getline(cin, filename);
    Image temp(filename);

    if (image.channels < 3) {
        cout << "The image must be a colored RGB image.\n";
        goto Enter_Image;
    }
    image = temp;
}





// ============ Saving Filtered Image ============
void save_image(Image &image)
{
    cout << "Enter the output image filename & Specify extension: ";
    cin.ignore();
    string filtered_filename;
    getline(cin, filtered_filename);
    if (image.saveImage(filtered_filename)) {
        cout << "Image saved successfully.\n";
    }
    else {
        cout << "Failed to save the image.\n";
    }
}




// ============ Menu ============

void Menu() {
    cout << "========================================\n";
    cout << "         Image Filters Menu            \n";
    cout << "========================================\n";
    cout << "0 --> Load Image\n";
    cout << "1 --> Greyscale\n";
    cout << "2 --> Black & White\n";
    cout << "3 --> Invert Image\n";
    cout << "4 --> Frame\n";
    cout << "5 --> Flip\n";
    cout << "6 --> Rotate\n";
    cout << "7 --> Darken / Lighten\n";
    cout << "8 --> Resize\n";
    cout << "9 --> Merge Images\n";
    cout << "10 --> Detect Edges\n";
    cout << "11 --> Crop\n";
    cout << "12 --> Blur\n";
    cout << "13 --> Sunlight\n";
    cout << "14 --> OldTV\n";
    cout << "15 --> Purple\n";
    cout << "16 --> Infrared\n";
    cout << "17 --> Image Skewing\n";
    cout << "18 --> Oil Painting\n";
    cout << "19 --> Save Image\n";
    cout << "20 --> Exit\n";
    cout << "========================================\n";
}

int main() {
// ============ Selecting Filter ============
    Image image(100,100);
    int choice;
    while (true) {
        Menu();
        cout << "Enter your choice: ";
        cin >> choice;
        if (choice == 20) {
            cout << "Do you want to save? y or n";
            string answer;
            cin >> answer;
            if (answer[0] == 'y')
            {
                save_image(image);
            }
            cout << "Exiting the program... Goodbye!\n";
            break;
        }


        switch (choice) {
            case 0:
                load_image(image);
                break;
            case 1:
                applyGreyscale(image);
                break;
            case 2:
                applyBlackandWhite(image);
                break;
            case 3:
                Invert_filter(image);
                break;
            case 4:
                Frame_Filter(image);
                break;
            case 5:
                applyFlip(image);
                break;
            case 6:
                 applyRotation(image);
                break;
            case 7:
                Darken_Lighten_filter(image);
                break;
            case 8:
                resizeFilter(image);
                break;
            case 9:
                merge(image);
                break;
            case 10:
                EdgeDetection(image);
                break;
            case 11:
                Crop_filter(image);
                break;
            case 12:
                //Blur
                break;
            case 13:
                Sunlight(image);
                break;
            case 14:
                 OldTV(image);
                 break;
            case 15:
                 Purple_Filter(image);
                 break;
            case 16:
                 applyInfraredFilter(image);
                 break;
            case 17:
                 Skewing_Pixels(image);
                 break;
            case 18:
                 oilPainting(image);
                 break;
            case 19:
                 save_image(image);
                 break;
            default:
                cout << "Invalid choice! Enter a number from 0 to 8.\n";
        }
    }
    return 0;
}