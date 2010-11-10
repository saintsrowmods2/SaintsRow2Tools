using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using Gibbed.SaintsRow2.Helpers;

namespace Gibbed.SaintsRow2.FileFormats
{
	public class LeStringsFile
	{
		public Dictionary<UInt32, string> Strings = new Dictionary<UInt32, string>();

		public void Read(Stream stream)
		{
			if (stream.ReadU32() != 0x0A84C7F73)
			{
				throw new Exception();
			}

			if (stream.ReadU16() != 1)
			{
				throw new Exception();
			}

			stream.Seek(0, SeekOrigin.Begin);
			byte[] data = new byte[(int)stream.Length];
			stream.Read(data, 0, (int)stream.Length);

			this.Strings.Clear();

			int indexSize = BitConverter.ToInt16(data, 6);
            Console.WriteLine("indexSize: {0}", indexSize);
			for (int i = 0; i < indexSize; i++)
			{
				int blockCount = BitConverter.ToInt32(data, 12 + (i * 8) + 0);
				int blockOffset = BitConverter.ToInt32(data, 12 + (i * 8) + 4);
                Console.WriteLine("\ti: {0:X2}, blockCount: {1}, blockOffset: {2}", i, blockCount, blockOffset);

				for (int j = 0; j < blockCount; j++)
				{
					int stringOffset = BitConverter.ToInt32(data, blockOffset + (j * 4));

					UInt32 hash = BitConverter.ToUInt32(data, stringOffset).Swap();
                    Console.Write("\t\tj: {0:X2}, stringOffset: {1}, hash: 0x{2:X8}, i & hash: 0x{3:X8} ", j, stringOffset, hash, i & hash);

					int k = 0;
					while (true)
					{
						ushort word = BitConverter.ToUInt16(data, stringOffset + 4 + (k * 2)).Swap();
						
						if (word == 0)
						{
							break;
						}

						k++;
					}

					byte[] stringData = new byte[k * 2];
					Array.Copy(data, stringOffset + 4, stringData, 0, k * 2);

					for (k = 0; k < stringData.Length; k += 2)
					{
						Array.Copy(BitConverter.GetBytes(BitConverter.ToUInt16(stringData, k).Swap()), 0, stringData, k, 2);
					}

					string text = Encoding.Unicode.GetString(stringData);
                    Console.WriteLine("stringData len: {0}", stringData.Length);
                    //Console.WriteLine(text);
					if (this.Strings.ContainsKey(hash))
					{
						throw new InvalidOperationException();
					}

					this.Strings[hash] = text;
				}
			}
		}
	}
}
