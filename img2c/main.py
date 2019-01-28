#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
from PIL import Image

MODE_RGB888 = False
MODE_RGB565 = True

def conv24to16bit_hex(R8, G8, B8):
    R5 = R8 >> 3
    G6 = G8 >> 2
    B5 = B8 >> 3
    col = (R5 << 11) + (G6 << 5) + B5
    return hex(col)

if __name__ == "__main__":

    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} [image name] [sprite set name]")
        sys.exit(-1)

    sprites = {}
    with open(sys.argv[2], 'r') as sprite_def:
        for sprite in sprite_def:
            if not sprite.startswith('#'):
                data = sprite.split(',')
                sprites[data[0]] = {'x': int(data[1]), 'y': int(data[2]), 'width': int(data[3]), 'height': int(data[4])}

    col_count = 16
    im = Image.open(sys.argv[1])
    im_quant = im.quantize(col_count, kmeans=3)
    im_quant.save('test.png')

    with open('palette.h', 'w') as palette_out:
        im_quant.getpalette()
        colors = []
        for col in [im_quant.getpalette()[n:n+3] for n in range(0, len(im_quant.getpalette()), 3)]:
            # print(f'{col[0]} {col[1]} {col[2]} -> {conv24to16bit_hex(col[0], col[1], col[2])}')
            colors.append(conv24to16bit_hex(col[0], col[1], col[2]))

        palette_out.write(f'static const uint16_t palette_len = {col_count};\n')
        palette_out.write('uint16_t palette[] = {\n')
        for n in range(col_count):
            palette_out.write(f"\t{colors[n]},\n")
        palette_out.write("};")

    pix = im_quant.load()
    for k, v in sprites.items():
        with open(f"{k}.h", "w+") as header_out:
            header_out.write(f"static const uint16_t {k.upper()}_WIDTH = {v['width']};\n")
            header_out.write(f"static const uint16_t {k.upper()}_HEIGHT = {v['height']};\n")
            header_out.write(f"static const char {k.upper()}_DATA[] PROGMEM = {{ ")
            for y in range(0, v['height']):
                for x in range(0, v['width']):
                    header_out.write(f"{pix[v['x']+x, v['y']+y]},")
            header_out.write("};\n")
