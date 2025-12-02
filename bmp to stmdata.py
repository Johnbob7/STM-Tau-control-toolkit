import struct
import sys
from pathlib import Path

def to_rgb565(r, g, b):
    # 8-bit RGB → RGB565
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3)

def bmp_to_rgb565(bmp_path, bin_out_path, header_out_path, array_name="image_rgb565"):
    bmp_path = Path(bmp_path)
    with bmp_path.open("rb") as f:
        header = f.read(54)

        signature = header[0:2]
        if signature != b"BM":
            raise ValueError("Not a BMP file")

        file_size      = struct.unpack("<I", header[2:6])[0]
        pixel_offset   = struct.unpack("<I", header[10:14])[0]
        dib_header_sz  = struct.unpack("<I", header[14:18])[0]
        width          = struct.unpack("<i", header[18:22])[0]
        height         = struct.unpack("<i", header[22:26])[0]
        planes         = struct.unpack("<H", header[26:28])[0]
        bpp            = struct.unpack("<H", header[28:30])[0]
        compression    = struct.unpack("<I", header[30:34])[0]

        if bpp not in (24, 32):
            raise ValueError(f"Only 24/32 bpp BMP supported, got {bpp}")
        if compression != 0:
            raise ValueError("Compressed BMP not supported")

        bytes_per_pixel = bpp // 8
        row_size = ((bpp * width + 31) // 32) * 4  # 4-byte aligned rows

        f.seek(pixel_offset)
        raw = f.read(row_size * abs(height))

    w, h = width, abs(height)
    bottom_up = (height > 0)

    pixels_565 = []
    for row in range(h):
        src_row = (h - 1 - row) if bottom_up else row
        row_start = src_row * row_size
        for col in range(w):
            px_start = row_start + col * bytes_per_pixel
            b = raw[px_start + 0]
            g = raw[px_start + 1]
            r = raw[px_start + 2]
            rgb565 = to_rgb565(r, g, b)
            pixels_565.append(rgb565)

    # write raw binary
    with open(bin_out_path, "wb") as f:
        for v in pixels_565:
            f.write(struct.pack("<H", v))  # little-endian

    # write C header
    with open(header_out_path, "w") as f:
        f.write("#pragma once\n#include <stdint.h>\n\n")
        f.write(f"#define IMAGE_WIDTH  {w}\n#define IMAGE_HEIGHT {h}\n\n")
        f.write(f"static const uint16_t {array_name}[IMAGE_WIDTH * IMAGE_HEIGHT] = {{\n")
        for i, v in enumerate(pixels_565):
            if i % 12 == 0:
                f.write("    ")
            f.write(f"0x{v:04X}")
            if i + 1 != len(pixels_565):
                f.write(", ")
            if (i + 1) % 12 == 0:
                f.write("\n")
        if len(pixels_565) % 12 != 0:
            f.write("\n")
        f.write("};\n")

    print(f"Done. Width={w}, Height={h}")
    print(f"Wrote: {bin_out_path}")
    print(f"Wrote: {header_out_path}")

if __name__ == "__main__":
    bmp_to_rgb565(
        "tiger.bmp",              # input BMP
        "tiger_rgb565.bin",       # output raw RGB565
        "tiger_rgb565.h",         # output C header
        array_name="tiger_rgb565"
    )
    print("Conversion complete.")

