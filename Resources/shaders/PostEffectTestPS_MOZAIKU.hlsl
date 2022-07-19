#include "PostEffectTest_NONE.hlsli"

Texture2D<float4> tex0 : register(t0);   // 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0);        // 0番スロットに設定されたサンプラー


float4 main(VSOutput input) : SV_TARGET
{
    float4 colortex0 = tex0.Sample(smp, input.uv);
    float density = 100;
    float4 col = tex0.Sample(smp,floor(input.uv * density) / density);
    return col;
}
