using System;
using System.Collections.Generic;
using System.IO;
using Gibbed.SaintsRow2.Helpers;

namespace Gibbed.SaintsRow2.FileFormats
{
	public class PegFile
	{
		public List<PegEntry> Entries = new List<PegEntry>();

		public UInt32 FileSize;
		public UInt32 DataFileSize;
        public UInt16 Unknown06;
		public UInt16 Unknown12;
		public UInt16 Unknown16;

        public bool BigEndian = false;

        public PegFile(bool bigEndian)
        {
            BigEndian = bigEndian;
        }

		public PegFrame ReadFrame(Stream stream)
		{
			byte[] data = new byte[0x30];
			stream.Read(data, 0, 0x30);
            PegFrame frame = (PegFrame)data.BytesToStructure(typeof(PegFrame));
            if (BigEndian)
            {
                frame.Offset = frame.Offset.Swap();
                frame.Width = frame.Width.Swap();
                frame.Width = 512;
                frame.Height = frame.Height.Swap();
                frame.Format = frame.Format.Swap();
                frame.Unknown0A = frame.Unknown0A.Swap();
                //frame.Format = (ushort)frame.Format;
                frame.Unknown0C = frame.Unknown0C.Swap();
                frame.Frames = frame.Frames.Swap();
                frame.Unknown12 = frame.Unknown12.Swap();
                frame.Unknown16 = frame.Unknown16.Swap();
                frame.UnknownFlags1A = frame.UnknownFlags1A.Swap();
                frame.Size = frame.Size.Swap();
                frame.Unknown20 = frame.Unknown20.Swap();
                frame.Unknown24 = frame.Unknown24.Swap();
                frame.Unknown28 = frame.Unknown28.Swap();
                frame.Unknown2C = frame.Unknown2C.Swap();
            }
            return frame;
		}

		public void Read(Stream stream)
		{
			this.Entries.Clear();

            uint magic = BigEndian ? stream.ReadU32BE() : stream.ReadU32(); // 00
			if (magic != 0x564B4547)
			{
				throw new NotAPegFileException("header must be GEKV");
			}

            ushort version = BigEndian ? stream.ReadU16BE() : stream.ReadU16(); // 04
			if (version != 10)
			{
				throw new UnsupportedPackageFileVersionException("only version 10 is supported");
			}

            Unknown06 =  BigEndian ? stream.ReadU16BE() : stream.ReadU16(); // 06
			this.FileSize = BigEndian ? stream.ReadU32BE() : stream.ReadU32(); // 08
			if (stream.Length != this.FileSize)
			{
				throw new PegFileException("size of file does not match size in header");
			}

            this.DataFileSize = BigEndian ? stream.ReadU32BE() : stream.ReadU32(); // 0C
			ushort entryCount = BigEndian ? stream.ReadU16BE() : stream.ReadU16(); // 10
            this.Unknown12 = BigEndian ? stream.ReadU16BE() : stream.ReadU16(); // 12
            ushort frameCount = BigEndian ? stream.ReadU16BE() : stream.ReadU16(); ; // 14
            this.Unknown16 = BigEndian ? stream.ReadU16BE() : stream.ReadU16(); ; // 16

			// Read names
			string[] names = new string[entryCount];
			stream.Seek(0x18 + (0x30 * frameCount), SeekOrigin.Begin);
			for (int i = 0; i < entryCount; i++)
			{
				names[i] = stream.ReadASCIIZ();
			}

			int totalFrames = 0;

			// Read entries
			stream.Seek(0x18, SeekOrigin.Begin);
			for (int i = 0; i < entryCount; i++)
			{
				PegEntry entry = new PegEntry();
				entry.Name = names[i];

				PegFrame frame = this.ReadFrame(stream);
				entry.Frames.Add(frame);
				totalFrames++;

				if (frame.Frames == 0)
				{
					throw new Exception("frame count is 0");
				}
				else if (frame.Frames > 1)
				{
					/* The first frame of a peg will say how many frames are in the
					 * image (including itself), all subsequent frames have 1 for the
					 * frame count.
					 */
					for (int j = 1; j < frame.Frames; j++)
					{
						entry.Frames.Add(this.ReadFrame(stream));
						totalFrames++;
					}
				}

				this.Entries.Add(entry);
			}

			if (totalFrames != frameCount)
			{
				throw new Exception("something bad happened");
			}
		}
	}
}
