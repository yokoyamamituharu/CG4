#include "GameScene.h"

GameScene::GameScene()
{
}

GameScene::~GameScene()
{
	//3D�I�u�W�F�N�g���
	delete fbxobject;
}

void GameScene::Initialize(DirectXCommon* dxCommon, Input* input, Camera* camera)
{
	this->dxCommon = dxCommon;
	this->input = input;
	this->camera = camera;	

	//�J�����̏����ʒu�A�����_
	camera->SetTarget({ 0,0,20 });
	camera->SetEye({ 0,-0,-20 });

	//�X�v���C�g���ʃe�N�X�`���ǂݍ���
	Sprite::LoadTexture(0, L"Resources/texture.png");
	Sprite::LoadTexture(1, L"Resources/torisetu.png");	

	//�X�v���C�g�̐���
	spriteBG = Sprite::Create(0, {0.0f,0.0f});


	//���f���̓ǂݍ���
	fbxmodel = FbxLoader::GetInstance()->LoadModelFromFile("cube");
	//3D�I�u�W�F�N�g�̐���
	fbxobject = new FBXObject;
	fbxobject->Initialize();
	fbxobject->SetModel(fbxmodel);
	fbxobject->PlayAnimetion();
	fbxobject->SetPos({ 0,0,+80 });
}

void GameScene::Update()
{
	//�J�����̈ړ�
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
	//�J�����̍X�V
	camera->MoveVector(camerapos);
	camera->Update();

	//3D�I�u�W�F�N�g�X�V
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
