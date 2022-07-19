#pragma once
#include "FbxModel.h"
#include "Camera.h"

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <string>

class FBXObject
{
protected://エイリアス
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public://定数
	//ボーンの最大数
	static const int MAX_BONES = 32;

public://サブクラス
	struct ConstBufferDataTransform
	{
		XMMATRIX viewproj;
		XMMATRIX world;
		XMFLOAT3 cameraPos;
	};

	//定数バッファ用データ構造体(スキニング)
	struct ConstBufferDataSkin
	{
		XMMATRIX bones[MAX_BONES];
	};



public://静的メンバ関数
	static bool StaticInitialize(ID3D12Device* device, Camera* camera);

	static void SetDevice(ID3D12Device* device) { FBXObject::device = device; }
	static void SetCamera(Camera* camera) { FBXObject::camera = camera; }
	//グラッフィクパイプラインの生成
	static void CreateGraphicsPipline();

public:
	//コンストラクタ
	FBXObject();
	//デストラクタ
	~FBXObject();

	//アニメーションの開始
	void PlayAnimetion();

	void Initialize();
	//毎フレーム処理
	void Update();
	//描画
	void Draw(ID3D12GraphicsCommandList* cmdList);

	void SetModel(FbxModel* model) { this->model = model; }

public://ゲッター・セッター
	void SetPos(XMFLOAT3 pos) { position = pos; }

private:
	static ID3D12Device* device;
	static Camera* camera;
	//ルートシグネチャ
	static ComPtr<ID3D12RootSignature>rootsignature;
	//パイプラインステートオブジェクト
	static ComPtr<ID3D12PipelineState>pipelinestate;

protected:
	//定数バッファ
	ComPtr<ID3D12Resource> constBuffTransform;
	//ローカルスケール
	XMFLOAT3 scale = {1,1,1 };
	//X,Y,Z軸回りのローカル回転角
	XMFLOAT3 rotation = { 0,0,0 };
	//ローカル座標
	XMFLOAT3 position = { 0,0,0 };
	//ローカルワールド変換行列
	XMMATRIX matWorld;
	//モデル
	FbxModel* model = nullptr;
	//定数バッファスキン
	ComPtr<ID3D12Resource>constBuffSkin;

	//1フレームの時間
	FbxTime frameTime;
	//アニメーション開始時間
	FbxTime startTime;
	//アニメーション終了時間
	FbxTime endTime;
	//現在時間（アニメーション）
	FbxTime currentTime;
	//アニメーション再生中
	bool isPlay = false;

};

