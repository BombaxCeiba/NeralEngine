/***
 * @Author: Dusk
 * @Date: 2021-11-22 13:55:59
 * @FilePath: \NRender\NRender\D3D12Render\D3D12Render.h
 * @Copyright (c) 2021 Dusk. All rights reserved.
 */
#pragma once
#include <vector>
#include <wrl/client.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <sal.h>
#include <DirectXMath.h>
#include "Common/include/DXUtils.h"
#include "Common/include/RenderBase.h"
#include "WindowFramework/include/Window.h"
#include "Common/include/Logger.h"

class D3D12Render : public RenderBase
{
public:
    ceiba::EventFunctionGuard<decltype(Window::on_rendering_)> on_rendering_event_guard_;

private:
    Microsoft::WRL::ComPtr<IDXGIFactory4> dxgi_factory_;
    Microsoft::WRL::ComPtr<ID3D12Device> d3d_device_;
    Microsoft::WRL::ComPtr<ID3D12Fence> fence_;
    UINT rtv_descriptor_size_;
    UINT dsv_descriptor_size_;
    UINT cbv_srv_uav_descriptor_size_;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> command_allocator_;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> command_queue_;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> command_list_;
    Microsoft::WRL::ComPtr<IDXGISwapChain> swap_chain_;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtv_heap_;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsv_heap_;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipeline_state_;
    DXGI_FORMAT back_buffer_format_;
    bool use_msaa4x_;
    UINT msaa4x_quality_level_;
    ceiba::EventFunctionGuard<decltype(Window::on_size_changed_)> on_resize_event_guard_;

public:
    D3D12Render() = delete;
    D3D12Render(Window& window);
    ~D3D12Render() = default;
    void PrintAdapters();
    void PrintAdapterOutputs(Microsoft::WRL::ComPtr<IDXGIAdapter> adapter);
    void PrintOutputDisplayModes(Microsoft::WRL::ComPtr<IDXGIOutput> output, DXGI_FORMAT format);
    ceiba::EventState OnRending(const ceiba::RenderEventArgs&);

private:
    void Initialize(Window& target_window);
    void InitializeSwapChain();
    void CreateSwapChain();

    void HandleIfDeviceLost(const HRESULT hr);
    void OnDeviceLost(const HRESULT error_type);
    void LoadAssets();
    //必须先重置命令列表分配器和命令列表本身，然后才能重复使用它们。 在更高级的方案中，可能有效的做法是每隔几个帧就重置分配器。 内存与执行命令列表后无法立即释放的分配器相关联。 此示例演示如何在完成每一帧后重置分配器。
    void ResetCommandList();
    //对于大多数应用而言，等待帧完成的做法过于低效。
    void WaitForPreviousFrameRending();
    ceiba::EventState OnResize(const ceiba::SizeChangedEventArgs& event_args);
};