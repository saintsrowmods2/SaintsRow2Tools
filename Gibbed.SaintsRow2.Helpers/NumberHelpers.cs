﻿using System;

namespace Gibbed.SaintsRow2.Helpers
{
	public static class NumberHelpers
	{
		public static Int16 Swap(this Int16 value)
		{
			UInt16 uvalue = (UInt16)value;
			UInt16 swapped = (UInt16)((0x00FF) & (value >> 8) |
							          (0xFF00) & (value << 8));
			return (Int16)swapped;
		}

		public static UInt16 Swap(this UInt16 value)
		{
			UInt16 swapped = (UInt16)((0x00FF) & (value >> 8) |
							          (0xFF00) & (value << 8));
			return swapped;
		}

		public static Int32 Swap(this Int32 value)
		{
			UInt32 uvalue = (UInt32)value;
			UInt32 swapped = ((0x000000FF) & (uvalue >> 24) |
							  (0x0000FF00) & (uvalue >> 8) |
							  (0x00FF0000) & (uvalue << 8) |
							  (0xFF000000) & (uvalue << 24));
			return (Int32)swapped;
		}

		public static UInt32 Swap(this UInt32 value)
		{
			UInt32 swapped = ((0x000000FF) & (value >> 24) |
							  (0x0000FF00) & (value >> 8) |
							  (0x00FF0000) & (value << 8) |
							  (0xFF000000) & (value << 24));
			return swapped;
		}

		public static Int64 Swap(this Int64 value)
		{
			UInt64 uvalue = (UInt64)value;
			UInt64 swapped = ((0x00000000000000FF) & (uvalue >> 56) |
							  (0x000000000000FF00) & (uvalue >> 40) |
							  (0x0000000000FF0000) & (uvalue >> 24) |
							  (0x00000000FF000000) & (uvalue >> 8) |
							  (0x000000FF00000000) & (uvalue << 8) |
							  (0x0000FF0000000000) & (uvalue << 24) |
							  (0x00FF000000000000) & (uvalue << 40) |
							  (0xFF00000000000000) & (uvalue << 56));
			return (Int64)swapped;
		}

		public static UInt64 Swap(this UInt64 value)
		{
			UInt64 swapped = ((0x00000000000000FF) & (value >> 56) |
							  (0x000000000000FF00) & (value >> 40) |
							  (0x0000000000FF0000) & (value >> 24) |
							  (0x00000000FF000000) & (value >> 8) |
							  (0x000000FF00000000) & (value << 8) |
							  (0x0000FF0000000000) & (value << 24) |
							  (0x00FF000000000000) & (value << 40) |
							  (0xFF00000000000000) & (value << 56));
			return swapped;
		}

        public static UInt32 Reverse(this UInt32 value)
        {
            UInt32 reversed = 0;

            for (Int32 i = 0; i < 32; i++)
            {
                Int32 targetBit = 31 - i;
                UInt32 testMask = (UInt32)(1 << i);
                UInt32 setMask = (UInt32)(1 << targetBit);

                if ((value & testMask) != 0)
                {
                    reversed |= setMask;
                }
                else
                {
                }
            }

            return reversed;
        }

        public static int Align(this int value, int align)
		{
			if (value == 0)
			{
				return value;
			}

			int left = value % align;

			if (left > 0)
			{
				return value + (align - left);
			}

			return value;
		}

		public static uint Align(this uint value, uint align)
		{
			if (value == 0)
			{
				return value;
			}

			uint left = value % align;

			if (left > 0)
			{
				return value + (align - left);
			}

			return value;
		}

		public static long Align(this long value, long align)
		{
			if (value == 0)
			{
				return value;
			}

			long left = value % align;

			if (left > 0)
			{
				return value + (align - left);
			}

			return value;
		}
	}
}
