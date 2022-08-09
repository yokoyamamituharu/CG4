#include "Player.h"

void Player::Initialize(Input* input, InputMouse* mouse)
{
	assert(input);
	assert(mouse);
	this->input = input;
	this->mouse = mouse;
	//プレイヤー用モデルを読み込み
	playermodel = new Model();
	playermodel->CreateFromOBJ("player");
	playerSpheremodel = new Model();
	playerSpheremodel->CreateFromOBJ("playerSphere");

	//オブジェクトの作成
	object = OBJobject::Create();
	object->SetModel(playermodel);
}

void Player::Update()
{
	if (mouse->PushMouse(MouseDIK::M_LEFT)) {
		isSphere = true;
	}
	else {
		isSphere = false;
	}

	if (isSphere) {
		//モデルを変える
		object->SetModel(playerSpheremodel);
		//マウスの下への移動量を保存（下に下げれば＋、上にあげれば―（0以下にはならない））
		rollingSpeed += mouse->MoveMouseVector('y') / 30;
		if (rollingSpeed < 0) {
			rollingSpeed = 0;
		}
		//マウスの移動量をプレイヤーの回転速度にもする
		spiralSpeed.z = rollingSpeed;
		//マウスを離したとき、移動量があったらプレイヤーを直進させる

		//その時のプレイヤーの回転速度はプレイヤーの移動速度に依存

	}
	else if (rollingSpeed > 0){
		MoveVector({ 0,0,rollingSpeed });
		rollingSpeed -= 1;
		if (rollingSpeed < 0) {
			rollingSpeed = 0;
		}
	}
	else{
		rollingSpeed = 0.0f;
		spiralSpeed.z = 0;
		object->SetRotation({ object->GetRotation().x, object->GetRotation().y, 0.0f, });
		object->SetModel(playermodel);
	}
	SpiralVector(spiralSpeed);

	//オブジェクトのアップデート
	object->Update();
}
