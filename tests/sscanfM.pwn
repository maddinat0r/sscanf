// This is a comment
// uncomment the line below if you want to write a filterscript
//#define FILTERSCRIPT

#define SSCANF_NO_NICE_FEATURES

#include <a_samp>
#include <sscanf2>

px(c)
{
	printf("%04x%04x", c >>> 16, c & 0xFFFF);
}

main()
{
	/*#if defined __PawnBuild
		printf("%s: %d", __file, __line);
	#else
		printf("%s: %d", "<unknown file>", -1);
	#endif*/
	printf("=====");
	{
		new c0, c1, c2, c3, c4, c5, c6;
		sscanf("#ABC", "m", c0);
		sscanf("{112244}", "m", c1);
		sscanf("#987654", "m", c2);
		sscanf("DEFFED", "m", c3);
		sscanf("FED55DEF", "m", c4);
		sscanf("0x4EFFE4", "m", c5);
		sscanf("0xFE4554EF", "m", c6);
		px(c0);
		px(c1);
		px(c2);
		px(c3);
		px(c4);
		px(c5);
		px(c6);
	}
	printf("=====");
	SSCANF_Option(SSCANF_ARGB, 1);
	{
		new c0, c1, c2, c3, c4, c5, c6;
		sscanf("#ABC", "m", c0);
		sscanf("{112244}", "m", c1);
		sscanf("#987654", "m", c2);
		sscanf("DEFFED", "m", c3);
		sscanf("FED55DEF", "m", c4);
		sscanf("0x4EFFE4", "m", c5);
		sscanf("0xFE4554EF", "m", c6);
		px(c0);
		px(c1);
		px(c2);
		px(c3);
		px(c4);
		px(c5);
		px(c6);
	}
	printf("=====");
	SSCANF_Option(SSCANF_ARGB, 0);
	SSCANF_Option(SSCANF_ALPHA, 0xAA);
	SSCANF_Option(SSCANF_COLOUR_FORMS, 96);
	{
		new c0, c1, c2, c3, c4, c5, c6;
		sscanf("#ABC", "m", c0);
		sscanf("{112244}", "m", c1);
		sscanf("#987654", "m", c2);
		sscanf("DEFFED", "m", c3);
		sscanf("FED55DEF", "m", c4);
		sscanf("0x4EFFE4", "m", c5);
		sscanf("0xFE4554EF", "m", c6);
		px(c0);
		px(c1);
		px(c2);
		px(c3);
		px(c4);
		px(c5);
		px(c6);
	}
	printf("=====");
	SSCANF_Option(SSCANF_COLOUR_FORMS, -1);
	{
		new c0, c1, c2, c3, c4, c5, c6;
		sscanf("#ABDC", "m", c0);
		sscanf("{11224}", "m", c1);
		sscanf("#9876541", "m", c2);
		sscanf("DEF", "m", c3);
		sscanf("FED55DE", "m", c4);
		sscanf("0x4EF", "m", c5);
		sscanf("0xE4554EF", "m", c6);
		px(c0);
		px(c1);
		px(c2);
		px(c3);
		px(c4);
		px(c5);
		px(c6);
	}
}


