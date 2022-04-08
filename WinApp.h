#pragma once
#include <wrl.h>

class WinApp
{
public://静的メンバ関数
	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

public://メンバ関数
	//初期化
	void Initialize();
	//メッセージ処理
	bool ProcessMessage();
	//終了
	void Finalize();

	//getter
	HWND GetHwnd() { return hwnd; };

	HINSTANCE GetHInstance() { return w.hInstance; }

public://メンバ変数
			// ウィンドウサイズ
	static const int window_width = 1280;  // 横幅
	static const int window_height = 720;  // 縦幅

private://メンバ変数


	WNDCLASSEX w{}; // ウィンドウクラスの設定

	HWND hwnd = nullptr;


};