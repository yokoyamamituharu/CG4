#pragma region include関連とWindowProc

#include<DirectXTex.h>

#include<d3dcompiler.h>
#pragma comment(lib,"d3dcompiler.lib")

#include <d3d12.h>
#include <dxgi1_6.h>

#include<vector>
#include<string>

#include<DirectXMath.h>
using namespace DirectX;

#pragma comment(lib, "d3d12.lib")

#pragma comment(lib, "dxgi.lib")

//入力
#define DIRECTINPUT_VERSION 0x0800		//DirectInputのバージョン指定
#include <dinput.h>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")



#pragma endregion
#include<d3dx12.h>
#include<stdlib.h>
#include<time.h>

#include"Input.h"
#include "WinApp.h"
#include"DirectXCommon.h"
#include"Object3d.h"

using namespace Microsoft::WRL;

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


//テクスチャの最大枚数
const int spriteSRVCount = 512;

//パイプラインセット
struct PipelineSet
{
	//パイプラインステートオブジェクト
	ComPtr<ID3D12PipelineState>pipelinestate;
	//ルートシグネチャ
	ComPtr<ID3D12RootSignature>rootsignature;
};

//定数バッファ用データ構造体
struct ConstBufferData {
	XMFLOAT4 color;		//色(RGBA)
	XMMATRIX mat;		//3D変換行列
};

struct Vertex
{
	XMFLOAT3 pos;	//xyz座標
	XMFLOAT3 normal;//法線ベクトル
	XMFLOAT2 uv;	//uｖ
};

//新しい頂点構造体の定義
struct VertexPosUv
{
	XMFLOAT3 pos;
	XMFLOAT2 uv;
};

//スプライト1枚分のデータ
struct Sprite
{
	ComPtr<ID3D12Resource>vertBuff;		//頂点バッファ
	D3D12_VERTEX_BUFFER_VIEW vbView{};	//頂点バッファビュー
	ComPtr<ID3D12Resource> constBuff = nullptr;//定数バッファ
	//Z軸回りの回転角
	float rotation = 0.0f;
	//座標
	XMFLOAT3 position = { 0,0,0 };
	//ワールド行列
	XMMATRIX matWorld;
	//色(RGBA)
	XMFLOAT4 color = { 1,1,1,1 };
	//テクスチャ番号
	UINT texNumber = 0;
	//大きさ
	XMFLOAT2 size = { 100,100 };
};

//スプライトの共通データ
struct SpriteCommon
{
	//パイプラインセット
	PipelineSet pipelineSet;
	//射影行列
	XMMATRIX matProjection{};
	//テクスチャ用デスクリプタヒープの生成
	ComPtr<ID3D12DescriptorHeap> descHeap;
	//テクスチャリソース（テクスチャバッファ）の配列
	ComPtr<ID3D12Resource>texBuffer[spriteSRVCount];
};

//スプライト単体頂点バッファの転送
void SpriteTransferVertexBuffer(const Sprite &sprite)
{
	HRESULT result = S_FALSE;

	//頂点データ
	VertexPosUv vertices[] = {
		{{},{0.0f,1.0f}},//左下
		{{},{0.0f,0.0f}},//左上
		{{},{1.0f,1.0f}},//右下
		{{},{1.0f,0.0f}},//右上
	};

	//左下、左上、右下、右上
	enum { LB, LT, RB, RT };

	vertices[LB].pos = { 0.0f,sprite.size.y,0.0f };//左下
	vertices[LT].pos = { 0.0f,0.0f,0.0f };//左上
	vertices[RB].pos = { sprite.size.x,sprite.size.y,0.0f };//右下
	vertices[RT].pos = { sprite.size.x,0.0f,0.0f };//右上

	//頂点バッファへのデータ転送
	VertexPosUv *vertMap = nullptr;
	result = sprite.vertBuff->Map(0, nullptr, (void **)&vertMap);
	memcpy(vertMap, vertices, sizeof(vertices));
	sprite.vertBuff->Unmap(0, nullptr);
}

//スプライト用パイプライン生成関数
PipelineSet SpriteCreateGraphicsPipeline(ID3D12Device *dev)
{
	//ローカルのresultを生成する
	HRESULT result;


	ComPtr<ID3DBlob> vsBlob = nullptr;//頂点シェーダーオブジェクト
	ComPtr<ID3DBlob> psBlob = nullptr;//ピクセルシェーダーオブジェクト
	ComPtr<ID3DBlob> errorBlob = nullptr;//エラーオブジェクト

	//頂点シェーダーの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"SpriteVS.hlsl",//シェーダーファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,//インクルード可能にする
		"main", "vs_5_0",//エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//でバック用設定
		0,
		&vsBlob, &errorBlob);


	//ピクセルシェーダーの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"SpritePS.hlsl",//シェーダーファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,//インクルードを可能にする
		"main", "ps_5_0",//エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバッグ用設定
		0,
		&psBlob, &errorBlob);


	//頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{//xyz座標
			"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		},
		{//uv座標
			"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		}
	};

	//グラフィックスパイプラインの設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};

	//頂点シェーダー、ピクセルシェーダーをパイプラインに設定
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;//標準設定

	//ラスタライザステート
	//標準的な設定(背面カリング、塗りつぶし、深度クリッピング有効)
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);	//一旦標準値をセット
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;			//背面カリングをしない



	//ブレンドステートの設定
	//レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC &blenddesc = gpipeline.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;


	//デプスステンシルステートの設定
//標準的な設定（深度テストを行う、書き込み許可、深度が小さければ合格）
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);	//一旦標準値をセット
	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;		//常に上書きルール
	gpipeline.DepthStencilState.DepthEnable = false;							//深度テストをしない

	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;//深度値フォーマット

		//頂点レイアウトの設定
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	//図形の形状を三角形に設定
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//その他の設定
	gpipeline.NumRenderTargets = 1;//描画対象は1つ
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;//0～255指定のRGBA
	gpipeline.SampleDesc.Count = 1;//1ピクセルにつき1回サンプリング

	//スタティックサンプラー(テクスチャサンプラー)
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);


	CD3DX12_DESCRIPTOR_RANGE  descRangeSRV;
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);//t0レジスタ



	CD3DX12_ROOT_PARAMETER rootparams[2];
	rootparams[0].InitAsConstantBufferView(0);
	rootparams[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);


	//パイプランとルートシグネチャのセット
	PipelineSet pipelineSet;


	ComPtr<ID3DBlob> rootSigBlob = nullptr;
	//バージョン自動判定でのシリアライズ
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);

	//ルートシグネチャの生成
	result = dev->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&pipelineSet.rootsignature));

	//パイプラインにルートシグネチャをセット
	gpipeline.pRootSignature = pipelineSet.rootsignature.Get();


	result = dev->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelineSet.pipelinestate));



	//パイプラインとルートシグネチャを返す
	return pipelineSet;
}

//スプライト共通データ生成
SpriteCommon SpriteCommonCreate(ID3D12Device *dev, int window_width, int window_height)
{
	HRESULT result = S_FALSE;

	//新たなスプライト共通データを生成
	SpriteCommon spriteCommon{};

	//デスクリプタヒープを生成
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descHeapDesc.NumDescriptors = spriteSRVCount;
	result = dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&spriteCommon.descHeap));

	//スプライト用パイプライン生成
	spriteCommon.pipelineSet = SpriteCreateGraphicsPipeline(dev);

	//平行投影射影行列生成
	spriteCommon.matProjection = XMMatrixOrthographicOffCenterLH(
		0.0f, (float)window_width, (float)window_height, 0.0f, 0.0f, 1.0f);

	//生成したスプライト共通データを返す
	return spriteCommon;
}

//スプライト生成
Sprite SpriteCreate(ID3D12Device *dev, int window_width, int window_height, UINT texNumber, const SpriteCommon &spriteCommon)
{
	HRESULT result = S_FALSE;

	//新しいスプライトを作る
	Sprite sprite{};

	//テクスチャ番号をコピー
	sprite.texNumber = texNumber;

	//頂点データ
	VertexPosUv vertices[] =
	{
		{{  0.0f,100.0f,0.0f},{0.0f,1.0f}},
		{{  0.0f,  0.0f,0.0f},{0.0f,0.0f}},
		{{100.0f,100.0f,0.0f},{1.0f,1.0f}},
		{{100.0f,  0.0f,0.0f},{1.0f,0.0f}}
	};

	//頂点バッファ生成
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertices)),
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&sprite.vertBuff));

	//指定番号の画像が読み込み済みなら
	if (spriteCommon.texBuffer[sprite.texNumber])
	{
		//テクスチャ情報取得
		D3D12_RESOURCE_DESC resDesc = spriteCommon.texBuffer[sprite.texNumber]->GetDesc();
		//スプライトの大きさを画像の解像度に合わせる
		sprite.size = { (float)resDesc.Width,(float)resDesc.Height };
	}
	SpriteTransferVertexBuffer(sprite);

	//頂点バッファへのデータ転送
	VertexPosUv *vertMap = nullptr;
	result = sprite.vertBuff->Map(0, nullptr, (void **)&vertMap);
	memcpy(vertMap, vertices, sizeof(vertices));
	sprite.vertBuff->Unmap(0, nullptr);

	//頂点バッファビューの作成
	sprite.vbView.BufferLocation = sprite.vertBuff->GetGPUVirtualAddress();
	sprite.vbView.SizeInBytes = sizeof(vertices);
	sprite.vbView.StrideInBytes = sizeof(vertices[0]);

	//定数バッファの生成
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&sprite.constBuff));

	// 定数バッファにデータ転送
	ConstBufferData *constMap = nullptr;
	result = sprite.constBuff->Map(0, nullptr, (void **)&constMap);
	constMap->color = XMFLOAT4(1, 1, 1, 1); //色指定(RGBA)
	//平行投影行列
	constMap->mat = XMMatrixOrthographicOffCenterLH(
		0.0f, window_width, window_height, 0.0f, 0.0f, 1.0f);
	sprite.constBuff->Unmap(0, nullptr);

	return sprite;
}

//スプライト共通テクスチャ読み込み
void SpriteCommonLoadTexture(SpriteCommon &spriteCommon, UINT texnumber, const wchar_t *filename, ID3D12Device *dev)
{
	//異常な番号の指定を検出
	assert(texnumber <= spriteSRVCount - 1);

	HRESULT result;

	//WICテクスチャのロード
	TexMetadata metadata{};
	ScratchImage scratchImg{};

	result = LoadFromWICFile(
		filename,
		WIC_FLAGS_NONE,
		&metadata, scratchImg);

	const Image *img = scratchImg.GetImage(0, 0, 0);	//生データー抽出


	//リソース設定
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format,
		metadata.width,
		(UINT)metadata.height,
		(UINT16)metadata.arraySize,
		(UINT16)metadata.mipLevels);

	//テクスチャ用バッファの生成
	result = dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,//テクスチャ用指定
		nullptr,
		IID_PPV_ARGS(&spriteCommon.texBuffer[texnumber]));

	// テクスチャバッファにデータ転送
	result = spriteCommon.texBuffer[texnumber]->WriteToSubresource(
		0,
		nullptr, // 全領域へコピー
		img->pixels,    // 元データアドレス
		(UINT)img->rowPitch,    // 1ラインサイズ
		(UINT)img->slicePitch   // 1枚サイズ
	);

	//シェーダリソースビュー設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};//設定構造体
	//srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;//RGBA
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = 1;

	//ヒープのtexnumber番目にシェーダーリソースビュー作成
	dev->CreateShaderResourceView(
		spriteCommon.texBuffer[texnumber].Get(),//ビューと関連付けるバッファ
		&srvDesc,//テクスチャ設定情報
		CD3DX12_CPU_DESCRIPTOR_HANDLE(spriteCommon.descHeap->GetCPUDescriptorHandleForHeapStart(), texnumber,
			dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		)
	);

	result = S_OK;
}

//スプライト単体更新
void SpriteUpdate(Sprite &sprite, const SpriteCommon &spriteCommon)
{
	//ワールド行列の更新
	sprite.matWorld = XMMatrixIdentity();
	//Z軸回転
	sprite.matWorld *= XMMatrixRotationZ(XMConvertToRadians(sprite.rotation));
	//平行移動
	sprite.matWorld *= XMMatrixTranslation(sprite.position.x, sprite.position.y, sprite.position.z);

	//定数バッファの転送
	ConstBufferData *constMap = nullptr;
	HRESULT result = sprite.constBuff->Map(0, nullptr, (void **)&constMap);
	constMap->mat = sprite.matWorld * spriteCommon.matProjection;
	constMap->color = sprite.color;
	sprite.constBuff->Unmap(0, nullptr);
}

//スプライト共通グラフィックコマンドのセット
void SpriteCommonBeginDraw(const SpriteCommon &spriteCommon, ID3D12GraphicsCommandList *cmdList)
{
	//パイプラインステートの設定
	cmdList->SetPipelineState(spriteCommon.pipelineSet.pipelinestate.Get());
	//ルートシグネチャの設定
	cmdList->SetGraphicsRootSignature(spriteCommon.pipelineSet.rootsignature.Get());
	//プリミティブ形状を設定
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//テクスチャ用デスクリプタヒープの設定
	ID3D12DescriptorHeap *ppHeaps[] = { spriteCommon.descHeap.Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
}

//スプライト単体描画
void SpriteDraw(const Sprite &sprite, ID3D12GraphicsCommandList *cmdList, const SpriteCommon &spriteCommon,
	ID3D12Device *dev)
{
	//頂点バッファをセット
	cmdList->IASetVertexBuffers(0, 1, &sprite.vbView);

	//定数バッファをセット
	cmdList->SetGraphicsRootConstantBufferView(0, sprite.constBuff->GetGPUVirtualAddress());

	//シェーダリソースビューをセット
	cmdList->SetGraphicsRootDescriptorTable(1,
		CD3DX12_GPU_DESCRIPTOR_HANDLE(
			spriteCommon.descHeap->GetGPUDescriptorHandleForHeapStart(),
			sprite.texNumber,
			dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

	//ポリゴンの描画（4頂点で四角形）
	cmdList->DrawInstanced(4, 1, 0, 0);
}


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	HRESULT result;
#pragma region 初期化処理

	// DirectX初期化処理　ここから
	//ポインタ置き場
	WinApp *winApp = nullptr;
	winApp = new WinApp();
	winApp->Initialize();

	DirectXCommon *dxCommon = nullptr;
	//DirectXの初期化
	dxCommon = new DirectXCommon();
	dxCommon->Initialize(winApp);

	//入力
	Input *input = nullptr;
	input = new Input();
	input->Initialize(winApp->GetHInstance(), winApp->GetHwnd());

	//モデルの静的初期化
	Model::StaticInitialize(dxCommon->GetDev());
	//3Dオブジェクト静的初期化
	Object3d::StaticInitialize(dxCommon->GetDev(), WinApp::window_width, WinApp::window_height);



	PipelineSet spritePipelineSet = SpriteCreateGraphicsPipeline(dxCommon->GetDev());
	//スプライト共通データ
	SpriteCommon spriteCommon;
	//スプライト共通データ生成
	spriteCommon = SpriteCommonCreate(dxCommon->GetDev(), winApp->window_width, winApp->window_height);
	//スプライト共通テクスチャ読み込み
	SpriteCommonLoadTexture(spriteCommon, 0, L"Resources/texture.jpg", dxCommon->GetDev());
	SpriteCommonLoadTexture(spriteCommon, 1, L"Resources/sora.png", dxCommon->GetDev());
	SpriteCommonLoadTexture(spriteCommon, 2, L"Resources/make.png", dxCommon->GetDev());
	SpriteCommonLoadTexture(spriteCommon, 3, L"Resources/kati.png", dxCommon->GetDev());
	SpriteCommonLoadTexture(spriteCommon, 10, L"Resources/0.png", dxCommon->GetDev());
	SpriteCommonLoadTexture(spriteCommon, 11, L"Resources/1.png", dxCommon->GetDev());
	SpriteCommonLoadTexture(spriteCommon, 12, L"Resources/2.png", dxCommon->GetDev());
	SpriteCommonLoadTexture(spriteCommon, 13, L"Resources/3.png", dxCommon->GetDev());
	SpriteCommonLoadTexture(spriteCommon, 14, L"Resources/4.png", dxCommon->GetDev());
	SpriteCommonLoadTexture(spriteCommon, 15, L"Resources/5.png", dxCommon->GetDev());
	SpriteCommonLoadTexture(spriteCommon, 16, L"Resources/6.png", dxCommon->GetDev());
	SpriteCommonLoadTexture(spriteCommon, 17, L"Resources/7.png", dxCommon->GetDev());
	SpriteCommonLoadTexture(spriteCommon, 18, L"Resources/8.png", dxCommon->GetDev());
	SpriteCommonLoadTexture(spriteCommon, 19, L"Resources/9.png", dxCommon->GetDev());



	// DirectX初期化処理　ここまで

#pragma endregion


	//スプライト
	Sprite sprite;
	//スプライトの生成
	sprite = SpriteCreate(dxCommon->GetDev(), winApp->window_width, winApp->window_height, 0, spriteCommon);
	//スプライト
	Sprite spriteWae;
	//スプライトの生成
	spriteWae = SpriteCreate(dxCommon->GetDev(), winApp->window_width, winApp->window_height, 1, spriteCommon);
	//スプライトの更新
	sprite.rotation = 0;
	sprite.position = { 0,0,0 };
	sprite.color = { 1,1,1,1 };
	SpriteUpdate(sprite, spriteCommon);
	sprite.size = { 1280,750 };
	//頂点バッファに反映
	SpriteTransferVertexBuffer(sprite);
	spriteWae.rotation = 0;
	spriteWae.position = { 0,0,0 };
	//spriteWae.color = { 1,1,0,1 };	
	SpriteUpdate(spriteWae, spriteCommon);
	spriteWae.size = { 1280,800 };
	SpriteTransferVertexBuffer(spriteWae);

	Sprite sprite1 = SpriteCreate(dxCommon->GetDev(), winApp->window_width, winApp->window_height, 11, spriteCommon);
	sprite1.rotation = 0;
	sprite1.position = { 1150,0,0 };
	SpriteUpdate(sprite1, spriteCommon);
	sprite1.size = { 50,50 };
	SpriteTransferVertexBuffer(sprite1);

	Sprite sprite2 = SpriteCreate(dxCommon->GetDev(), winApp->window_width, winApp->window_height, 11, spriteCommon);
	sprite2.rotation = 0;
	sprite2.position = { 1200,0,0 };
	SpriteUpdate(sprite2, spriteCommon);
	sprite2.size = { 50,50 };
	SpriteTransferVertexBuffer(sprite2);



	//変数宣言
	/*モデル生成*/
	Model *playermodel = Model::Create();
	playermodel->CreateFromOBJ("player");
	Model *enemymodel = Model::Create();
	enemymodel->CreateFromOBJ("enemy");
	Model *titleModel = Model::Create();
	titleModel->CreateFromOBJ("title");
	Model *endModel = Model::Create();
	endModel->CreateFromOBJ("end");

	Model *backModel1 = Model::Create();
	backModel1->CreateFromOBJ("backb");

	Model *ropeModel = Model::Create();
	ropeModel->CreateFromOBJ("rope");

	/*Model *zeroModel = Model::Create();
	endModel->CreateFromOBJ("0");
	Model *oneModel = Model::Create();
	endModel->CreateFromOBJ("1");
	Model *twoModel = Model::Create();
	endModel->CreateFromOBJ("2");
	Model *threeModel = Model::Create();
	endModel->CreateFromOBJ("3");
	Model *fourModel = Model::Create();
	endModel->CreateFromOBJ("4");
	Model *fiveModel = Model::Create();
	endModel->CreateFromOBJ("5");
	Model *sixModel = Model::Create();
	endModel->CreateFromOBJ("6");
	Model *sevenModel = Model::Create();
	endModel->CreateFromOBJ("7");
	Model *eightModel = Model::Create();
	endModel->CreateFromOBJ("8");
	Model *nineModel = Model::Create();
	endModel->CreateFromOBJ("9");*/



	/*3dオブジェクト生成*/
	//プレイヤー
	Object3d *playerobj = Object3d::Create();
	playerobj->SetModel(playermodel);
	playerobj->SetScale({ 1.0f,1.0f,1.0f });
	playerobj->SetPosition({ 0.0f,0.0f,0.0f });
	playerobj->SetRotation({ 0.0f,150.0f,0.0f });
	//敵
	const int enemynum = 20;
	int isenemy[enemynum] = { 0 };	//画面内にいる敵のかず
	//isenemy[0] = 1;
	Object3d *bardobj[enemynum];
	for (int i = 0; i < enemynum; i++)
	{
		bardobj[i] = Object3d::Create();
		bardobj[i]->SetModel(enemymodel);
		bardobj[i]->SetScale({ 1.0f,1.0f,1.0f });
		bardobj[i]->SetPosition({ 0.0f,0.0f,0.0f });
		bardobj[i]->SetRotation({ 0.0f,90.0f,0.0f });
	}
	//UIとか
	//タイトル
	Object3d *titleobj = Object3d::Create();
	titleobj->SetModel(titleModel);
	titleobj->SetScale({ 20.0f,20.0f,20.0f });
	titleobj->SetPosition({ 0.0f,0.0f,0.0f });
	titleobj->SetRotation({ -90.0f, 0.0f, 0.0f });
	//タイトル用の背景
	Object3d *backobj = Object3d::Create();
	backobj->SetModel(backModel1);
	backobj->SetScale({ 10.0f,10.0f,10.0f });
	backobj->SetPosition({ 0.0f,0.0f,0.0f });
	backobj->SetRotation({ -90.0f, 0.0f, 0.0f });

	//えんど
	Object3d *endobj = Object3d::Create();
	endobj->SetModel(endModel);
	endobj->SetScale({ 20.0f,20.0f,20.0f });
	endobj->SetPosition({ 0.0f,0.0f,0.0f });
	endobj->SetRotation({ -90.0f, 0.0f, 0.0f });

	//ロープ
	Object3d *ropeobj = Object3d::Create();
	ropeobj->SetModel(ropeModel);
	ropeobj->SetScale({ 2.0f,10.0f,2.0f });
	ropeobj->SetPosition({ 1.5f,60.0f,-3.0f });
	ropeobj->SetRotation({ 0.0f, 0.0f, 0.0f });



	//オブジェクトの初期設定の反映
	playerobj->Update();
	for (int i = 0; i < enemynum; i++)
	{
		bardobj[i]->Update();
	}
	titleobj->Update();
	endobj->Update();
	backobj->Update();
	ropeobj->Update();

	const int spowntimeInit = 50;
	int spowntime = spowntimeInit;
	int gameTimerInit = 1800;
	int gameTimer = gameTimerInit;



	int respowntime = 0;
	float bardspeed = 0.8f;
	srand(time(NULL));	//時間を乱数のシードに

	int point = 5;
	int score = 0;	//スコア

	enum
	{
		title,
		game,
		end
	};
	int scene = title;


	bool hit = false;


	while (true)  // ゲームループ
	{
		// ブロック内はページ右側を参照
		// メッセージがある？
		if (winApp->ProcessMessage())
		{
			break;
		}

		//DirectX毎フレーム処理　ここから
		//-----入力の更新処理
		input->Update();


#pragma region 更新処理



#pragma endregion

		//プレイヤーの更新処理
		XMFLOAT3 playerpos = playerobj->GetPosition();
		if (input->PushKey(DIK_W))
		{
			playerpos.y += 1.0f;
		}
		if (input->PushKey(DIK_S))
		{
			playerpos.y -= 1.0f;
		}
		if (input->PushKey(DIK_A))
		{
			playerpos.x -= 1.0f;
		}
		if (input->PushKey(DIK_D))
		{
			playerpos.x += 1.0f;
		}
		playerobj->SetPosition(playerpos);


		//3Dオブジェクト更新
		playerobj->Update();

		//DirectX毎フレーム処理　ここまで


		//描画処理
		dxCommon->PreDraw();


		//スプライト共通コマンド
		SpriteCommonBeginDraw(spriteCommon, dxCommon->GetCmdList());
		//スプライト描画
		//SpriteDraw(spriteWae, dxCommon->GetCmdList(), spriteCommon, dxCommon->GetDev());
		if (scene == game)
		{
			int time = gameTimer / 60;
			sprite1.texNumber = time / 10 + 10;
			if (sprite1.texNumber < 0)
			{
				sprite1.texNumber = 0;
			}
			sprite2.texNumber = time % 10 + 10;
			if (sprite2.texNumber < 0)
			{
				sprite2.texNumber = 0;
			}
			//SpriteDraw(sprite1, dxCommon->GetCmdList(), spriteCommon, dxCommon->GetDev());
			//SpriteDraw(sprite2, dxCommon->GetCmdList(), spriteCommon, dxCommon->GetDev());
		}

		//3Dオブジェクト描画前処理
		Object3d::PreDraw(dxCommon->GetCmdList());

		if (scene == title)
		{
		//	titleobj->Draw();
		//	playerobj->Draw();
		}
		else if (scene == game)
		{
			//スプライト共通コマンド
			//SpriteCommonBeginDraw(spriteCommon, dxCommon->GetCmdList());
			//スプライト描画
			//SpriteDraw(sprite1, dxCommon->GetCmdList(), spriteCommon, dxCommon->GetDev());
			//SpriteDraw(sprite2, dxCommon->GetCmdList(), spriteCommon, dxCommon->GetDev());

			//3Dオブジェクト描画処理			
		//	playerobj->Draw();
		//	ropeobj->Draw();

			for (int i = 0; i < enemynum; i++)
			{
				if (isenemy[i] > 0)
				{
		//			bardobj[i]->Draw();
				}
			}
			if (hit)
			{
			//	backobj->Draw();
			}
		}
		else
		{
			SpriteCommonBeginDraw(spriteCommon, dxCommon->GetCmdList());

		//	SpriteDraw(sprite, dxCommon->GetCmdList(), spriteCommon, dxCommon->GetDev());
			//endobj->Draw();
		}

		playerobj->Draw();


		//3Dオブジェクト描画後処理
		Object3d::PostDraw();
		dxCommon->PostDraw();
	}


	winApp->Finalize();

	delete winApp;
	winApp = nullptr;
	//DirectX解放
	delete dxCommon;
	//入力の解放
	delete input;
	//3Dオブジェクト解放
	delete playerobj;

	return 0;
}