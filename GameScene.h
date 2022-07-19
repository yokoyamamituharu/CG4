#pragma once
#include "WinApp.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "PostEffect.h"
#include "Camera.h"
#include "Sprite.h"
#include "OBJobject.h"
#include "FBXObject.h"
#include "FbxLoader.h"

#include <vector>

class GameScene
{
private:    
    template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
    using XMFLOAT2 = DirectX::XMFLOAT2;
    using XMFLOAT3 = DirectX::XMFLOAT3;
    using XMFLOAT4 = DirectX::XMFLOAT4;
    using XMVECTOR = DirectX::XMVECTOR;
    using XMMATRIX = DirectX::XMMATRIX;

public:
    //コンストラクタ
    GameScene();
    //デストラクタ
    ~GameScene();
    //初期化
	void Initialize(DirectXCommon* dxCommon,Input* input,Camera* camera);
    //更新
	void Update();
    //描画
	void Draw();

private: // メンバ変数
    DirectXCommon* dxCommon = nullptr;
    Input* input = nullptr;
    Camera* camera;

    // ゲームシーン用  
    //スプライト
    Sprite* spriteBG = nullptr;
    //3Dオブジェクト
    FbxModel* fbxmodel = nullptr;
    FBXObject* fbxobject = nullptr;

};

