1. Image input/flip/output

Program name: sol_1.cpp
 Compile:
'''$g++ sol_1.cpp -o sol_1'''	

Usage:
'''./sol_1 [input bmp file]'''
會吃一個外部參數[input bmp file]，其input bmp 檔名必須為["input"+數字+".bmp"]

Example:
'''./sol_1 input1.bmp'''
輸入input1.bmp，產生水平翻轉後的output1_flip.bmp
'''./sol_1 input2.bmp'''
輸入input2.bmp，產生水平翻轉後的output2_flip.bmp

-----------------------------------------------------------------------------------------
II. Resolution

Program name: sol_2.cpp
 Compile:
'''$g++ sol_2.cpp -o sol_2'''	

Usage:
'''./sol_2 [input bmp file] [quantbit_num]'''
會吃兩個外部參數[input bmp file]，第一個是input bmp 檔名必須為["input"+數字+".bmp"]。
第二個則是量化成的新bit num。


Example:
'''./sol_2 input1.bmp 6'''
輸入input1.bmp，量化成6bit，輸出output1_1.bmp
'''./sol_2 input2.bmp 2'''
輸入input2.bmp，量化成2bit，輸出output2_3.bmp



-----------------------------------------------------------------------------------------
III. Scaling

Program name: sol_3.cpp
 Compile:
'''$g++ sol_3.cpp -o sol_3'''	

Usage:
'''./sol_3 [input bmp file] ["u" or "d"]'''
會吃兩個外部參數[input bmp file]，第一個是input bmp 檔名必須為["input"+數字+".bmp"]。
第二個字母u或d，u代表up-sample放大1.5倍，d代表down-sample縮小1.5倍


Example:
'''./sol_3 input1.bmp u'''
輸入input1.bmp，放大1.5倍，輸出output1_up.bmp
'''./sol_3 input2.bmp 2'''
輸入input2.bmp，縮小1.5倍，輸出output2_down.bmp