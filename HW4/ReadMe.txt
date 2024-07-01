我在WSL上使用opencv，因此需要確保有cmake、opencv(4.2.0)

1.Compile步驟:
在資料夾內，有CMakeLists.txt、main.cpp
%cmake .
%make
2.Run 步驟:
當compile結束時，會出現DIPHW4的binary執行檔

使用方式:
%./DIPHW4 [input file] [output file]
Example:
%./DIPHW4 ./input1.bmp ./output1.bmp
輸入input1.bmp，處理後輸出output1.bmp
%./DIPHW4 ./input2.bmp ./output2.bmp
輸入input2.bmp，處理後輸出output2.bmp