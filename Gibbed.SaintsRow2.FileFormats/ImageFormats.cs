using System.Drawing;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;

namespace Gibbed.SaintsRow2.FileFormats
{
	public class ImageFormats
	{
        // See http://code.google.com/p/libsquish/

		public enum Flags
		{
			DXT1 = (1 << 0),
			DXT3 = (1 << 1),
			DXT5 = (1 << 2),
			ColourClusterFit = (1 << 3),
			ColourRangeFit = (1 << 4),
			ColourMetricPerceptual = (1 << 5),
			ColourMetricUniform = (1 << 6),
			WeightColourByAlpha = (1 << 7),
			ColourIterativeClusterFit = (1 << 8),
		}

        internal static class NativeMethods
        {
            [DllImport("squish.dll", EntryPoint = "CompressImage")]
            internal static extern void CompressImage([MarshalAs(UnmanagedType.LPArray)] byte[] rgba, uint width, uint height, [MarshalAs(UnmanagedType.LPArray)] byte[] blocks, int flags);

            [DllImport("squish.dll", EntryPoint = "DecompressImage")]
            internal static extern void DecompressImage([MarshalAs(UnmanagedType.LPArray)] byte[] rgba, uint width, uint height, [MarshalAs(UnmanagedType.LPArray)] byte[] blocks, int flags);
        }

        public static byte[] Compress(byte[] decompressed, uint width, uint height, PegFormat format, uint compressedSize)
        {
            byte[] compressed = new byte[compressedSize];
            
            Flags flags = 0;
            if (format == PegFormat.DXT1)
                flags |= Flags.DXT1;
            else if (format == PegFormat.DXT3)
                flags |= Flags.DXT3;
            else if (format == PegFormat.DXT5)
                flags |= Flags.DXT5;

            flags |= Flags.ColourMetricUniform | Flags.ColourIterativeClusterFit;

            NativeMethods.CompressImage(decompressed, width, height, compressed, (int)flags);

            return compressed;
        }

        public static byte[] Decompress(byte[] compressed, uint width, uint height, PegFormat format)
        {
            byte[] decompressed = new byte[width * height * 4];
            Flags flags = 0;
            if (format == PegFormat.DXT1)
                flags |= Flags.DXT1;
            else if (format == PegFormat.DXT3)
                flags |= Flags.DXT3;
            else if (format == PegFormat.DXT5)
                flags |= Flags.DXT5;

            NativeMethods.DecompressImage(decompressed, width, height, compressed, (int)flags);

            return decompressed;
        }

        public static void SwapRedAndBlue(uint width, uint height, ref byte[] buffer)
        {
            for (uint i = 0; i < width * height * 4; i += 4)
            {
                // flip red and blue
                byte r = buffer[i + 0];
                buffer[i + 0] = buffer[i + 2];
                buffer[i + 2] = r;
            }
        }

        public static Bitmap MakeBitmapFromDXT(uint width, uint height, byte[] buffer, bool keepAlpha)
        {
            Bitmap bitmap = new Bitmap((int)width, (int)height, PixelFormat.Format32bppArgb);

            SwapRedAndBlue(width, height, ref buffer);

            Rectangle area = new Rectangle(0, 0, bitmap.Width, bitmap.Height);
            BitmapData data = bitmap.LockBits(area, ImageLockMode.WriteOnly, bitmap.PixelFormat);
            Marshal.Copy(buffer, 0, data.Scan0, (int)(width * height * 4));
            bitmap.UnlockBits(data);

            /*if (!keepAlpha)
            {
                for (int x = 0; x < bitmap.Width; x++)
                {
                    for (int y = 0; y < bitmap.Height; y++)
                    {
                        Color c = bitmap.GetPixel(x, y);
                        //if (c.B == 0 && c.G == 0 && c.R == 0)
                        //{
                            Color newColor = Color.FromArgb(127, c);
                            bitmap.SetPixel(x, y, newColor);
                        //}
                    }
                }
            }*/
            
            return bitmap;
        }

        public static Bitmap MakeBitmapFromR5G6B5(uint width, uint height, byte[] buffer)
        {
            Bitmap bitmap = new Bitmap((int)width, (int)height, PixelFormat.Format16bppRgb565);
            Rectangle area = new Rectangle(0, 0, bitmap.Width, bitmap.Height);
            BitmapData data = bitmap.LockBits(area, ImageLockMode.WriteOnly, bitmap.PixelFormat);
            Marshal.Copy(buffer, 0, data.Scan0, (int)(width * height * 2));
            bitmap.UnlockBits(data);
            return bitmap;
        }

        public static Bitmap MakeBitmapFromA8R8G8B8(uint width, uint height, byte[] buffer)
        {
            Bitmap bitmap = new Bitmap((int)width, (int)height, PixelFormat.Format32bppArgb);
            Rectangle area = new Rectangle(0, 0, bitmap.Width, bitmap.Height);
            BitmapData data = bitmap.LockBits(area, ImageLockMode.WriteOnly, bitmap.PixelFormat);
            Marshal.Copy(buffer, 0, data.Scan0, (int)(width * height * 4));
            bitmap.UnlockBits(data);
            return bitmap;
        }
	}
}
