#pragma once
#include "Sprite.h"
class PostEffect :
    public Sprite
{
public:
    PostEffect();
    
    //������
    void Initialize();

    void Draw(ID3D12GraphicsCommandList* cmdList);

private://�����o�ϐ�
    ComPtr<ID3D12Resource>texBuff;
    //SRV�p�f�X�N���v�^�q�[�v
    ComPtr<ID3D12DescriptorHeap>descHeapSRV;

};

