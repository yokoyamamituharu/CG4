#include "GameScene.h"

GameScene::GameScene()
{
}

GameScene::~GameScene()
{
	//3Dオブジェクト解放
	delete fbxobject;
}

void GameScene::Initialize(DirectXCommon* dxCommon, Input* input, Camera* camera)
{
	this->dxCommon = dxCommon;
	this->input = input;
	this->camera = camera;	

	//カメラの初期位置、注視点
	camera->SetTarget({ 0,0,20 });
	camera->SetEye({ 0,-0,-20 });

	//スプライト共通テクスチャ読み込み
	Sprite::LoadTexture(0, L"Resources/texture.png");
	Sprite::LoadTexture(1, L"Resources/torisetu.png");	

	//スプライトの生成
	spriteBG = Sprite::Create(0, {0.0f,0.0f});


	//モデルの読み込み
	fbxmodel = FbxLoader::GetInstance()->LoadModelFromFile("cube");
	//3Dオブジェクトの生成
	fbxobject = new FBXObject;
	fbxobject->Initialize();
	fbxobject->SetModel(fbxmodel);
	fbxobject->PlayAnimetion();
	fbxobject->SetPos({ 0,0,+80 });
}

void GameScene::Update()
{
	//カメラの移動
	XMFLOAT3 camerapos = { 0,0,0 };
	float num = 0.8;
	if (input->PushKey(DIK_W)){
		camerapos.y += num;
	}
	if (input->PushKey(DIK_S)){
		camerapos.y -= num;
	}
	if (input->PushKey(DIK_A)){
		camerapos.x -= num;
	}
	if (input->PushKey(DIK_D)){
		camerapos.x += num;
	}

	if (input->PushKey(DIK_Q)){
		camerapos.z -= num;
	}
	if (input->PushKey(DIK_E)){
		camerapos.z += num;
	}
	//カメラの更新
	camera->MoveVector(camerapos);
	camera->Update();

	//3Dオブジェクト更新
	fbxobject->Update();
}

void GameScene::Draw()
{
	Sprite::PreDraw(dxCommon->GetCmdList());
	spriteBG->Draw();
	Sprite::PostDraw();

	OBJobject::PreDraw(dxCommon->GetCmdList());
	fbxobject->Draw(dxCommon->GetCmdList());
	OBJobject::PostDraw();

}
