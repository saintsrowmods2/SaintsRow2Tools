using System;
using System.Text;

namespace Gibbed.SaintsRow2.Helpers
{
	public static class StringHelpers
	{
		public static uint CRC32(this string input)
		{
			return BitConverter.ToUInt32(new CRC32().ComputeHash(Encoding.ASCII.GetBytes(input)), 0);
		}

		public static uint KeyCRC32(this string input)
		{
			return BitConverter.ToUInt32(new BrokenCRC32().ComputeHash(Encoding.ASCII.GetBytes(input.ToLower())), 0);
		}

        // From Volition (SR2, RFG)
        public static UInt32 CrcVolition(this string input)
        {
            input = input.ToLowerInvariant();
            UInt32 hash = 0;
            for (int i = 0; i < input.Length; i++)
            {
                hash = Tables.CrcVolition[(byte)hash ^ (byte)input[i]] ^ (hash >> 8);
            }
            return hash;
        }

        public static UInt32 HashVolition(this string input)
        {
            UInt32 hash = 0;
            for (int i = 0; i < input.Length; i++)
            {
                UInt32 c = (char)(input[i]);
                if ((c - 0x41) <= 0x19)
                {
                    c += 0x20;
                }

                // rotate left by 6
                hash = (hash << 6) | (hash >> (32 - 6));
                hash = c ^ hash;
            }
            return hash;
        }

        public static UInt32 ParseHex32(this string input)
        {
            if (input.StartsWith("0x"))
            {
                return UInt32.Parse(input.Substring(2), System.Globalization.NumberStyles.AllowHexSpecifier);
            }

            return UInt32.Parse(input);
        }

        public static UInt64 ParseHex64(this string input)
        {
            if (input.StartsWith("0x"))
            {
                return UInt64.Parse(input.Substring(2), System.Globalization.NumberStyles.AllowHexSpecifier);
            }

            return UInt64.Parse(input);
        }

		public static uint GetHexNumber(this string input)
		{
			if (input.StartsWith("0x"))
			{
				return uint.Parse(input.Substring(2), System.Globalization.NumberStyles.AllowHexSpecifier);
			}

			return uint.Parse(input);
		}
	}
}
