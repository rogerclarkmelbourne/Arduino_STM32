using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

namespace BMP24toILI565
{
    internal class Program
    {
        static private FileStream rgb24file;
        static private FileStream rgb16file;
        static private int bmpWidth, bmpHeight; // W+H in pixels
        static private int bmpDepth; // Bit depth (currently must be 24)
        static private UInt32 bmpImageoffset; // Start of image data in file
        static private int rowSize; // Not always = bmpWidth; may have padding
        static private bool goodBmp = false; // Set to true on valid header parse
        static private bool flip = true; // BMP is stored bottom-to-top
        static private UInt16 w, h, row, col;
        static private byte r, g, b;
        static private long pos = 0, startTime;
        static private bool first = true;


        private static void Main(string[] args)
        {
            string codeBase = Assembly.GetExecutingAssembly().CodeBase;
            string exeFilename = Path.GetFileName(codeBase);

            if (args.Length == 0)
            {
                var bmpFiles = Directory.EnumerateFiles(Directory.GetCurrentDirectory(), "*.bmp").ToList();
                if (bmpFiles.Count == 0)
                    Console.WriteLine("\nNo .bmp files found.");
                else
                    bmpFiles.ForEach(i => ConvertImage(i));
            }
            else
            {
                ConvertImage(args[0]);
            }
        }

        static void ConvertImage(string imageFilename)
        {
            if (File.Exists(imageFilename))
            {
                try
                {
                    Console.WriteLine("\nConverting {0}...", imageFilename);
                    rgb24file = new FileStream(imageFilename, FileMode.Open);
                    if (rgb24file == null)
                    {
                        Console.WriteLine("Could not open {0}", imageFilename);
                        return;
                    }

                    rgb24file.Seek(0, SeekOrigin.Begin);

                    if (read16(rgb24file) != 0x4D42)
                    {
                        Console.WriteLine("Not a BMP file");
                        return;
                    }

                    // BMP signature
                    Console.WriteLine("File size: {0}", read32(rgb24file));
                    read32(rgb24file); // Read & ignore creator bytes
                    bmpImageoffset = read32(rgb24file); // Start of image data
                    Console.WriteLine("Image Offset: {0}", bmpImageoffset);
                    // Read DIB header
                    Console.WriteLine("Header size: {0}", read32(rgb24file));
                    bmpWidth = (UInt16)read32(rgb24file);
                    bmpHeight = (UInt16)read32(rgb24file);
                    if (read16(rgb24file) != 1) // # planes -- must be '1'
                    {
                        Console.WriteLine("Number of planes must be 1");
                        return;
                    }

                    bmpDepth = read16(rgb24file); // bits per pixel
                    Console.WriteLine("Bit Depth: {0}", bmpDepth);
                    if (bmpDepth != 24)
                    {
                        Console.WriteLine("Image is not in 24bit");
                        return;
                    }

                    if (read32(rgb24file) != 0) // 0 = uncompressed
                    {
                        Console.WriteLine("BMP must be in uncompressed format");
                        return;
                    }

                    goodBmp = true; // Supported BMP format -- proceed!
                    Console.WriteLine("Image size: {0}x{1}", bmpWidth, bmpHeight);

                    string outFilename = Path.ChangeExtension(imageFilename, "565");

                    Console.WriteLine("{0} created", outFilename);

                    rgb16file = new FileStream(outFilename, FileMode.Create);

                    if (rgb16file == null)
                    {
                        Console.WriteLine("Could not create file {0}", outFilename);
                        return;
                    }

                    var header = new byte[54];

                    rgb24file.Seek(0, SeekOrigin.Begin);
                    rgb24file.Read(header, 0, 54);

                    header[0x1C] = 16; // updage bit depth to 16 bpp

                    rgb16file.Write(header, 0, 54);

                    // BMP rows are padded (if needed) to 4-byte boundary
                    rowSize = (bmpWidth * 3 + 3) & ~3;

                    // If bmpHeight is negative, image is in top-down order.
                    // This is not canon but has been observed in the wild.
                    if (bmpHeight < 0)
                    {
                        bmpHeight = -bmpHeight;
                        flip = false;
                    }

                    var inRGB = new byte[3];
                    var outRGB = new byte[2];

                    for (row = 0; row < bmpHeight; row++)
                    {
                        // For each scanline...

                        if (flip) // Bitmap is stored bottom-to-top order (normal BMP)
                            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
                        else // Bitmap is stored top-to-bottom
                            pos = bmpImageoffset + row * rowSize;

                        rgb24file.Seek(pos, SeekOrigin.Begin);

                        for (var c = 0; c < 3 * bmpWidth; c += 3)
                        {
                            rgb24file.Read(inRGB, 0, 3);

                            UInt16 iliColor = to565(inRGB[2], inRGB[1], inRGB[0]);
                            outRGB[0] = (byte)(iliColor >> 8);
                            outRGB[1] = (byte)(iliColor & 0xFF);
                            rgb16file.Write(outRGB, 0, 2);
                        }
                    }
                }
                catch (Exception ex)
                {
                    Console.WriteLine(ex.Message);
                    Console.WriteLine(ex.StackTrace);
                }
                finally
                {
                    if (rgb24file != null)
                    {
                        rgb24file.Close();
                        rgb24file = null;
                    }
                    if (rgb16file != null)
                    {
                        rgb16file.Close();
                        rgb16file = null;
                    }
                }
            }
            else
            {
                Console.WriteLine("File {0} does not exists");
            }
        }

        static private UInt16 to565(byte r, byte g, byte b)
        {
            return (UInt16)(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
            //return ((red >> 3) << 11) | ((green >> 2) << 5) | (blue >> 3);
        }

        static UInt16 read16(FileStream file)
        {
            var bytes = new byte[2];

            file.Read(bytes, 0, 2);
            var word = (UInt16)(bytes[1] << 8 | bytes[0]);

            return word;
        }

        static UInt32 read32(FileStream file)
        {
            var bytes = new byte[4];

            file.Read(bytes, 0, 4);
            var dword = (UInt32)(bytes[3] << 24 | bytes[2] << 16 | bytes[1] << 8 | bytes[0]);

            return dword;
        }
    }


}
