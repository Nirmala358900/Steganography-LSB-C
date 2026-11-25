# Image Steganography using LSB in C

Hides secret text inside a BMP image using Least Significant Bit (LSB) technique.

## How to Run
```bash
gcc test_encode.c encode.c decode.c -o stego
./stego -e beautiful.bmp secret.txt stego.bmp
./stego -d stego.bmp output.txt
## Files
- `encode.c/h` - Encoding logic
- `decode.c/h` - Decoding logic
- `beautiful.bmp` - Input image
- `secret.txt` - Secret message
