#include "SceneManager.h"

void SceneManager::Initialize(DirectXCommon* dxCommon, Input* input, InputMouse* mouse, Camera* camera)
{
	gameScene = new GameScene;
	gameScene->Initialize(dxCommon, input, mouse, camera);
	batlleScene = new BatlleScene;
	batlleScene->Initialize(dxCommon, input, mouse, camera, gameScene);

	sceneNo = GAMESCENE;
}

void SceneManager::Update()
{
	if (Input::GetInstance()->PushKey(DIK_1)) {
		sceneNo = GAMESCENE;
	}
	else if (Input::GetInstance()->PushKey(DIK_2)) {
		sceneNo = BATTLESCENE;
	}

	if (sceneNo == GAMESCENE) {
		gameScene->Update();
	}
	else {
		batlleScene->Update();
	}
}

void SceneManager::Draw()
{
	if (sceneNo ==GAMESCENE) {
		gameScene->Draw();
	}
	else {
		batlleScene->Draw();
	}

}
