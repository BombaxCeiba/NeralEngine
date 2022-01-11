#pragma once
//***************************************************************************************
//SoftRender.cpp by Dusk_NM02 (c) 2021 All Rights Reserved.
//***************************************************************************************
#include <memory>
#include <cmath>
#include <algorithm>
#include <Windows.h>
#include <Gdiplus.h>

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

#include "ObjLoader.hpp"
#include "../WindowFramework/include/Window.h"

namespace
{
    constexpr inline Gdiplus::PixelFormat g_softrender_bitmap_pixel_format = PixelFormat24bppRGB;
};

class SoftRender : public RenderBase
{
public:
    class Camera;
    ceiba::EventFunctionGuard<decltype(Window::on_rendering_)> on_render_event_guard_;

private:
    using ObjContentType = ObjContent<16>;
    //TODO:应当含有贴图的指针
    using SoftShaderType = std::function<Vector3fAlignasDefault(const ObjContentType::Vertex&, const MtlContent&, const Vector4fAlignas16&, const Vector3fAlignas16&)>;
    static Vector3fAlignasDefault DefaultShader(const ObjContentType::Vertex& vertex, const MtlContent& mtl, const Vector4fAlignas16& camera_position, const Vector3fAlignas16& view_position);
    SoftShaderType custom_shader_;
    bool render_target_flag;
    std::unique_ptr<Gdiplus::Bitmap> up_render_picture_;
    const ObjContentType* p_obj_content_;
    Matrix4fAlignas16 model_matrix_;
    Matrix4fAlignas16 view_matrix_;
    Matrix4fAlignas16 projection_matrix_;
    ceiba::EventFunctionGuard<decltype(Window::on_size_changed_)> on_size_changed_event_guard_;
    //EventFunctionGuard<decltype(Window::o)>
    Window& render_target_window_;
    Matrix4fAlignas16 PrecomputeMVPMatrix();
    auto MVPAndViewportTransform(const std::array<Vector4fAlignas16, 3>& positions, const Matrix4fAlignas16& mvp_matrix)
        -> std::tuple<std::array<Vector4fAlignas16, 3>, std::array<Vector3fAlignas16, 3>>;
    void SetModelMatrix();
    void SetViewMatrix(const SoftRender::Camera& camera);
    void SetProjectionMatrix(const float z_near, const float z_far, const std::int32_t width, const std::int32_t height, const float fov_y_degree);
    static Vector4fAlignas16 GetBarycentricArgs(const Vector4fAlignas16& target_position, const std::array<Vector4fAlignas16, 3>& triangle_positions);
    static Vector4fAlignas16 Interpolate2D(const Vector4fAlignas16& barycentric_args, const float weight, const std::array<Vector4fAlignas16, 3>& properties);
    static Vector2fAlignas16 Interpolate2D(const Vector4fAlignas16& barycentric_args, const float weight, const std::array<Vector2fAlignas16, 3>& properties);
    static Vector4fAlignas16 Interpolate3D(const Vector4fAlignas16& barycentric_args, const float weight, const std::array<Vector4fAlignas16, 3>& properties);
    static Vector3fAlignas16 Interpolate3D(const Vector4fAlignas16& barycentric_args, const float weight, const std::array<Vector3fAlignas16, 3>& properties);
    static bool IsPointInTriangle(const Vector4fAlignas16& barycentric_args);
    static float InterpolateZ(const Vector4fAlignas16& barycentric_args, const std::array<Vector4fAlignas16, 3>& triangle_positions);
    static ObjContentType::Vertex GetVertex(const Vector4fAlignas16& barycentric_args, const float x, const float y, const float z, const ObjContentType::TriangleVector::value_type& triangle);
    //stored by BGR
    struct ColorRGB24
    {
        ColorRGB24() = default;
        ColorRGB24(const Vector3fAlignasDefault& float_color);
        std::uint8_t LimitNumber(std::uint16_t number);
        std::uint8_t b_;
        std::uint8_t g_;
        std::uint8_t r_;
    };
    ColorRGB24 background_color_;
    struct Light
    {
        Vector3fAlignas16 position_;
        Vector3fAlignas16 intensity_;
    };

    void Set24bppRGBPixelColorUnsafe(void* scan0, size_t pixel_index, const ColorRGB24& color);
    auto Get24bppRGBPixelColorUnsafe(void* scan0, size_t x, size_t y) -> Vector3fAlignasDefault;
    class DepthBuffer
    {
    public:
        DepthBuffer(Window& target_window, size_t width, size_t height);
        ~DepthBuffer() = default;
        void ResizeDepthBuffer(size_t width, size_t height); //此工作交由SoftRender类一并处理，省去一次函数调用开销
        void ResetDepthBuffer();
        float GetDepth(size_t x, size_t y, size_t sample_index) const;
        size_t GetWidth() const;
        size_t GetHeight() const;
        void SetDepth(size_t x, size_t y, size_t sample_index, float new_depth);
        bool TryToSetDepth(size_t x, size_t y, size_t sample_index, float new_depth);

    private:
        static const float default_depth_;
        size_t width_;
        size_t height_;
        ceiba::EventFunctionGuard<decltype(Window::on_size_changed_)> size_change_event_guard_;
        std::vector<float> depth_buffer_;
    } depth_buffer_;

public:
    SoftRender(Window& window, const Vector4fAlignas16& position, const Vector4fAlignas16& gaze_direction, const Vector4fAlignas16& up_direction)
        : render_target_window_{window}, render_target_flag{false},
          camera_{position, gaze_direction, up_direction},
          model_matrix_{Matrix4fAlignas16::IdentityMatrix()},
          view_matrix_{Matrix4fAlignas16::IdentityMatrix()},
          projection_matrix_{Matrix4fAlignas16::IdentityMatrix()},
          depth_buffer_{window, static_cast<size_t>(window.GetWidth()), static_cast<size_t>(window.GetHeight())},
          on_size_changed_event_guard_(window.on_size_changed_, std::bind_front(&SoftRender::OnWindowSizeChange, this))
    {
        up_render_picture_ = std::make_unique<Gdiplus::Bitmap>(window.GetWidth(), window.GetHeight(), g_softrender_bitmap_pixel_format);
        background_color_.r_ = 1;
        background_color_.g_ = 1;
        background_color_.b_ = 1;
    }
    ~SoftRender() = default;
    void SetObjContent(const ObjContentType& p_obj_content);
    //省略了Model变换
    ceiba::EventState Rendering(const ceiba::RenderEventArgs& event_args);
    ceiba::EventState OnWindowSizeChange(const ceiba::SizeChangedEventArgs& event_args);
    //第0个参数Position，第1个参数GazeDirection，第2个参数UpDirection
    class Camera
    {
    public:
        Camera(const Vector4fAlignas16& position, const Vector4fAlignas16& gaze_direction, const Vector4fAlignas16& up_direction);
        ~Camera() = default;
        const Vector4fAlignas16& GetPosition() const;
        const Vector4fAlignas16& GetGazePosition() const;
        const Vector4fAlignas16& GetUpDirection() const;
        void SetPosition(const Vector4fAlignas16& position);
        void SetGazePosition(const Vector4fAlignas16& gaze_direction);
        void SetUpDirection(const Vector4fAlignas16& up_direction);

    private:
        enum class DataType : size_t
        {
            Position = 0,
            GazePostion = 1,
            UpDirection = 2
        };
        std::array<Vector4fAlignas16, 3> camera_;
    } camera_;
};