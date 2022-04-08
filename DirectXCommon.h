#pragma once

#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <d3dx12.h>
#include <cstdlib>

#include "WinApp.h"

#include<d3d12.h>
#include<d3dx12.h>
#include<dxgi1_6.h>
#include<wrl.h>

#include<cassert>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

using namespace Microsoft::WRL;

//DirectX汎用
class DirectXCommon
{
private: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public: // メンバ関数
	//初期化
	void Initialize(WinApp *win);

	//描画前処理
	void PreDraw();
	//描画後処理
	void PostDraw();

	//デバイス取得
	ID3D12Device *GetDev() { return device.Get(); }

	//コマンドリスト取得
	ID3D12GraphicsCommandList *GetCmdList() { return commandList.Get(); }

private: // メンバ変数
	bool InitializeDXGIDevice();
	bool InitializeCommand();
	bool CreateSwapChain();
	bool CreateFinalRenderTargets();
	bool CreateDepthBuffer();
	bool CreateFence();

private: // メンバ関数

private:
	
	ComPtr<IDXGIFactory6>dxgiFactory;
	ComPtr <ID3D12Device>device;
	ComPtr<ID3D12GraphicsCommandList>commandList = nullptr;
	ComPtr<ID3D12CommandAllocator>commandAllocator = nullptr;
	ComPtr<ID3D12CommandQueue>commandQueue = nullptr;
	ComPtr<IDXGISwapChain4>swapchain = nullptr;
	std::vector<ComPtr<ID3D12Resource>> backBuffers;
	ComPtr<ID3D12Resource> depthBuffer;
	ComPtr<ID3D12DescriptorHeap>rtvHeaps = nullptr;
	ComPtr<ID3D12DescriptorHeap> dsvHeap = nullptr;
	ComPtr<ID3D12Fence> fence = nullptr;
	UINT64 fenceVal = 0;

	//WindowsAPI
	WinApp *winApp = nullptr;

};