#include "BatlleScene.h"

BatlleScene::BatlleScene()
{
}

BatlleScene::~BatlleScene()
{
}

void BatlleScene::Initialize(DirectXCommon* dxCommon, Input* input, InputMouse* mouse, Camera* camera,GameScene* gameScene)
{
	assert(dxCommon);
	this->dxCommon = dxCommon;
	this->input = input;
	this->mouse = mouse;
	this->camera = camera;

	this->player = gameScene->GetPlayer();

	grundmodel = new Model();
	grundmodel->CreateFromOBJ("battlegrund");
	ground = OBJobject::Create();
	ground->SetModel(grundmodel);
	ground->SetScale({ 10.0f,1.0f,10.0f });
	ground->SetPosition({ 0.0f,-10.0f,0.0f });
	ground->SetRotation({ 0.0f,0.0f,0.0f });
}

void BatlleScene::Update()
{
	camera->Update();	
	ground->Update();
	player->Update();
}

void BatlleScene::Draw()
{
	Sprite::PreDraw(dxCommon->GetCmdList());
	//spriteBG->Draw();
	Sprite::PostDraw();

	OBJobject::PreDraw(dxCommon->GetCmdList());
	player->object->Draw();
	ground->Draw();
	//fbxobject->Draw(dxCommon->GetCmdList());

	OBJobject::PostDraw();
}
