#pragma region include関連
#include <DirectXTex.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dx12.h>
#define DIRECTINPUT_VERSION 0x0800 //DirectInputのバージョン指定
#include <dinput.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <time.h>
#include "fbxsdk.h"

#include "WinApp.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "PostEffect.h"
#include "Camera.h"
#include "Sprite.h"
#include "OBJobject.h"
#include "FBXObject.h"
#include "GameScene.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"d3dcompiler.lib")

using namespace Microsoft::WRL;
using namespace DirectX;
#pragma endregion

#include "InputMouse.h"
#include "DebugText.h"

//立方体の当たり判定
bool CubeCollision(XMFLOAT3 object1, XMFLOAT3 radius1, XMFLOAT3 object2, XMFLOAT3 radius2) {
	float disX1 = object1.x + radius1.x;
	float disX2 = object2.x - radius2.x;
	float disX3 = object1.x - radius1.x;
	float disX4 = object2.x + radius2.x;
	float disY1 = object1.y + radius1.y;
	float disY2 = object2.y - radius2.y;
	float disY3 = object1.y - radius1.y;
	float disY4 = object2.y + radius2.y;
	float disZ1 = object1.z + radius1.z;
	float disZ2 = object2.z - radius2.z;
	float disZ3 = object1.z - radius1.z;
	float disZ4 = object2.z + radius2.z;

	//xとyはそれぞれ中心座標として計算する
	return disX1 > disX2 && disX4 > disX3 && disY1 > disY2 && disY4 > disY3 && disZ1 > disZ2 && disZ4 > disZ3;
}


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	HRESULT result;
	FbxManager* fbxManager = FbxManager::Create();

	//-----初期化処理-----//
	WinApp* winApp = nullptr;
	winApp = new WinApp();
	winApp->Initialize();
	DirectXCommon* dxCommon = nullptr;
	//DirectXの初期化
	dxCommon = new DirectXCommon();
	dxCommon->Initialize(winApp);
	//入力
	Input* input = nullptr;
	input = Input::GetInstance();
	input->Initialize(winApp->GetHInstance(), winApp->GetHwnd());
	//マウス
	InputMouse* mouse = nullptr;
	mouse = new InputMouse();
	mouse->Initialize(winApp->GetHInstance(), winApp->GetHwnd());
	//カメラ
	Camera* camera = nullptr;
	camera = new Camera(winApp->window_width, winApp->window_height);
	//スプライトの静的初期化
	Sprite::StaticInitialize(dxCommon->GetDev(), WinApp::window_width, WinApp::window_height);
	//モデルの静的初期化
	Model::StaticInitialize(dxCommon->GetDev());
	//3Dオブジェクト静的初期化
	OBJobject::StaticInitialize(dxCommon->GetDev(), WinApp::window_width, WinApp::window_height, camera);
	//FBX		
	FbxLoader::GetInstance()->Initialize(dxCommon->GetDev());
	FBXObject::StaticInitialize(dxCommon->GetDev(), camera);

	//-----変数宣言-----//
	//ポストエフェクトの初期化
	PostEffect* postEffect = nullptr;
	postEffect = new PostEffect();
	postEffect->Initialize();

	GameScene* gameScene = nullptr;
	gameScene = new GameScene;
	gameScene->Initialize(dxCommon, input, camera);

	bool isSetMousePoint = false;	//マウスのポインタの位置を固定するかどうか

	Sprite::LoadTexture(1, L"Resources/texture.png");
	DebugText debugText;
	debugText.Initialize(dxCommon->GetDev(), winApp->window_width, winApp->window_height, 1);

	debugText.Print("Hello,DirectX!", 200, 100);
	debugText.Print("koko benko", 400, 200, 2);

	Sprite* sprite0 = Sprite::Create(1, { 0,0 });

	sprite0->SetSize({ 5, 5 });

	while (true)  // ゲームループ
	{
		// ブロック内はページ右側を参照
		// メッセージがある？
		if (winApp->ProcessMessage())
		{
			break;
		}
		//-----更新処理-----//
		//入力の更新処理
		input->Update();
		mouse->Update();
		if (input->TriggerKey(DIK_M)) {	//Mキーを押されたらマウスポインターの移動を停止
			if (isSetMousePoint) { isSetMousePoint = false; }
			else { isSetMousePoint = true; }
		}
		if (isSetMousePoint) {
			SetCursorPos(1280 / 2, 720 / 2);
		}

		//クリック確認用
		if (mouse->PushMouse(MouseDIK::M_RIGHT))
		{
			int a = 0;
		}
		if (mouse->PushMouse(MouseDIK::M_LEFT))
		{
			int a = 0;
		}


		//シーンの更新
		gameScene->Update();
		debugText.Print("Hello,DirectX!", 200, 100);
		//debugText.Print("koko benko", 400, 200, 2);
		//-----描画処理-----//
		//ポストエフェクトの準備
		//postEffect->PreDrawScene(dxCommon->GetCmdList());
		//postEffect->PosDrawScene(dxCommon->GetCmdList());

		dxCommon->PreDraw();
		//postEffect->Draw(dxCommon->GetCmdList());		
		//gameScene->Draw();
		Sprite::PreDraw(dxCommon->GetCmdList());
		debugText.DrawAll();
		//sprite0->Draw();
		Sprite::PostDraw();
		dxCommon->PostDraw();
	}

	//winAppの解放
	winApp->Finalize();
	delete winApp;
	winApp = nullptr;
	//DirectX解放
	delete dxCommon;
	//入力の解放
	//delete input;
	//シーンの開放
	delete gameScene;
	//FBXの解放処理
	FbxLoader::GetInstance()->Finalize();
	return 0;
}