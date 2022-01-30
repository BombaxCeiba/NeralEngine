/***
 * @Author: Dusk
 * @Date: 2021-12-01 22:07:36
 * @FilePath: \NRender\NRender\D3D12Render\D3D12Render.cpp
 * @Copyright (c) 2021 Dusk. All rights reserved.
 */
#if defined(DEBUG) || defined(_DEBUG)
#include <d3d12sdklayers.h>
#endif
#include "D3D12Render.h"
#include "Utils.h"
#include "Win32APIHelper.h"

using namespace Microsoft::WRL;
using namespace dusk::tools;

void D3D12Render::PrintAdapters()
{
    ComPtr<IDXGIAdapter> adapter{};
    DXGI_ADAPTER_DESC adapter_desc;

    constexpr static int ADAPTER_WCHAR_DESC_SIZE = dusk::GetLength(&DXGI_ADAPTER_DESC::Description);
    //预分配内存
    dusk::DynamicBuffer<char> buffer_for_conversion{ADAPTER_WCHAR_DESC_SIZE};

    for (UINT i = 0; dxgi_factory_->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i)
    {
        adapter->GetDesc(&adapter_desc);

        ::WideCharToMultiByte(
            CP_ACP,
            dusk::EMPTY_FLAG,
            adapter_desc.Description,
            ADAPTER_WCHAR_DESC_SIZE,
            buffer_for_conversion.ToWriteAllByByteSize(
                dusk::EvaluateWideCharToMultiByteSize(CP_ACP, adapter_desc.Description, dusk::EMPTY_FLAG, ADAPTER_WCHAR_DESC_SIZE)),
            ADAPTER_WCHAR_DESC_SIZE, //使用预分配的缓冲区大小
            nullptr,
            nullptr);
        dusk::LogDebug<dusk::Outputer::Console>("Adapter:", buffer_for_conversion.ToRead());
        buffer_for_conversion.ResetMemory();

        PrintAdapterOutputs(std::move(adapter));
    }
}

void D3D12Render::PrintAdapterOutputs(Microsoft::WRL::ComPtr<IDXGIAdapter> adapter)
{
    ComPtr<IDXGIOutput> output{};
    DXGI_OUTPUT_DESC output_desc;

    constexpr static int OUTPUT_DEVICE_NAME_WCHAR_SIZE = dusk::GetLength(&DXGI_OUTPUT_DESC::DeviceName);
    dusk::DynamicBuffer<char> buffer_for_conversion{OUTPUT_DEVICE_NAME_WCHAR_SIZE};
    for (UINT i = 0; adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND; ++i)
    {
        output->GetDesc(&output_desc);

        ::WideCharToMultiByte(
            CP_ACP,
            dusk::EMPTY_FLAG,
            output_desc.DeviceName,
            OUTPUT_DEVICE_NAME_WCHAR_SIZE,
            buffer_for_conversion.ToWriteAllByByteSize(
                dusk::EvaluateWideCharToMultiByteSize(CP_ACP, output_desc.DeviceName, dusk::EMPTY_FLAG, OUTPUT_DEVICE_NAME_WCHAR_SIZE)),
            OUTPUT_DEVICE_NAME_WCHAR_SIZE, //使用预分配的缓冲区大小
            nullptr,
            nullptr);
        dusk::LogDebug<dusk::Outputer::Console>("  Outputer:", buffer_for_conversion.ToRead());
        buffer_for_conversion.ResetMemory();

        PrintOutputDisplayModes(std::move(output), back_buffer_format_);
    }
}

void D3D12Render::ResetCommandList()
{
    ThrowIfFailed(command_allocator_->Reset());
    ThrowIfFailed(command_list_->Reset(command_allocator_.Get(), pipeline_state_.Get()));
}

void D3D12Render::PrintOutputDisplayModes(Microsoft::WRL::ComPtr<IDXGIOutput> output, DXGI_FORMAT format)
{
    UINT display_mode_count = 0;
    UINT flags = 0;
    std::string retract{"    "};

    // 输入空指针以取得元素数量
    ThrowIfFailed(output->GetDisplayModeList(format, flags, &display_mode_count, nullptr));

    std::vector<DXGI_MODE_DESC> mode_descs(display_mode_count);
    ThrowIfFailed(output->GetDisplayModeList(format, flags, &display_mode_count, &mode_descs.front()));

    for (const auto& desc : mode_descs)
    {
        auto rate = static_cast<double>(desc.RefreshRate.Numerator) / static_cast<double>(desc.RefreshRate.Denominator);
        dusk::LogDebug<dusk::Outputer::Console>(
            retract, "Width = ", std::to_string(desc.Width),
            retract, "Height = ", std::to_string(desc.Height),
            retract, "Refresh rate = ", std::to_string(rate));
    }
}

void D3D12Render::LoadAssets()
{
}

D3D12Render::D3D12Render(Window& window)
    : back_buffer_format_{DXGI_FORMAT_R8G8B8A8_UNORM}, use_msaa4x_{true}, msaa4x_quality_level_{0},
      on_resize_event_guard_{window.on_size_changed_, [this](const ceiba::SizeChangedEventArgs& e) -> ceiba::EventState
                             { return this->OnResize(e); }}
{
    Initialize(window);
}

void D3D12Render::Initialize(Window& target_window)
{
#if defined(DEBUG) || defined(_DEBUG)
    {
        //启动调试层
        ComPtr<ID3D12Debug> debugController;
        ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
        debugController->EnableDebugLayer();
    }
#endif
    //初始化dxgifactory
    ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&dxgi_factory_)));
    //尝试创建硬件设备
    try
    {
        ThrowIfFailed(D3D12CreateDevice(
            nullptr,                //默认适配器
            D3D_FEATURE_LEVEL_11_0, //特性等级
            IID_PPV_ARGS(&d3d_device_)));
    }
    catch (const DXException& e) //创建设备出错，回退到软件设备
    {
        ComPtr<IDXGIAdapter> warp_adapter;
        ThrowIfFailed(dxgi_factory_->EnumWarpAdapter(IID_PPV_ARGS(&warp_adapter)));
        ThrowIfFailed(D3D12CreateDevice(warp_adapter.Get(),
                                        D3D_FEATURE_LEVEL_11_0,
                                        IID_PPV_ARGS(&d3d_device_)));
    }
    //创建围栏
    ThrowIfFailed(d3d_device_->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_)));
    rtv_descriptor_size_ = d3d_device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    dsv_descriptor_size_ = d3d_device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    cbv_srv_uav_descriptor_size_ = d3d_device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    //检查并启用MSAA 4X
    D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msaa_level{
        .Format = back_buffer_format_,
        .SampleCount = 4,
        .Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE,
        .NumQualityLevels = 0};
    ThrowIfFailed(d3d_device_->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msaa_level, sizeof(msaa_level)));
    if (msaa_level.NumQualityLevels <= 0)
        [[unlikely]]
    {
        std::runtime_error{"system does not support MSAA 4X"};
    }
    msaa4x_quality_level_ = msaa_level.NumQualityLevels;
#if defined(DEBUG) || defined(_DEBUG)
    PrintAdapters();
#endif
    //初始化命令列表
    D3D12_COMMAND_QUEUE_DESC command_queue_desc{};
    command_queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    command_queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    ThrowIfFailed(d3d_device_->CreateCommandQueue(&command_queue_desc, IID_PPV_ARGS(&command_queue_)));
    ThrowIfFailed(d3d_device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&command_allocator_)));
    ThrowIfFailed(d3d_device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
                                                 command_allocator_.Get(),
                                                 nullptr,
                                                 IID_PPV_ARGS(&command_list_)));
    command_list_->Close();
    //初始化交换链
    DXGI_SWAP_CHAIN_DESC swap_chain_desc = {};
    swap_chain_desc.BufferCount = 2;
    swap_chain_desc.BufferDesc.Width = target_window.GetWidth();
    swap_chain_desc.BufferDesc.Height = target_window.GetHeight();
    swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swap_chain_desc.OutputWindow = target_window.GetHwnd();
    swap_chain_desc.SampleDesc.Count = 1;
    swap_chain_desc.Windowed = TRUE;
    ComPtr<IDXGISwapChain> swapChain; //使用ComPtr防止抛出异常后内存泄露
    ThrowIfFailed(dxgi_factory_->CreateSwapChain(
        command_queue_.Get(),
        &swap_chain_desc,
        &swapChain));
    ThrowIfFailed(swapChain.As(&swap_chain_));
}

void D3D12Render::HandleIfDeviceLost(const HRESULT hr)
{
    if (hr == DXGI_ERROR_DEVICE_RESET || hr == DXGI_ERROR_DEVICE_REMOVED)
    {
        // If the device was removed for any reason, a new device and swap chain will need to be created.
        OnDeviceLost(hr);

        // Everything is set up now. Do not continue execution of this method. HandleDeviceLost will reenter this method
        // and correctly set up the new device.
    }
    else
    {
        //Throw other error
        ThrowIfFailed(hr);
    }
}

void D3D12Render::OnDeviceLost(const HRESULT error_type)
{
}
//现在，对当前帧重复使用命令列表。 将视区重新附加到命令列表（必须在每次重置命令列表之后、执行命令之前完成），指示资源将用作渲染器目标，记录命令，然后指示在执行完命令列表后，将使用渲染器目标来呈现信息。

ceiba::EventState D3D12Render::OnResize(const ceiba::SizeChangedEventArgs& event_args)
{
    // If the swap chain already exists, resize it.
    HRESULT hr = swap_chain_->ResizeBuffers(
        2, // Double-buffered swap chain.
        static_cast<UINT>(event_args.new_width_),
        static_cast<UINT>(event_args.new_height_),
        DXGI_FORMAT_B8G8R8A8_UNORM,
        0);
    HandleIfDeviceLost(hr);
    return ceiba::EventState::Continue;
}

ceiba::EventState D3D12Render::OnRending(const ceiba::RenderEventArgs& event_args)
{
    HRESULT present_hr = swap_chain_->Present(1, 0); //呈现帧
    //command_list_->ClearRenderTargetView();
    HandleIfDeviceLost(present_hr);
    return ceiba::EventState::Continue;
}