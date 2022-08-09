#pragma once
#include "GameScene.h"
#include "BatlleScene.h"

enum SCENE
{
	NONESCENE,
	GAMESCENE,
	BATTLESCENE
};

class SceneManager
{
public:
	void Initialize(DirectXCommon* dxCommon, Input* input, InputMouse* mouse, Camera* camera);
	void Update();
	void Draw();
private:
	GameScene* gameScene = nullptr;
	BatlleScene* batlleScene = nullptr;

	int sceneNo = NONESCENE;
};

