# SigMaker
A simple tool for creating signatures of addresses in binary files. 
You can choose the size and address of the binary of which a signature 
should be made and get the pattern and mask as a result. It can also
show the amount of occurrences within the file.

![output](https://github.com/danielfvm/SigMaker/assets/23420640/2eecf107-514e-424d-8fab-40822a9c65be)


# Usage
```
Usage:
  sigmaker <file> <address> <size>

  file     Path to executable file
  address  Create signature for selected hex address
  size     Set the size of signature
```

# Installation
## Linux
```
git clone https://github.com/danielfvm/SigMaker
cd SigMaker
make
make install
```
