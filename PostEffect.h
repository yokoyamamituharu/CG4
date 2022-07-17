#pragma once
#include "Sprite.h"
class PostEffect :
    public Sprite
{
public:
    PostEffect();
    
    //初期化
    void Initialize();

    void Draw(ID3D12GraphicsCommandList* cmdList);

private://メンバ変数
    ComPtr<ID3D12Resource>texBuff;
    //SRV用デスクリプタヒープ
    ComPtr<ID3D12DescriptorHeap>descHeapSRV;

};

