#include "PostEffectTest_GAUSSIAN.hlsli"

Texture2D<float4> tex0 : register(t0); //0番スロットに設定されたテクスチャ
Texture2D<float4> tex1 : register(t1); //1番スロットに設定されたテクスチャ

SamplerState smp : register(s0);

float Gaussian(float2 drawUV, float2 pickUV, float sigma)
{
    float d = distance(drawUV, pickUV);
    return exp(-(d * d) / (2 * sigma * sigma));
}

float4 main(VSOutput input) : SV_TARGET
{
    float4 colortex0 = tex0.Sample(smp, input.uv);
    float4 colortex1 = tex1.Sample(smp, input.uv);

    float4 color = colortex0;
    if (fmod(input.uv.y, 0.1f) < 0.05f)
    {
        color = colortex1;
    }
    //return float4(colortex0.rgb, 1);
    float ShiftWidth = 0.005;
    float4 col = float4(0, 0, 0, 1);
    col += tex0.Sample(smp, input.uv + float2(-ShiftWidth, -ShiftWidth));
    col += tex0.Sample(smp, input.uv + float2(0, -ShiftWidth));
    col += tex0.Sample(smp, input.uv + float2(ShiftWidth, -ShiftWidth));
    col += tex0.Sample(smp, input.uv + float2(-ShiftWidth, 0));
    col += tex0.Sample(smp, input.uv + float2(0, 0));
    col += tex0.Sample(smp, input.uv + float2(-ShiftWidth, 0));
    col += tex0.Sample(smp, input.uv + float2(-ShiftWidth, ShiftWidth));
    col += tex0.Sample(smp, input.uv + float2(ShiftWidth, ShiftWidth));
    col.rgb = col.rgb / 9;
    //return col;
    
    
    
    
    float totalweight = 0, sigma = 0.005, stepwidth = 0.001;
    float4 cool = float4(0, 0, 0, 0);
    
    cool = tex0.Sample(smp, input.uv);
    for (float py = -sigma * 2; py <= sigma * 2; py += stepwidth)
    {
        for (float px = -sigma * 2; px <= sigma * 2; px += stepwidth)
        {
            float2 pickUV = input.uv + float2(px, py);
            float weight = Gaussian(input.uv, pickUV, sigma);
            cool += tex0.Sample(smp, pickUV) * weight;
            totalweight += weight;
        }    
    }
    cool.rgb = cool.rgb / totalweight;
    return float4(cool.rgb, 1);
    
    
}