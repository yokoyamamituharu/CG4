#include "InputMouse.h"
#include <cassert>
#pragma comment(lib, "dinput8.lib")

// デバイス発見時に実行される
BOOL CALLBACK DeviceFindCallBack(LPCDIDEVICEINSTANCE ipddi, LPVOID pvRef)
{
	return DIENUM_CONTINUE;
}

bool InputMouse::Initialize(HINSTANCE hInstance, HWND hwnd)
{
	HRESULT result = S_FALSE;

	// DirectInputオブジェクトの生成	
	result = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&dinput, nullptr);
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	//-------------------マウス---------------------//

	// マウスデバイスの生成	
	result = dinput->CreateDevice(GUID_SysMouse, &devmouse, NULL);
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	// 入力データ形式のセット
	result = devmouse->SetDataFormat(&c_dfDIMouse); // 標準形式
	if (FAILED(result)) {
		assert(0);
		return result;
	}
	// 排他制御レベルのセット
	result = devmouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	return true;
}

void InputMouse::Update()
{
	HRESULT result;

	//---------------------------マウス------------------//
	result = devmouse->Acquire();	// キーボード動作開始

	// 前回のマウス入力を保存
	//memcpy(keyPre, key, sizeof(key));
	oldmouse = mouse;

	// マウスの入力
	result = devmouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouse);

}


bool InputMouse::PushMouse(MouseDIK click)
{
	//左クリック
	if (click == MouseDIK::M_LEFT && mouse.rgbButtons[0] & (0x80))
	{
		return true;
	}

	//右クリック
	if (click == MouseDIK::M_RIGHT && mouse.rgbButtons[1] & (0x80))
	{
		return true;
	}

	return false;
}

int InputMouse::MoveMouse(char h)
{
	if (h == 'x')
	{
		if (mouse.lX > 0)
		{
			return 1;
		}
		if (mouse.lX < 0)
		{
			return 2;
		}
	}
	if (h == 'y')
	{
		if (mouse.lY > 0)
		{
			return 3;
		}
		if (mouse.lY < 0)
		{
			return 4;
		}
	}
	return 0;
}


float InputMouse::MoveMouseVector(char h)
{
	if (h == 'x')
	{
		return mouse.lX;
	}
	if (h == 'y')
	{
		return mouse.lY;
	}
	return 0;
}

