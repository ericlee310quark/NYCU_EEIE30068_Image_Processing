I run the following programs in WSL2 (Ubuntu Linux20.04 version)
----------------------------------------------


1. Low-luminosity Enhancement

Program name: sol_1.cpp
 Compile:
```$g++ sol_1.cpp -o sol_1```

Usage:
```./sol_1 [input bmp file] [output bmp filename] [gamma inverse ratio]```
會吃3個外部參數:
[input bmp file]:	input 圖片位址
[output bmp filename]: ouput 圖片位址
[gamma inverse ratio]:	gamma倒數數值 (floating type)

Example:
```./sol_1 input1.bmp output1_1.bmp 2```
輸入input1.bmp，產生gamma 1/2 的output1_2.bmp圖像
```/sol_1 input1.bmp output1_2.bmp 5```
輸入input1.bmp，產生gamma 1/5 的output1_2.bmp圖像

-----------------------------------------------------------------------------------------
II. Sharpness Enhancement

Program name: sol_2.cpp
 Compile:
```$g++ sol_2.cpp -o sol_2```	

Usage:
```./sol_2 [input bmp file] [output bmp filename] [sharpen filter apply time]```
會吃3個外部參數:
[input bmp file]:	input 圖片位址
[output bmp filename]: ouput 圖片位址
[sharpen filter apply time] : sharpen filter施加次數，越多則效果越明顯 (int type)

Example:
```./sol_2 input2.bmp output2_1.bmp 1```
輸入input2.bmp，施加1次sharpen filter，輸出output2_1.bmp
```./sol_2 input2.bmp output2_2.bmp 2```
輸入input2.bmp，施加2次sharpen filter，輸出output2_2.bmp

-----------------------------------------------------------------------------------------
III. Denoise

Program name: sol_3.cpp
 Compile:
```$g++ sol_3.cpp -o sol_3```

Usage:
```./sol_3 [input bmp file] [output bmp filename] [mean filter apply time]```
會吃3個外部參數:
[input bmp file]:	input 圖片位址
[output bmp filename]: ouput 圖片位址
[mean filter apply time] : mean filter施加次數，越多則效果越明顯(int type)


Example:
```./sol_3 input3.bmp output3_1.bmp 3```
輸入input3.bmp，施加3次mean filter，輸出output3_1.bmp
```./sol_3 input3.bmp output3_1.bmp 10```
輸入input3.bmp，施加10次mean filter，輸出output3_2.bmp