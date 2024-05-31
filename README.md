
# Legendre-dOPRF-network
A library for the Legendre PRF-based distributed OPRF

The optimal way to run experiments is to use the `go.sh` script.
Otherwise, read below how to use the makefile.

### Dependencies
`gcc, blake3`

**OSX:**  
	Install the blake3 library with homebrew  
	`brew install blake3`

**Linux:**  
	Install `cmake`  
	Clone the blake3 repo https://github.com/BLAKE3-team/BLAKE3  
	Enter the directory `BLAKE3`, and run  
```
cmake -S c -B build -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Release
cmake --build build
sudo cmake --install build
```

Blake3 should now be installed in `/usr/local/lib` and the headers in `/usr/local/include`

# How to get efficiency results for the adpated network communication

To get the results for a security level of 128, simply run `go128.sh`, then you will obtain a list of files 
`client256*.log` file which records the online phase efficiency regarding online time and communication, additionally you get estimated offline phase run time at files `offline_SH_cost.log`or `offline_Mal_cost.log`.

Similarly, you run `go256.sh` if you want to get result for a security level of 256.

# Copyright 2024 REDACTED FOR ANONYMITY

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


