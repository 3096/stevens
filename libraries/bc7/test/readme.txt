BC7Ltest-152-152-RGBA.avpcl is a compressed file that exercises all possible decode paths and
should decompress to the file BC7Ltest.tga.

colors.tga is a sample image file that compresses to colors-128-128-RGB.avpcl and decompresses
to colors-avpcl.tga.

Note that "RGBA" or "RGB" in the .avpcl file name refers to whether the input image had a
constant 255 alpha channel (RGB) or it didn't (RGBA.) Both RGB and RGBA images can generate
decompressed images with an alpha channel of other than 255 (if that would result in a smaller
squared error.)