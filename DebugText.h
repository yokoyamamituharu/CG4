#pragma once
#include "Sprite.h"
#include <string>

class DebugText
{
public:
	static const int maxCharCount = 256;
	static const int fontWidth = 9;
	static const int fontHeight = 18;
	static const int fontLineCount = 14;

	//スプライトの配列
	Sprite *sprites[maxCharCount];

	//スプライト配列の添え字番号
	int spriteIndex = 0;

public:
	void Initialize(ID3D12Device* dev, int window_width, int window_height, UINT texnumber);

	void Print(const std::string& text, float x, float y, float scale = 1.0f);

	void DrawAll();
};

