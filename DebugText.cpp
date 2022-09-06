#include "DebugText.h"

void DebugText::Initialize(ID3D12Device* dev, int window_width, int window_height, UINT texnumber)
{
	//すべてのスプライトのデータについて
	for (int i = 0; i < _countof(sprites); i++)
	{
		//スプライトを生成する
		sprites[i] = Sprite::Create(texnumber, { 0, 0 });
	}
}

void DebugText::Print(const std::string& text, float x, float y, float scale)
{
	//すべての文字について
	for (int i = 0; i < text.size(); i++)
	{
		//最大数文字数超過
		if (spriteIndex >= maxCharCount) {
			break;
		}

		//1文字取り出す(*アスキーコードしか成り立たない)
		const unsigned char& character = text[i];

		//アスキーコードの2ッ段分飛ばした番号を計算
		int fontIndex = character - 32;
		if (character >= 0x7f) {
			fontIndex = 0;
		}

		int fontIndexY = fontIndex / fontLineCount;
		int fontIndexX = fontIndex % fontLineCount;

		//座標計算
		sprites[spriteIndex]->SetPos({ x + fontWidth * scale * i, y });
		sprites[spriteIndex]->texLeftTop = { (float)fontIndexX + fontWidth,(float)fontIndexY + fontHeight };
		sprites[spriteIndex]->texCutSize = { fontWidth,fontHeight };
		sprites[spriteIndex]->SetSize({ fontWidth * scale,fontHeight * scale });

		//更新

		//文字を一つ進める
		spriteIndex++;
	}
}

void DebugText::DrawAll()
{
	//すべての文字のスプライトについて
	for (int i = 0; i < spriteIndex; i++)
	{
		//スプライト描画
		sprites[i]->Draw();
	}

	spriteIndex = 0;
}
