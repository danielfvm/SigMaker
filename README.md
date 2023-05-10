# SigMaker
A simple tool for creating signatures of addresses in binary files. 
You can choose the size and address of the binary of which a signature 
should be made and get the pattern and mask as a result. It can also
show the amount of occurrences within the file.

[Screencast from 2023-05-11 00-30-48.webm](https://github.com/danielfvm/SigMaker/assets/23420640/c4093272-2ee2-4601-b7c4-c82731d60dea)


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
