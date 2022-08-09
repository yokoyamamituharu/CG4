#pragma once
#include "OBJObject.h"
#include "Input.h"
#include "InputMouse.h"

class Player
{
public:
	void Initialize(Input* input, InputMouse* mouse);
	void Update();
	void MoveVector(DirectX::XMFLOAT3 vec)
	{
		object->SetPosition({
		 object->GetPosition().x + vec.x,
		 object->GetPosition().y + vec.y,
		 object->GetPosition().z + vec.z });
	}

	void SpiralVector(DirectX::XMFLOAT3 vec)
	{
		object->SetRotation({
		 object->GetRotation().x + vec.x,
		 object->GetRotation().y + vec.y,
		 object->GetRotation().z + vec.z });
	}

public:
	void SetInput(Input* input) { this->input = input; }
	void SetMouse(InputMouse* mouse) { this->mouse = mouse; }

public:
	OBJobject* object = nullptr;

private:
	Input* input;
	InputMouse* mouse;
	Model* playermodel = nullptr;
	Model* playerSpheremodel = nullptr;

	bool isSphere = false;
	DirectX::XMFLOAT3 spiralSpeed = {};
	float rollingSpeed = 0.0f;
};

