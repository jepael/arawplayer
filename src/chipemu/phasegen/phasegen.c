
unsigned int calcphaseinc(unsigned int fnum, unsigned int block, unsigned int multi)
{
	unsigned int pginc;

	/*
		Per-channel base increment is FNUM * 2^(BLOCK-1).

		If block is 0, then LSB of FNUM is lost.
		If block is 1, then FNUM is used as is.

		End result fits into 16 bits, but intermediate result needs 17 bits.
	*/

	pginc = (fnum << block) >> 1;

	/*
		Per-operator multiplier applied.

		If multiplier is 0, then factor is 0.5, so one bit is shifted out.
		Multipliers 1 to 15 are as is with three exceptions :
		 - 11 does not exist, it is same as 10.
		 - 13 does not exist, it is same as 12.
		 - 14 does not exist, it is same as 15.
	*/

	switch (multi)
	{
		case 0:
			return (pginc >> 1);
			break;

		case 11:
			multi = 10;
			break;

		case 13:
			multi = 12;
			break;

		case 14:
			multi = 15;
			break;

		default:
			break;
	}

	return (pginc * multi);
}
