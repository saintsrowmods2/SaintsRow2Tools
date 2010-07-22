using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Xml;

using Gibbed.SaintsRow2.FileFormats;
using Gibbed.SaintsRow2.Helpers;

namespace PegTool
{
    public static class Program
    {
        public static void Main(string[] args)
        {
            if (args.Length == 2)
            {
                if (args[0] == "unpack")
                {
                    Unpack(args[1]);
                }
                else if (args[0] == "repack")
                {
                    Repack(args[1]);
                }
                else
                    Usage();
            }
            else
                Usage();

#if DEBUG
            Console.ReadLine();
#endif
        }

        public static void Usage()
        {
            Console.WriteLine("PegTool");
            Console.WriteLine("Saints Row 2 texture manipulation tool");
            Console.WriteLine();
            Console.WriteLine("Available actions:");
            Console.WriteLine("\tunpack <peg_pc file>");
            Console.WriteLine("\trepack <peg_desc file>");
        }

        public static void Unpack(string pegFilePath)
        {
            Console.WriteLine("Unpacking {0}:", pegFilePath);
            if (!File.Exists(pegFilePath))
            {
                Console.WriteLine("peg_pc file does not exist: {0}", pegFilePath);
                return;
            }
            string extension = Path.GetExtension(pegFilePath);
            bool xbox = (extension == ".peg_xbox2");
            if (!(extension == ".peg_pc" || extension == ".peg_xbox2"))
            {
                Console.WriteLine("File is not a peg_pc file: {0}", pegFilePath);
                return;
            }

            string pegDataFilePath = xbox ? Path.ChangeExtension(pegFilePath, "g_peg_xbox2") : Path.ChangeExtension(pegFilePath, "g_peg_pc");
            string pegDescFilePath = Path.ChangeExtension(pegFilePath, "peg_desc");

            if (!File.Exists(pegDataFilePath))
            {
                Console.WriteLine("g_peg_pc file does not exist: {0}", pegDataFilePath);
                Console.WriteLine("Did you copy it into the same directory as {0}?", pegFilePath);
                return;
            }

            PegFile pegFile = new PegFile(xbox);
            FileStream pegFileStream = new FileStream(pegFilePath, FileMode.Open, FileAccess.Read, FileShare.Read);
            FileStream pegDataFileStream = new FileStream(pegDataFilePath, FileMode.Open, FileAccess.Read, FileShare.Read);
            pegFile.Read(pegFileStream);

            string extractionPath = Path.GetDirectoryName(pegFilePath);
            Console.Write("Writing peg description to: {0}", pegDescFilePath);
            Console.WriteLine(" done.");
            WritePegDescription(pegFile, pegDescFilePath);
            Console.WriteLine("Unpacking images:");
            UnpackImages(pegFile, extractionPath, pegDataFileStream);
            pegFileStream.Close();
            pegDataFileStream.Close();
            Console.WriteLine();
            Console.WriteLine("Finished unpacking.");
        }

        public static void WritePegDescription(PegFile pegFile, string pegDescFilePath)
        {
            XmlTextWriter writer = new XmlTextWriter(pegDescFilePath, Encoding.UTF8);
            writer.Formatting = Formatting.Indented;
            writer.IndentChar = '\t';
            writer.Indentation = 1;
            
            writer.WriteStartDocument();
            writer.WriteStartElement("PegDescription");

            // Header data
            writer.WriteElementString("BigEndian", pegFile.BigEndian.ToString());
            writer.WriteElementString("Magic", "GEKV");
            writer.WriteElementString("Version", "10");
            writer.WriteElementString("Unknown06", pegFile.Unknown06.ToString());
            writer.WriteElementString("FileSize", pegFile.FileSize.ToString());
            writer.WriteElementString("DataFileSize", pegFile.DataFileSize.ToString());
            writer.WriteElementString("EntryCount", pegFile.Entries.Count.ToString());
            writer.WriteElementString("Unknown12", pegFile.Unknown12.ToString());
            int frameCount = 0;
            foreach (PegEntry entry in pegFile.Entries)
                frameCount += entry.Frames.Count;
            writer.WriteElementString("FrameCount", frameCount.ToString());
            writer.WriteElementString("Unknown16", pegFile.Unknown16.ToString());

            writer.WriteStartElement("Entries");

            foreach (PegEntry entry in pegFile.Entries)
            {
                writer.WriteStartElement("Entry");
                writer.WriteElementString("Name", entry.Name);
                writer.WriteStartElement("Frames");
                foreach (PegFrame frame in entry.Frames)
                {
                    writer.WriteStartElement("Frame");
                    writer.WriteElementString("Offset", frame.Offset.ToString());
                    writer.WriteElementString("Width", frame.Width.ToString());
                    writer.WriteElementString("Height", frame.Height.ToString());
                    PegFormat frameFormat = (PegFormat)frame.Format;
                    writer.WriteElementString("Format", frameFormat.ToString());
                    writer.WriteElementString("Unknown0A", frame.Unknown0A.ToString());
                    writer.WriteElementString("Unknown0C", frame.Unknown0C.ToString());
                    writer.WriteElementString("Frames", frame.Frames.ToString());
                    writer.WriteElementString("Unknown12", frame.Unknown12.ToString());
                    writer.WriteElementString("Unknown16", frame.Unknown16.ToString());
                    writer.WriteElementString("UnknownFlags1A", frame.UnknownFlags1A.ToString("X4"));
                    writer.WriteElementString("Size", frame.Size.ToString());
                    writer.WriteElementString("Unknown20", frame.Unknown20.ToString());
                    writer.WriteElementString("Unknown24", frame.Unknown24.ToString());
                    writer.WriteElementString("Unknown28", frame.Unknown28.ToString());
                    writer.WriteElementString("Unknown2C", frame.Unknown2C.ToString());
                    writer.WriteEndElement();
                }
                writer.WriteEndElement();
                writer.WriteEndElement();
            }

            writer.WriteEndElement();
            writer.WriteEndElement();
            writer.Close();
        }

        public static void UnpackImages(PegFile pegFile, string path, Stream pegDataFileStream)
        {
            foreach (PegEntry entry in pegFile.Entries)
            {
                Console.WriteLine("Extracting {0} ({1} frame{2}):", entry.Name, entry.Frames.Count, (entry.Frames.Count == 1 ? "" : "s"));
                for (int i = 0; i < entry.Frames.Count; i++)
                {
                    string filePath = Path.Combine(path, String.Format("{0}_{1}.png", entry.Name, i));
                    string rawFilePath = Path.Combine(path, String.Format("{0}_{1}.raw", entry.Name, i));
                    Console.Write(" - Extracting {0}... ", filePath);

                    PegFrame frame = entry.Frames[i];
                    pegDataFileStream.Seek(frame.Offset, SeekOrigin.Begin);
                    byte[] rawData = new byte[frame.Size];
                    pegDataFileStream.Read(rawData, 0, (int)frame.Size);

                    PegFormat format = (PegFormat)frame.Format;

                    FileStream rawFileStream = new FileStream(rawFilePath, FileMode.Create, FileAccess.Write, FileShare.None);
                    rawFileStream.Write(rawData, 0, rawData.Length);
                    rawFileStream.Flush();
                    rawFileStream.Close();

                    Bitmap bitmap = null;

                    switch (format)
                    {
                        case PegFormat.A8R8G8B8:
                            bitmap = ImageFormats.MakeBitmapFromA8R8G8B8(frame.Width, frame.Height, rawData);
                            break;
                        case PegFormat.R5G6B5:
                            bitmap = ImageFormats.MakeBitmapFromR5G6B5(frame.Width, frame.Height, rawData);
                            break;
                        case PegFormat.DXT1:
                            byte[] decompressedDXT1 = ImageFormats.Decompress(rawData, frame.Width, frame.Height, format);
                            bitmap = ImageFormats.MakeBitmapFromDXT(frame.Width, frame.Height, decompressedDXT1, false);
                            break;
                        case PegFormat.DXT3:
                        case PegFormat.DXT5:
                            byte[] decompressed = ImageFormats.Decompress(rawData, frame.Width, frame.Height, format);
                            bitmap = ImageFormats.MakeBitmapFromDXT(frame.Width, frame.Height, decompressed, true);
                            break;
                        default:
                            throw new Exception("Unhandled format: " + format.ToString());
                    }

                    bitmap.Save(filePath, ImageFormat.Png);
                    bitmap.Dispose();

                    Console.WriteLine("done.");
                }
            }
        }

        public static void Repack(string descFilePath)
        {
            Console.WriteLine("Repacking {0}:", descFilePath);
            if (!File.Exists(descFilePath))
            {
                Console.WriteLine("peg_desc file does not exist: {0}", descFilePath);
                return;
            }

            string extension = Path.GetExtension(descFilePath);
            if (!(extension == ".peg_desc"))
            {
                Console.WriteLine("File is not a peg_desc file: {0}", descFilePath);
                return;
            }

            string descFolder = Path.GetDirectoryName(descFilePath);
            Console.Write("Loading Peg Description... ");
            PegFile pegFile = XmlParser.ParseFile(descFilePath);
            if (pegFile == null)
            {
                Console.WriteLine("An error occurred while loading the peg_desc file.");
                return;
            }
            Console.WriteLine("done.");

            Console.WriteLine("Loading texture data, performing colour space conversions and compressing data:");
            Dictionary<PegFrame, byte[]> rawFrameData = new Dictionary<PegFrame,byte[]>();

            uint totalSize = 0;
            ushort totalFrames = 0;
            foreach (PegEntry entry in pegFile.Entries)
            {
                for (int i = 0; i < entry.Frames.Count; i++)
                {
                    PegFrame frame = entry.Frames[i];
                    Console.Write(" - {0} (frame {1})... ", entry.Name, i);
                    frame.Frames = (ushort)((i == 0) ? entry.Frames.Count : 1);
                    frame.Offset = totalSize;
                    PegFormat format = (PegFormat)frame.Format;
                    byte[] rawData = null;

                    string rawFilePath = Path.Combine(descFolder, String.Format("{0}_{1}.raw", entry.Name, i));
                    string filePath = Path.Combine(descFolder, String.Format("{0}_{1}.png", entry.Name, i));

                    if (File.Exists(rawFilePath))
                    {
                        Console.Write("loading from {0}... ", rawFilePath);
                        FileStream rawFileStream = new FileStream(rawFilePath, FileMode.Open, FileAccess.Read, FileShare.None);
                        rawData = new byte[rawFileStream.Length];
                        rawFileStream.Read(rawData, 0, rawData.Length);
                        rawFileStream.Close();
                        Console.WriteLine("done.");
                    }
                    else
                    {
                        Console.Write("importing from {0}... ", filePath);
                        if (!File.Exists(filePath))
                        {
                            Console.WriteLine("Unable to find file: {0}", filePath);
                            Console.WriteLine("Make sure all your texture files are in the correct location and correctly named.");
                            return;
                        }

                        Bitmap srcBitmap = new Bitmap(filePath);
                        Rectangle lockArea = new Rectangle(0, 0, srcBitmap.Width, srcBitmap.Height);
                        Bitmap outBitmap = null;
                        BitmapData bitmapData = null;
                        Graphics g = null;
                        switch (format)
                        {
                            case PegFormat.DXT1:
                            /*outBitmap = new Bitmap(srcBitmap.Width, srcBitmap.Height, PixelFormat.Format32bppArgb);
                            g = Graphics.FromImage(outBitmap);
                            g.DrawImage(srcBitmap, 0, 0);
                            g.Dispose();
                            bitmapData = outBitmap.LockBits(lockArea, ImageLockMode.ReadOnly, outBitmap.PixelFormat);
                            rawData = new byte[srcBitmap.Width * srcBitmap.Height * 4];
                            Marshal.Copy(bitmapData.Scan0, rawData, 0, rawData.Length);
                            outBitmap.UnlockBits(bitmapData);
                            ImageFormats.SwapRedAndBlue((uint)srcBitmap.Width, (uint)srcBitmap.Height, ref rawData);
                            rawData = ImageFormats.Compress(rawData, (uint)srcBitmap.Width, (uint)srcBitmap.Height, format, frame.Size);
                            break;*/

                            case PegFormat.DXT3:
                            case PegFormat.DXT5:
                                outBitmap = new Bitmap(srcBitmap.Width, srcBitmap.Height, PixelFormat.Format32bppArgb);
                                g = Graphics.FromImage(outBitmap);
                                g.CompositingMode = System.Drawing.Drawing2D.CompositingMode.SourceCopy;
                                g.CompositingQuality = System.Drawing.Drawing2D.CompositingQuality.HighQuality;
                                g.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.None;
                                g.DrawImageUnscaled(srcBitmap, 0, 0);
                                g.Dispose();
                                bitmapData = outBitmap.LockBits(lockArea, ImageLockMode.ReadOnly, outBitmap.PixelFormat);
                                rawData = new byte[srcBitmap.Width * srcBitmap.Height * 4];
                                Marshal.Copy(bitmapData.Scan0, rawData, 0, rawData.Length);
                                outBitmap.UnlockBits(bitmapData);
                                ImageFormats.SwapRedAndBlue((uint)srcBitmap.Width, (uint)srcBitmap.Height, ref rawData);
                                rawData = ImageFormats.Compress(rawData, (uint)srcBitmap.Width, (uint)srcBitmap.Height, format, frame.Size);
                                break;

                            case PegFormat.A8R8G8B8:
                                /*outBitmap = new Bitmap(srcBitmap.Width, srcBitmap.Height, PixelFormat.Format32bppArgb);
                                g = Graphics.FromImage(outBitmap);
                                g.CompositingMode = System.Drawing.Drawing2D.CompositingMode.SourceCopy;
                                g.CompositingQuality = System.Drawing.Drawing2D.CompositingQuality.GammaCorrected;
                                g.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.None;
                                g.DrawImageUnscaled(srcBitmap, 0, 0);
                                g.Dispose();*/
                                Console.WriteLine(srcBitmap.PixelFormat);
                                bitmapData = srcBitmap.LockBits(lockArea, ImageLockMode.ReadOnly, srcBitmap.PixelFormat);
                                rawData = new byte[srcBitmap.Width * srcBitmap.Height * 4];
                                Marshal.Copy(bitmapData.Scan0, rawData, 0, rawData.Length);
                                srcBitmap.UnlockBits(bitmapData);
                                break;
                            case PegFormat.R5G6B5:
                                outBitmap = new Bitmap(srcBitmap.Width, srcBitmap.Height, PixelFormat.Format16bppRgb565);
                                g = Graphics.FromImage(outBitmap);
                                g.CompositingMode = System.Drawing.Drawing2D.CompositingMode.SourceCopy;
                                g.CompositingQuality = System.Drawing.Drawing2D.CompositingQuality.HighQuality;
                                g.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.None;
                                g.DrawImageUnscaled(srcBitmap, 0, 0);
                                g.Dispose();
                                bitmapData = outBitmap.LockBits(lockArea, ImageLockMode.ReadOnly, outBitmap.PixelFormat);
                                rawData = new byte[srcBitmap.Width * srcBitmap.Height * 2];
                                Marshal.Copy(bitmapData.Scan0, rawData, 0, rawData.Length);
                                outBitmap.UnlockBits(bitmapData);
                                break;
                            default:
                                throw new Exception("Unhandled format: " + format.ToString());
                        }

                        if (outBitmap != null)
                            outBitmap.Dispose();
                        srcBitmap.Dispose();
                        Console.WriteLine("done.");
                    }
                    frame.Size = (uint)rawData.Length;
                    totalSize += frame.Size;
                    if (totalSize % 16 != 0)
                    {
                        uint difference = (uint)(Math.Ceiling((float)totalSize / 16f) * 16) - totalSize;
                        totalSize += difference;
                    }
                    totalFrames ++;
                    entry.Frames[i] = frame;
                    rawFrameData.Add(frame, rawData);
                }
            }
            pegFile.DataFileSize = totalSize;
            Console.WriteLine("Finished loading texture data.");

            Console.Write("Writing peg_pc and g_peg_pc files...");
            string pegFilePath = Path.ChangeExtension(descFilePath, "peg_pc");
            string pegDataFilePath = Path.ChangeExtension(descFilePath, "g_peg_pc");

            FileStream pegFileStream = new FileStream(pegFilePath, FileMode.Create, FileAccess.Write, FileShare.None);
            FileStream pegDataFileStream = new FileStream(pegDataFilePath, FileMode.Create, FileAccess.Write, FileShare.None);

            pegFileStream.WriteU32(0x564B4547); // GEKV
            pegFileStream.WriteU16(10); // Version
            pegFileStream.WriteU16(pegFile.Unknown06);
            pegFileStream.WriteU32(pegFile.FileSize); // Length - we come back and fill this in later
            pegFileStream.WriteU32(pegFile.DataFileSize);
            pegFileStream.WriteU16((ushort)pegFile.Entries.Count);
            pegFileStream.WriteU16(pegFile.Unknown12);
            pegFileStream.WriteU16(totalFrames);
            pegFileStream.WriteU16(pegFile.Unknown16);

            // Write entries
            pegFileStream.Seek(0x18, SeekOrigin.Begin);
            foreach (PegEntry entry in pegFile.Entries)
            {
                for (int i = 0; i < entry.Frames.Count; i++)
                {
                    PegFrame frame = entry.Frames[i];
                    byte[] frameData = StructureToByteArray(frame);
                    pegFileStream.Write(frameData, 0, frameData.Length);
                }
            }

            // Write names
            pegFileStream.Seek(0x18 + (0x30 * totalFrames), SeekOrigin.Begin);
            foreach (PegEntry entry in pegFile.Entries)
            {
                pegFileStream.WriteASCIIZ(entry.Name);
            }
            pegFileStream.Seek(0x08, SeekOrigin.Begin);
            pegFileStream.WriteU32((uint)pegFileStream.Length);
            pegFileStream.Flush();
            pegFileStream.Close();

            pegDataFileStream.SetLength(pegFile.DataFileSize);

            foreach (PegEntry entry in pegFile.Entries)
            {
                foreach (PegFrame frame in entry.Frames)
                {
                    pegDataFileStream.Seek(frame.Offset, SeekOrigin.Begin);
                    pegDataFileStream.Write(rawFrameData[frame], 0, (int)frame.Size);
                }
            }
            pegDataFileStream.Flush();
            pegDataFileStream.Close();

            Console.WriteLine("done.");

            Console.WriteLine();
            Console.WriteLine("Finished repacking.");
        }

        static byte[] StructureToByteArray(object obj)
        {
            int len = Marshal.SizeOf(obj);
            byte[] arr = new byte[len];
            IntPtr ptr = Marshal.AllocHGlobal(len);
            Marshal.StructureToPtr(obj, ptr, true);
            Marshal.Copy(ptr, arr, 0, len);
            Marshal.FreeHGlobal(ptr);
            return arr;
        }
    }
}
