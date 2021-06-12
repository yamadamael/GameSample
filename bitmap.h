#ifndef __BITMAP_H_INCLUDED__
#define __BITMAP_H_INCLUDED__

#define FILEHEADERSIZE 14
#define INFOHEADERSIZE 40
#define HEADERSIZE (FILEHEADERSIZE + INFOHEADERSIZE)

class bitmap
{
	typedef struct
	{
		unsigned char b;
		unsigned char g;
		unsigned char r;
	} Rgb;

	typedef struct
	{
		unsigned int height;
		unsigned int width;
		Rgb *data;
	} Image;

	Image *img;

public:
	// コンストラクタ
	bitmap();

	// 取得に成功すればポインタを、失敗すればNullを返す
	Image *Read_Bmp(const char *fileName);

	// 書き込みに成功すれば0を、失敗すれば1を返す
	int Write_Bmp(char *fileName);

	// 描画
	int Draw_Bmp(HDC hdc, int x, int y);

	// Imageを作成し、RGB情報もwidth*height分だけ動的に取得する
	// 成功すればポインタを、失敗すればNullを返す
	Image *Create_Image(int width, int height);

	Image *Get_Image();

	// Imageを解放する
	void Free_Image();
};

#endif /*__BITMAP_H_INCLUDED__*/
