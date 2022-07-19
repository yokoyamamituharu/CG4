#pragma region include�֘A��WindowProc

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

//����
#define DIRECTINPUT_VERSION 0x0800		//DirectInput�̃o�[�W�����w��
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
#include"OBJobject.h"

using namespace Microsoft::WRL;

#include"fbxsdk.h"
#include"FbxLoader.h"
#include "Camera.h"
#include "FBXObject.h"
#include "Sprite.h"
#include "PostEffect.h"

//�����̂̓����蔻��
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

	//x��y�͂��ꂼ�ꒆ�S���W�Ƃ��Čv�Z����
	return disX1 > disX2 && disX4 > disX3 && disY1 > disY2 && disY4 > disY3 && disZ1 > disZ2 && disZ4 > disZ3;
}


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	HRESULT result;
	FbxManager* fbxManager = FbxManager::Create();
#pragma region ����������

	// DirectX�����������@��������
	//�|�C���^�u����
	WinApp* winApp = nullptr;
	winApp = new WinApp();
	winApp->Initialize();

	DirectXCommon* dxCommon = nullptr;
	//DirectX�̏�����
	dxCommon = new DirectXCommon();
	dxCommon->Initialize(winApp);

	//����
	Input* input = nullptr;
	input = Input::GetInstance();
	input->Initialize(winApp->GetHInstance(), winApp->GetHwnd());

	Camera* camera = nullptr;
	camera = new Camera(winApp->window_width, winApp->window_height);

	//�X�v���C�g�̐ÓI������
	Sprite::StaticInitialize(dxCommon->GetDev(), WinApp::window_width, WinApp::window_height);

	//���f���̐ÓI������
	Model::StaticInitialize(dxCommon->GetDev());
	//3D�I�u�W�F�N�g�ÓI������
	OBJobject::StaticInitialize(dxCommon->GetDev(), WinApp::window_width, WinApp::window_height, camera);

	//FBX
	FBXObject::SetDevice(dxCommon->GetDev());
	FBXObject::SetCamera(camera);
	FBXObject::CreateGraphicsPipline();

	//�X�v���C�g���ʃe�N�X�`���ǂݍ���
	Sprite::LoadTexture(0, L"Resources/texture.jpg");
	Sprite::LoadTexture(1, L"Resources/torisetu.png");
	Sprite::LoadTexture(2, L"Resources/haiiro.png");
	Sprite::LoadTexture(3, L"Resources/kati.png");
	Sprite::LoadTexture(10, L"Resources/0.png");
	Sprite::LoadTexture(11, L"Resources/1.png");
	Sprite::LoadTexture(12, L"Resources/2.png");
	Sprite::LoadTexture(13, L"Resources/3.png");
	Sprite::LoadTexture(14, L"Resources/4.png");
	Sprite::LoadTexture(15, L"Resources/5.png");
	Sprite::LoadTexture(16, L"Resources/6.png");
	Sprite::LoadTexture(17, L"Resources/7.png");
	Sprite::LoadTexture(18, L"Resources/8.png");
	Sprite::LoadTexture(15, L"Resources/sora.png");

	//�|�X�g�G�t�F�N�g�p�e�N�X�`���̓ǂݍ���
	//Sprite::LoadTexture(100, L"Resources/white1x1.png");
	//�|�X�g�G�t�F�N�g�̏�����
	PostEffect* postEffect = nullptr;
	postEffect = new PostEffect();
	postEffect->Initialize();

	//FBX�̏�����
	FbxLoader::GetInstance()->Initialize(dxCommon->GetDev());

	// DirectX�����������@�����܂�

#pragma endregion


	//�X�v���C�g
	Sprite* sprite1 = Sprite::Create(1, { 1,1 });

	//�X�v���C�g
	Sprite* sprite2 = Sprite::Create(2, { 100,100 });
	Sprite* sprite3 = Sprite::Create(11, { 100,200 });
	Sprite* sprite4 = Sprite::Create(17, { 100,300 });
	Sprite* sprite5 = Sprite::Create(1, { 0,0 });



	//�ϐ��錾
	/*���f������*/
	Model* playermodel = Model::Create();
	playermodel->CreateFromOBJ("player");
	Model* enemymodel = Model::Create();
	enemymodel->CreateFromOBJ("enemy");
	Model* titleModel = Model::Create();
	titleModel->CreateFromOBJ("title");
	Model* endModel = Model::Create();
	endModel->CreateFromOBJ("end");

	Model* backModel1 = Model::Create();
	backModel1->CreateFromOBJ("backb");

	Model* ropeModel = Model::Create();
	ropeModel->CreateFromOBJ("rope");




	/*3d�I�u�W�F�N�g����*/
	//�v���C���[
	OBJobject* playerobj = OBJobject::Create();
	playerobj->SetModel(playermodel);
	playerobj->SetScale({ 1.0f,1.0f,1.0f });
	playerobj->SetPosition({ 0.0f,0.0f,0.0f });
	playerobj->SetRotation({ 0.0f,150.0f,0.0f });




	//�I�u�W�F�N�g�̏����ݒ�̔��f
	playerobj->Update();
	//���f�������w�肵�ăt�@�C���ǂݍ���
	FbxLoader::GetInstance()->LoadModelFromFile("boneTest");

	FbxModel* fbxmodel = nullptr;
	FBXObject* fbxobject = nullptr;

	fbxmodel = FbxLoader::GetInstance()->LoadModelFromFile("cube");
	fbxobject = new FBXObject;
	fbxobject->Initialize();
	fbxobject->SetModel(fbxmodel);
	fbxobject->PlayAnimetion();
	fbxobject->SetPos({ 0,0,+80 });

	camera->SetTarget({ 0,0,20 });
	camera->SetEye({ 0,-0,-20 });

	while (true)  // �Q�[�����[�v
	{
		// �u���b�N���̓y�[�W�E�����Q��
		// ���b�Z�[�W������H
		if (winApp->ProcessMessage())
		{
			break;
		}

		//DirectX���t���[�������@��������
		//-----���͂̍X�V����
		input->Update();


#pragma region �X�V����		
		fbxobject->Update();

#pragma endregion

		//�v���C���[�̍X�V����
		XMFLOAT3 playerpos = playerobj->GetPosition();
		XMFLOAT3 camerapos = { 0,0,0 };
		float num = 0.8;
		if (input->PushKey(DIK_W))
		{
			playerpos.y += 1.0f;
			camerapos.y += num;
		}
		if (input->PushKey(DIK_S))
		{
			playerpos.y -= 1.0f;
			camerapos.y -= num;
		}
		if (input->PushKey(DIK_A))
		{
			playerpos.x -= 1.0f;
			camerapos.x -= num;
		}
		if (input->PushKey(DIK_D))
		{
			playerpos.x += 1.0f;
			camerapos.x += num;
		}

		if (input->PushKey(DIK_Q))
		{
			playerpos.x -= 1.0f;
			camerapos.z -= num;
		}
		if (input->PushKey(DIK_E))
		{
			playerpos.x += 1.0f;
			camerapos.z += num;
		}
		//playerobj->SetPosition(playerpos);
		camera->MoveVector(camerapos);

		camera->Update();

		//3D�I�u�W�F�N�g�X�V
		playerobj->Update();

		//DirectX���t���[�������@�����܂�

		Sprite::PreDraw(dxCommon->GetCmdList());
		sprite1->Draw();
		Sprite::PostDraw();


		postEffect->PreDrawScene(dxCommon->GetCmdList());

		Sprite::PreDraw(dxCommon->GetCmdList());
		//sprite2->Draw();
		Sprite::PostDraw();
		//3D�I�u�W�F�N�g�`��O����
		OBJobject::PreDraw(dxCommon->GetCmdList());
		//playerobj->Draw();
		fbxobject->Draw(dxCommon->GetCmdList());
		//3D�I�u�W�F�N�g�`��㏈��
		OBJobject::PostDraw();

		postEffect->PosDrawScene(dxCommon->GetCmdList());



		//�`�揈��
		dxCommon->PreDraw();

		postEffect->Draw(dxCommon->GetCmdList());

		Sprite::PreDraw(dxCommon->GetCmdList());
		sprite5->Draw();
		Sprite::PostDraw();
		dxCommon->PostDraw();
	}


	winApp->Finalize();

	delete winApp;
	winApp = nullptr;
	//DirectX���
	delete dxCommon;
	//���͂̉��
	delete input;
	//3D�I�u�W�F�N�g���
	delete playerobj;

	//FBX�̉������
	FbxLoader::GetInstance()->Finalize();
	return 0;
}