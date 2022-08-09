#pragma once
#include "WinApp.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "InputMouse.h"
#include "PostEffect.h"
#include "Camera.h"
#include "Sprite.h"
#include "OBJobject.h"
#include "FBXObject.h"
#include "FbxLoader.h"
#include "Player.h"
#include <vector>  
#include "GameScene.h"

class BatlleScene
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
        BatlleScene();
        //デストラクタ
        ~BatlleScene();
        //初期化
        void Initialize(DirectXCommon* dxCommon, Input* input, InputMouse* mouse, Camera* camera,GameScene* gameScene);
        //更新
        void Update();
        //描画
        void Draw();

    private: // メンバ変数
        DirectXCommon* dxCommon = nullptr;
        Input* input = nullptr;
        InputMouse* mouse = nullptr;
        Camera* camera;

        // ゲームシーン用  
        //スプライト
        Sprite* spriteBG = nullptr;
        //3Dオブジェクト
        FbxModel* fbxmodel = nullptr;
        FBXObject* fbxobject = nullptr;

        Model* playermodel = nullptr;
        Model* playerSpheremodel = nullptr;
        Player* player = nullptr;
        Model* grundmodel = nullptr;
        OBJobject* ground = nullptr;
    };

