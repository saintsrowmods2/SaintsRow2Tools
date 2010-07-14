using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Text;

namespace Gibbed.SaintsRow2.FileFormats
{
    public static class DXTDecompressor
    {
        public static bool BigEndian = false;

        public static uint PackRGBA(byte r, byte g, byte b, byte a)
        {
            return (uint)((r << 24) | (g << 16) | (b << 8) | a);
        }

        public static Color PackedRGBAtoColor(uint packed)
        {
            return Color.FromArgb((int)packed);
        }

        public static Bitmap DecompressDXT1(byte[] blocks, int width, int height)
        {
            Bitmap b = new Bitmap(width, height, System.Drawing.Imaging.PixelFormat.Format32bppArgb);
            
            int blockCountX = (width+3) / 4;
            int blockCountY = (height+3) / 4;
            int blockWidth = (width < 4) ? width : 4;
            int blockHeight = (height < 4) ? height : 4;

            for (int blockY = 0; blockY < blockCountY; blockY++)
            {
                for (int blockX = 0; blockX < blockCountX; blockX++)
                {
                    DecompressDXT1Block(blocks, (uint)((blockY * blockCountX * 8) + blockX * 8), blockX * 4, blockY * 4, ref b);
                }
            }

            return b;
        }

        public static void DecompressDXT1Block(byte[] blocks, uint offset, int x, int y, ref Bitmap b)
        {
            ushort color0 = GetUShort(blocks, offset);
            ushort color1 = GetUShort(blocks, offset + 2);

            uint temp;

            temp = (uint)((color0 >> 11) * 255 + 16);
            byte r0 = (byte)((temp / 32 + temp) / 32);
            temp = (uint)(((color0 & 0x08E0) >> 5) * 255 + 32);
            byte g0 = (byte)((temp / 64 + temp) / 64);
            temp = (uint)((color0 & 0x001F) * 255 + 16);
            byte b0 = (byte)((temp / 32 + temp) / 32);

            temp = (uint)((color1 >> 11) * 255 + 16);
            byte r1 = (byte)((temp / 32 + temp) / 32);
            temp = (uint)(((color1 & 0x08E0) >> 5) * 255 + 32);
            byte g1 = (byte)((temp / 64 + temp) / 64);
            temp = (uint)((color1 & 0x001F) * 255 + 16);
            byte b1 = (byte)((temp / 32 + temp) / 32);

            uint code = GetUInt(blocks, offset + 4);

            for (int j = 0; j < 4; j++)
            {
                for (int i = 0; i < 4; i++)
                {
                    uint finalColor = 0;
                    byte positionCode = (byte)((code >> 2 * (4 * j + i)) & 0x03);

                    if (color0 > color1)
                    {
                        switch (positionCode)
                        {
                            case 0:
                                finalColor = PackRGBA(r0, g0, b0, 255);
                                break;
                            case 1:
                                finalColor = PackRGBA(r1, g1, b1, 255);
                                break;
                            case 2:
                                finalColor = PackRGBA((byte)((2 * r0 + r1) / 3), (byte)((2 * g0 + g1) / 3), (byte)((2 * b0 + b1) / 3), 255);
                                break;
                            case 3:
                                finalColor = PackRGBA((byte)((r0 + 2 * r1) / 3), (byte)((g0 + 2 * g1) / 3), (byte)((b0 + 2 * b1) / 3), 255);
                                break;
                        }
                    }
                    else
                    {
                        switch (positionCode)
                        {
                            case 0:
                                finalColor = PackRGBA(r0, g0, b0, 255);
                                break;
                            case 1:
                                finalColor = PackRGBA(r1, g1, b1, 255);
                                break;
                            case 2:
                                finalColor = PackRGBA((byte)((r0 + r1) / 2), (byte)((g0 + g1) / 2), (byte)((b0 + b1) / 2), 255);
                                break;
                            case 3:
                                finalColor = PackRGBA(0, 0, 0, 255);
                                break;
                        }
                    }

                    if (x + i < b.Width)
                    {
                        b.SetPixel(x + i, y + j, PackedRGBAtoColor(finalColor));
                    }
                }
            }
        }

        public static ushort GetUShort(byte[] buffer, uint offset)
        {
            return (ushort)(buffer[offset] + (buffer[offset+1] << 8));
            //return (ushort)(buffer[offset] + (buffer[offset+1] << 8));
        }

        public static uint GetUInt(byte[] buffer, uint offset)
        {
            return (uint)(buffer[offset] + (buffer[offset + 1] << 8) + (buffer[offset + 2] << 16) + (buffer[offset + 3] << 24));
        }
    }
}