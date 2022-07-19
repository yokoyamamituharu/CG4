#pragma once
#include "Sprite.h"
class PostEffect :
	public Sprite
{
public:
	PostEffect();

	//初期化
	void Initialize();

	//シーン描画前処理
	void PreDrawScene(ID3D12GraphicsCommandList* cmdList);

	//シーン描画後処理
	void PosDrawScene(ID3D12GraphicsCommandList* cmdList);

	void Draw(ID3D12GraphicsCommandList* cmdList);

	int GetPipelineNum() { return pipelineNum; }

private:
	//パイプライン生成
	void CreateGraphicsPipeLineState();


private://静的メンバ変数
	//画面クリアカラー
	static const float clearColor[4];


private://メンバ変数
	//ポストエフェクトの数
	static const int postNum = 3;
	ComPtr<ID3D12Resource>texBuff[2];
	//SRV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap>descHeapSRV;
	//深度バッファ
	ComPtr<ID3D12Resource>depthBuff;
	//RTV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap>descHeapRTV;
	//DSV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap>descHeapDSV;

	//グラフィックスパイプライン
	ComPtr<ID3D12PipelineState> pipelineState[postNum];
	//ルートシグネチャ
	ComPtr<ID3D12RootSignature> rootSignature;

	//グラフィックパイラプライン選択用変数
	int pipelineNum = 0;

};

