#include "PostEffectTest_NONE.hlsli"

Texture2D<float4> tex : register(t0);   // 0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp : register(s0);        // 0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

SamplerState smp : register(s0);


float4 main(VSOutput input) : SV_TARGET
{
    float4 colortex0 = tex0.Sample(smp, input.uv);
    float density = 100;
    float4 col = tex0.Sample(smp,floor(input.uv * density) / density);
    return col;
}
