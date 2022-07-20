#pragma once
#include <Windows.h>
#include <wrl.h>

#define DIRECTINPUT_VERSION     0x0800          // DirectInputのバージョン指定
#include <dinput.h>

enum MouseDIK
{
	M_RIGHT,
	M_LEFT,
	M_SCROLL
};
class InputMouse
{
public:


	private: // エイリアス
		// Microsoft::WRL::を省略
		template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	public: // メンバ関数
		/// <summary>
		/// 初期化
		/// </summary>
		/// <returns>成否</returns>
		bool Initialize(HINSTANCE hInstance, HWND hwnd);

		/// <summary>
		/// 毎フレーム処理
		/// </summary>
		void Update();

		bool PushMouse(MouseDIK click);

		int MoveMouse(char h);

		float MoveMouseVector(char h);

	private: // メンバ変数
		ComPtr<IDirectInput8> dinput;

		ComPtr<IDirectInputDevice8> devmouse;
		DIMOUSESTATE2 mouse;
		DIMOUSESTATE2 oldmouse;

	};
