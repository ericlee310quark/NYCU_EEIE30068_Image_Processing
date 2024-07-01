I run the following programs in WSL2 (Ubuntu Linux20.04 version)
----------------------------------------------


I. Chromatic Adaptaion

Program name: sol_1.cpp
 Compile:
```$g++ sol_1.cpp -o sol_1```

Usage:
```./sol_1 [input bmp file] [output bmp filename] [alg type]```
會吃3個外部參數:
[input bmp file]:	input 圖片位址
[output bmp filename]: ouput 圖片位址
[alg type]:	演算法選擇，可用 "X"  (Gray World), "M" (MaxRGB) 
Example:

./sol_1 ./input/input1.bmp ./output1/output1_1.bmp X
./sol_1 ./input/input2.bmp ./output1/output2_1.bmp M
./sol_1 ./input/input3.bmp ./output1/output3_1.bmp M
./sol_1 ./input/input4.bmp ./output1/output4_1.bmp X


-----------------------------------------------------------------------------------------
II. Sharpness Enhancement

Program name: sol_2.cpp
 Compile:
```$g++ sol_2.cpp -o sol_2```	

Usage:
```./sol_2 [input bmp file] [output bmp filename] [input picture index]```
會吃3個外部參數:
[input bmp file]:	input 圖片位址
[output bmp filename]: ouput 圖片位址
[input picture index] : 輸入圖片是第幾個照片 (若是output1_1.bmp 作為輸入，需填上1；若是output2_1.bmp 作為輸入，需填上2) (int type)

Example:

./sol_2  ./output1/output1_1.bmp ./output/output1_2.bmp 1
./sol_2  ./output1/output2_1.bmp ./output/output2_2.bmp 2
./sol_2  ./output1/output3_1.bmp ./output/output3_2.bmp 3
./sol_2  ./output1/output4_1.bmp ./output/output4_2.bmp 4

