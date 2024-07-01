
#include <complex>

#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

#include <vector>

using namespace std;
using namespace cv;
#pragma pack(push, 1) // Ensure that structures are byte-aligned
struct BMPHeader {
    uint16_t type; // Magic identifier "BM"
    uint32_t size; // File size
    uint32_t reserved; // Reserved
    uint32_t offset; // Offset to image data
    uint32_t dibHeaderSize; // DIB Header size
    int32_t width; // Image width
    int32_t height; // Image height (positive for bottom-up)
    uint16_t planes; // Number of color planes (must be 1)
    uint16_t bitsPerPixel; // Bits per pixel
    uint32_t compression; // Compression method
    uint32_t imageSize; // Image data size
    int32_t xPixelsPerMeter; // Horizontal pixels per meter
    int32_t yPixelsPerMeter; // Vertical pixels per meter
    uint32_t colorsUsed; // Number of colors in the palette
    uint32_t colorsImportant; // Important colors
};
#pragma pack(pop)

Mat Mult(Mat input1, Mat input2, bool conj)
{
	if ((input1.channels() != 2) | (input2.channels() != 2)) {
		std::cout << "The channel of input1 and input2 should be 2!" << std::endl;
		return Mat();
	}
	Mat src1 = input1.clone();
	Mat src2 = input2.clone();
	Mat F[2];
	split(src1, F);
	Mat G[2];
	split(src2, G);
    if(conj){
        G[1] = G[1]*-1;
    }
	Mat ac;
	multiply(F[0], G[0], ac);
	Mat bd;
	multiply(F[1], G[1], bd);
	Mat Real = ac - bd;
	Mat ad;
	multiply(F[0], G[1], ad);
	Mat bc;
	multiply(F[1], G[0], bc);
	Mat Im = ad + bc;
	Mat value[2] = { Real,Im };
	Mat result;
	merge(value, 2, result);
	return result;
}




Mat Div(Mat input1, Mat input2)
{
	if ((input1.channels() != 2) | (input2.channels() != 2)) {
		std::cout << "The channel of input1 and input2 should be 2!" << std::endl;
		return Mat();
	}
	Mat src1 = input1.clone();
	Mat src2 = input2.clone();
	Mat F[2];
	split(src1, F);
	Mat G[2];
	split(src2, G);
	Mat a = F[0];
	Mat b = F[1];
	Mat c = G[0];
	Mat d = G[1];
	Mat c2;
	multiply(c, c, c2);
	Mat d2;
	multiply(d, d, d2);
	Mat ac;
	multiply(a, c, ac);
	Mat bd;
	multiply(b, d, bd);
	Mat bc;
	multiply(b, c, bc);
	Mat ad;
	multiply(a, d, ad);
	Mat real;
	divide(ac + bd, c2 + d2, real);
	Mat im;
	divide(bc - ad, c2 + d2, im);
	Mat value[2] = { real,im };
	Mat result;
	merge(value, 2, result);
	return result;
}
void fftshift(const Mat& inputImg, Mat& outputImg){
    outputImg = inputImg.clone();
    int cx = floor(outputImg.cols / 2);
    int cy = floor(outputImg.rows / 2);
    Mat q0(outputImg, Rect(0, 0, cx, cy));
    Mat q1(outputImg, Rect(cx, 0, cx, cy));
    Mat q2(outputImg, Rect(0, cy, cx, cy));
    Mat q3(outputImg, Rect(cx, cy, cx, cy));
    Mat tmp;
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);
    q1.copyTo(tmp);
    q2.copyTo(q1);
    tmp.copyTo(q2);
}


cv::Mat getMotionPSF(cv::Size shape, double angle, int dist) {
    double xCenter = (shape.width - 1) / 2.0;
    double yCenter = (shape.height - 1) / 2.0;
    double sinVal = sin(angle * CV_PI / 180.0);
    double cosVal = cos(angle * CV_PI / 180.0);
    int count = 0;
    cv::Mat PSF = cv::Mat::zeros(cv::Size(shape.height,shape.width), CV_64F);

    for (int i = 0; i < dist; i++) {
        double xOffset = (sinVal * i);
        double yOffset = (cosVal * i);
        PSF.at<double>(cvRound(xCenter - xOffset),cvRound(yCenter + yOffset)) = 1;
        count++;
    }

    Mat PSF_1(PSF);
    return PSF / count;
}

cv::Mat wienerFilter(cv::Mat input, cv::Mat PSF, double eps, double K=0.02) {
    cv::Mat fftImg, fftPSF, filteredResult, temp_img, final_img;
    
    cv::dft(input, fftImg, cv::DFT_COMPLEX_OUTPUT);
    fftshift(PSF, PSF);
    cv::dft(PSF, fftPSF, cv::DFT_COMPLEX_OUTPUT);
 
   
    fftPSF += eps;
    Mat square;
    square = Mult(fftPSF, fftPSF, true);
    //cv::mulSpectrums(fftPSF, fftPSF, square, 0, true);

    vector <Mat> square_K_temp;
    Mat square_K;
    split(square,square_K_temp);
    vector <Mat> fftPSF_i_v;
    Mat fftPSF_i;
    split(fftPSF,fftPSF_i_v);
    fftPSF_i_v[1] *= -1;
    merge(fftPSF_i_v,fftPSF_i);
    //cv::addWeighted(square_K_temp[0], 1.0, cv::Scalar::all(K), 1.0,0.0, square_K_temp[0]);
    square_K_temp[0] += K;

    merge(square_K_temp,square_K);
    Mat temp0, temp1;
    temp0 = Mult(fftImg, fftPSF_i, false);
    filteredResult = Div(temp0, square_K);

    cv::idft(filteredResult, temp_img,  cv::DFT_SCALE |cv::DFT_REAL_OUTPUT);
    //fftshift(temp_img, final_img);
    cv::Mat absResult = abs(temp_img);
    return absResult;
}

int main(int argc, char *argv[]){

    string str_input1 = argv[1];
    string str_out_filename = argv[2];

    std::ifstream inputFile(str_input1, std::ios::binary);
    if (!inputFile) {
        std::cerr << "[Error] Failed to open input BMP file." << std::endl;
        return 1;
    }

    //* Read the input bmp header
    BMPHeader header;
    inputFile.read(reinterpret_cast<char*>(&header), sizeof(BMPHeader));
    int channel = header.bitsPerPixel/8;
    //* Read the input bmp file
    const int imageSize = header.width * header.height * channel;
    std::vector<char> inputData(imageSize);
    inputFile.read(inputData.data(), imageSize);
    inputFile.close();


    // Create Mat for B, G, R channels
    Mat input_b = cv::Mat::zeros(cv::Size(header.width,header.height), CV_8U);
    Mat input_g = cv::Mat::zeros(cv::Size(header.width,header.height), CV_8U);
    Mat input_r = cv::Mat::zeros(cv::Size(header.width,header.height), CV_8U);
    // fill out the B, G, R mat with input image (uint8)
    for (int y = 0; y < header.height; y++) {
        for (int x = 0; x < header.width; x++) {
            input_b.at<uint8_t>(y,x) = inputData[(((header.height-y-1)*header.width)+ x)*channel+0];
            input_g.at<uint8_t>(y,x) = inputData[(((header.height-y-1)*header.width)+ x)*channel+1];
            input_r.at<uint8_t>(y,x) = inputData[(((header.height-y-1)*header.width)+ x)*channel+2];
        }
    }

    vector <Mat> input_img;
    input_img.push_back(input_b);
    input_img.push_back(input_g);
    input_img.push_back(input_r);

    // guess PSF
    Mat psf = getMotionPSF(cv::Size(header.height,header.width),37,25);
    
    // convert Mat data type to 64-floating
    input_b.convertTo(input_b,CV_64FC1);
    input_g.convertTo(input_g,CV_64FC1);
    input_r.convertTo(input_r,CV_64FC1);
    
    
    double scale =  0.075;
    Mat r_Mat, g_Mat, b_Mat;
    // apply wiener filter
    b_Mat = wienerFilter(input_b, psf, scale);
    g_Mat = wienerFilter(input_g, psf, scale);
    r_Mat = wienerFilter(input_r, psf, scale);
    // convert Mat data type back to 8-bit unsigned int
    b_Mat.convertTo(b_Mat, CV_8UC1);
    g_Mat.convertTo(g_Mat, CV_8UC1);
    r_Mat.convertTo(r_Mat, CV_8UC1);

    // merge BGR Mats into a single Mat
    Mat imgOut1;
    vector<Mat> imgOut1_channels;
    imgOut1_channels.push_back(b_Mat);
    imgOut1_channels.push_back(g_Mat);
    imgOut1_channels.push_back(r_Mat);
    merge(imgOut1_channels,imgOut1);

    // Copy the result Mat to vector container and inverse vertically
    vector <uint8_t> imageData;
    vector <uint8_t> inverse_image(imageSize);
    imageData.assign(imgOut1.datastart,imgOut1.dataend);
    for(int y=0; y<header.height;y++){
        for(int x=0;x<header.width;x++){
            for(int ch=0;ch<channel;ch++){
                inverse_image[((y*header.width)+ x)*channel+ch] =  
                    imageData[(((header.height-y-1)*header.width)+ x)*channel+ch];
            }
        }
    }




    vector <uint8_t> avgfileter_image(imageSize);
    vector <uint8_t> sharpen_image(imageSize);


    int avg_ky_size = 1, avg_kx_size = 1;

    for (int loop_time = 0; loop_time<3;loop_time++){
        for (int y = avg_ky_size; y < header.height - avg_ky_size; y++) {
            for (int x = avg_kx_size; x < header.width - avg_kx_size; x++) {
                for(int ch = 0; ch < channel; ch++){
                    int byte_data=0;
                    for (int ky = avg_ky_size*-1; ky <= avg_ky_size; ky++) {
                        for (int kx = avg_kx_size*-1; kx <= avg_kx_size; kx++) {
                                const int temp =  
                                    (uint8_t)imageData[(((y+ky)*header.width)+ (x+kx))*channel+ch];
                                byte_data += temp;
                            }
                        }
                        avgfileter_image[(((header.height - 1-y)*header.width)+ x)*channel+ch]= 
                            (uint8_t)(byte_data/((1+2*avg_kx_size)*(1+2*avg_ky_size)));
                }
            }
        }
    }
    // sharpen kernel
    int kernel[3][3] = {{-1, -1, -1}, {-1, 9, -1}, {-1, -1, -1}};

    for (int loop_time = 0; loop_time<1;loop_time++){
        for (int y = 1; y < header.height - 1; y++) {
            for (int x = 1; x < header.width - 1; x++) {
                for(int ch = 0; ch < channel; ch++){
                    int byte_data=0;
                    for (int ky = -1; ky <= 1; ky++) {
                        for (int kx = -1; kx <= 1; kx++) {
                            byte_data += ((uint8_t)avgfileter_image[(((y+ky)*header.width)+ (x+kx))*channel+ch] * kernel[ky + 1][kx + 1]);
                        }
                    }
                    sharpen_image[(((y)*header.width)+ x)*channel+ch]= (uint8_t)fmin(fmax(byte_data, 0), 255);
                }
            }
        }
    }


    std::ofstream outputFile(str_out_filename, std::ios::binary);
    if (!outputFile) {
        std::cerr << "[Error] Failed to create output BMP file." << std::endl;
        return 1;
    }

    //* Output the header and image raw data
    outputFile.write(reinterpret_cast<const char*>(&header), sizeof(BMPHeader));
    outputFile.write((const char *)inverse_image.data(), imageSize);
    outputFile.close();

    std::cout << "Image is sharpen and saved as "<< str_out_filename << std::endl;

    return 0;
}
