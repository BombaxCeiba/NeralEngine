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
# undef min
#endif

#ifdef max
# undef max
#endif

#include "ObjLoader.hpp"
#include "Window.h"

#pragma comment(lib,"Gdiplus.lib")

constexpr inline Gdiplus::PixelFormat g_softrender_bitmap_pixel_format = PixelFormat24bppRGB;

class SoftRender :public RenderBase
{
public:
    class Camera;
private:
    using ObjContentType = ObjContent<16>;
    //TODO:应当含有贴图的指针
    using SoftShaderType = std::function<Vector3fAlignasDefault(const ObjContentType::Vertex& vertex, const MtlContent& mtl, const Vector4fAlignas16& camera_position)>;
    static Vector3fAlignasDefault DefaultShader(const ObjContentType::Vertex& vertex, const MtlContent& mtl, const Vector4fAlignas16& camera_position);
    SoftShaderType custom_shader_;
    bool render_target_flag;
    std::unique_ptr<Gdiplus::Bitmap> up_render_picture_;
    const ObjContentType* p_obj_content_;
    Matrix4fAlignas16 model_matrix_;
    Matrix4fAlignas16 view_matrix_;
    Matrix4fAlignas16 projection_matrix_;
    int32_t render_function_index_;
    int32_t size_change_function_index;
    Window& render_target_window_;
    Matrix4fAlignas16 PrecomputeMVPMatrix();
    std::array<Vector4fAlignas16, 3> MVPAndViewportTransform(const Vector4fAlignas16 positions[],const Matrix4fAlignas16& mvp_matrix);
    void SetModelMatrix();
    void SetViewMatrix(const SoftRender::Camera& camera);
    void SetProjectionMatrix(const float z_near, const float z_far, const std::int32_t width, const std::int32_t height, const float fov_y_degree);
    static Vector4fAlignas16 GetBarycentricArgs(const Vector4fAlignas16& target_position, const std::array<Vector4fAlignas16, 3>& triangle_positions);
    static Vector4fAlignas16 Interpolate2D(const Vector4fAlignas16& barycentric_args, const float weight, const Vector4fAlignas16 properties[]);
    static Vector2fAlignas16 Interpolate2D(const Vector4fAlignas16& barycentric_args, const float weight, const Vector2fAlignas16 properties[]);
    static Vector4fAlignas16 Interpolate3D(const Vector4fAlignas16& barycentric_args, const float weight, const Vector4fAlignas16 properties[]);
    static Vector3fAlignas16 Interpolate3D(const Vector4fAlignas16& barycentric_args, const float weight, const Vector3fAlignas16 properties[]);
    static bool IsPointsInTriangle2D(const float z_in_barycentric_2D_args);
    static float InterpolateZ(const Vector4fAlignas16& barycentric_args,const std::array<Vector4fAlignas16, 3>& triangle_positions);
    static ObjContentType::Vertex GetVertex(const Vector4fAlignas16& barycentric_args, const float x, const float y, const float z, const ObjContentType::TriangleVector::value_type& triangle);
    struct ColorRGB24
    {
        int8_t r_;
        int8_t g_;
        int8_t b_;
    };
    struct Light {
        Vector4fAlignas16 position_;
        Vector4fAlignas16 intensity_;
    };

    void Set24bppRGBPixelColorUnsafe(void* scan0,size_t pixel_index,const ColorRGB24& color);
    class DepthBuffer {
    public:
        DepthBuffer(Window& target_window, size_t width, size_t height);
        ~DepthBuffer() = default;
        void ResizeDepthBuffer(size_t width, size_t height);//此工作交由SoftRender类一并处理，省去一次函数调用开销
        void ResetDepthBuffer();
        float GetDepth(size_t x, size_t y, size_t sample_index) const;
        size_t GetWidth() const;
        size_t GetHeight() const;
        void SetDepth(size_t x, size_t y, size_t sample_index, float new_depth);
        bool TryToSetDepth(size_t x, size_t y, size_t sample_index, float new_depth);
    private:
        size_t width_;
        size_t height_;
        std::vector<float> depth_buffer_;
    } depth_buffer_;
public:
    template<typename... CameraArgs> requires (std::is_convertible_v<std::remove_const_t<std::remove_reference_t<CameraArgs>>, Vector4fAlignas16> && ...)
    SoftRender(Window& window, const CameraArgs&&... camera_args) :render_target_window_{ window }, render_target_flag{ false },
        camera_{ camera_args... }, model_matrix_{ Matrix4fAlignas16::IdentityMatrix() },
        view_matrix_{ Matrix4fAlignas16::IdentityMatrix() }, projection_matrix_{ Matrix4fAlignas16::IdentityMatrix() },
        depth_buffer_{ window,window.GetWidth(),window.GetHeight() }
    {
        static_assert(sizeof...(camera_args) <= 4);
        up_render_picture_ = std::make_unique<Gdiplus::Bitmap>(window.GetWidth(), window.GetHeight(), g_softrender_bitmap_pixel_format);
        render_function_index_ = window.on_render_.AddFunction(std::bind_front(&SoftRender::Rendering, this));
        size_change_function_index = window.on_size_changed_.AddFunction(std::bind_front(&SoftRender::OnWindowSizeChange, this));
    }
    ~SoftRender();
    void SetObjContent(const ObjContentType& p_obj_content);
    //省略了Model变换
    EventState Rendering(const RenderEventArgs& event_args);
    EventState OnWindowSizeChange(const SizeChangedEventArgs& event_args);
    //第0个参数Position，第1个参数GazeDirection，第2个参数UpDirection
    class Camera
    {
    public:
        template<typename... Args> requires (std::is_convertible_v<Args, Vector4fAlignas16> && ...)
            Camera(Args&&... args) :camera_{ {std::forward<Args>(args)...} } 
        {
            if constexpr (sizeof...(args) >= 1)
            {
                //camera_[0].NormalizeSelf();
            }
            if constexpr (sizeof...(args) >= 2)
            {
                //camera_[1].NormalizeSelf();
                SetCameraUpDirection();
            }
        }
        ~Camera() = default;
        const Vector4fAlignas16& GetPosition() const;
        const Vector4fAlignas16& GetGazeDirection() const;
        const Vector4fAlignas16& GetUpDirection() const;
        void SetCameraPosition(const Vector4fAlignas16& position);
        void SetCameraGazeDirection(const Vector4fAlignas16& gaze_direction);
        void SetCameraUpDirection();
    private:
        std::array<Vector4fAlignas16, 3> camera_;
    };

    Camera camera_;
};