# SigMaker
A simple tool for creating signatures of addresses in binary files.  You can choose the size and address of 
the binary of which a signature should be made and get the pattern and mask as a result. It can also show 
the amount of occurrences within the file. Currently only works for x64 applications.

![output](https://github.com/danielfvm/SigMaker/assets/23420640/2eecf107-514e-424d-8fab-40822a9c65be)


# Usage
The following three arguments are required for runnning the program. Keep in mind that the size argument
is only defines the approximate size and may be larger or shorter depending on the assembly at the address.
```
Usage:
  sigmaker <file> <address> <size>

  file     Path to executable file
  address  Create signature for selected hex address
  size     Set the size of signature
```

## Example
![image](https://github.com/danielfvm/SigMaker/assets/23420640/e96f9ca9-ccfd-42ac-ad5a-29d0f55abd66)
As you can see in the above image you will first receive the assembler instructions and their addresses (+offset)
together with the binary code in hex. The bold hex codes on the right side are used for the pattern and
will probably stay the same even if the target application has been updated. The `00` that are not highlighted
are addresses or offsets that are most likely to change after the application is being updated and
are therefore marked in the mask with a `?`. Below you can see the pattern and mask together with the
length. THese can be used for searching the address in your application. The `First result` indicates if the first
match with the generated pattern (starting with the lower addresses) is the address, if not you can make the
size larger. `Occurrences` is the amount of matches found in the target application.
Here is an example implementation that you can use for finding addresses with the generated pattern and 
mask.
```cpp
int64_t scanPattern(const PageInfo page, const char* pattern, const char* mask, int length) {
    int64_t addr;
    int i;

    for (addr = page.start; addr < page.end; ++ addr) {
        for (i = 0; i < length; ++ i) {
            if (mask[i] == '?')
                continue;

            if (pattern[i] != *((char*)(addr+i))) {
                break;
            }
        }

        if (i == length) {
            return addr;
        }
    }

    return -1;
}
```

# Installation
You will need the [udis869](https://github.com/vmt/udis86) library to compile and install this program.

## Linux
Following commands will download, compile and install this program. Keep in mind that you porbably will
need root permission for running `make install`, which will then copy the binary file to `/usr/bin/`
```
git clone https://github.com/danielfvm/SigMaker
cd SigMaker
make
make install
```
