#include "SoftRender.h"

SoftRender::SoftRender(Window& window, const Vector4fAlignas16& position, const Vector4fAlignas16& gaze_direction, const Vector4fAlignas16& up_direction)
    : render_target_window_{window}, render_target_flag{false},
      camera_{position, gaze_direction, up_direction, window},
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

Vector3fAlignasDefault SoftRender::DefaultShader(const ObjContentType::Vertex& vertex, const MtlContent& mtl, const Vector4fAlignas16& camera_position, const Vector3fAlignas16& view_position)
{
    //TODO:传入原始坐标数据进行着色
    //Vector3fAlignasDefault result_color = Vector3fAlignasDefault{ 1.0f,1.0f,1.0f };
    static std::array<Light, 2> light_list{
        Light{{20, 20, 20}, {0, 122, 204}},
        Light{{-20, -20, -20}, {202, 81, 0}}};

    constexpr float p = 150;
    Vector3fAlignasDefault result_color{0};
    for (auto& light : light_list)
    {
        // 光的方向
        Vector3fAlignas16 light_dir = light.position_ - view_position;
        // 衰减因子
        float r = light_dir.Dot(light_dir);
        auto n = vertex.normal;
        Vector3fAlignas16 l_divide_r_squre = light.intensity_ / r;

        // ambient
        Vector3fAlignas16 La = mtl.Ka * light.intensity_;
        // diffuse
        Vector3fAlignas16 Ld = mtl.Kd * l_divide_r_squre;
        Ld *= std::max(0.0f, n.Dot(light_dir.Normalize()));
        // specular
        // 视线方向
        Vector3fAlignas16 view_dir = Vector3fAlignas16::ToVector3(camera_position) - view_position;
        Vector3fAlignas16 h = (light_dir + view_dir).Normalize();
        Vector3fAlignas16 Ls = mtl.Ks * l_divide_r_squre;
        Ls *= std::pow(std::max(0.0f, n.Dot(h)), p);
        result_color += La + Ld + Ls;
    }
    return result_color;
}

Vector3fAlignasDefault SoftRender::DepthVisualizer(const ObjContentType::Vertex& vertex, const MtlContent& mtl, const Vector4fAlignas16& camera_position, const Vector3fAlignas16& view_position)
{
    float depth = vertex.position.z() + 1.0f;
    depth /= 2.0f;
    return {depth, depth, depth};
}

Vector4fAlignas16 SoftRender::GetBarycentricArgs(const Vector4fAlignas16& target_position, const std::array<Vector4fAlignas16, 3>& triangle_positions)
{
    //TODO:应当用shuffle代替set
    //set的值载入后方向为从右往左，load的值按原先的情况排列
    float x = target_position.x();
    float y = target_position.y();
    Vector4fAlignas16 result{};
    result.SetX((x * (triangle_positions[1].y() - triangle_positions[2].y()) + (triangle_positions[2].x() - triangle_positions[1].x()) * y + triangle_positions[1].x() * triangle_positions[2].y() - triangle_positions[2].x() * triangle_positions[1].y()) / (triangle_positions[0].x() * (triangle_positions[1].y() - triangle_positions[2].y()) + (triangle_positions[2].x() - triangle_positions[1].x()) * triangle_positions[0].y() + triangle_positions[1].x() * triangle_positions[2].y() - triangle_positions[2].x() * triangle_positions[1].y()));
    result.SetY((x * (triangle_positions[2].y() - triangle_positions[0].y()) + (triangle_positions[0].x() - triangle_positions[2].x()) * y + triangle_positions[2].x() * triangle_positions[0].y() - triangle_positions[0].x() * triangle_positions[2].y()) / (triangle_positions[1].x() * (triangle_positions[2].y() - triangle_positions[0].y()) + (triangle_positions[0].x() - triangle_positions[2].x()) * triangle_positions[1].y() + triangle_positions[2].x() * triangle_positions[0].y() - triangle_positions[0].x() * triangle_positions[2].y()));
    //result.SetZ((target_position.x() * (triangle_positions[0].y() - triangle_positions[1].y()) + (triangle_positions[1].x() - triangle_positions[0].x()) * target_position.y() + triangle_positions[0].x() * triangle_positions[1].y() - triangle_positions[1].x() * triangle_positions[0].y()) / (triangle_positions[2].x() * (triangle_positions[0].y() - triangle_positions[1].y()) + (triangle_positions[1].x() - triangle_positions[0].x()) * triangle_positions[2].y() + triangle_positions[0].x() * triangle_positions[1].y() - triangle_positions[1].x() * triangle_positions[0].y()));
    result.SetZ(1.0f - result.x() - result.y());
    return result;

    /*__m128 alpha_tmp = _mm_set_ps(target_position.y(), target_position.x(), triangle_positions[0].y(), triangle_positions[0].x());
    alpha_tmp = _mm_sub_ps(alpha_tmp, _mm_set_ps(triangle_positions[1].y(), triangle_positions[1].x(), triangle_positions[1].y(), triangle_positions[1].x()));
    __m128 tmp_to_mul = _mm_sub_ps(
        _mm_set_ps(triangle_positions[2].x(), triangle_positions[2].y(), triangle_positions[0].x(), triangle_positions[0].y()),
        _mm_set_ps(triangle_positions[1].x(), triangle_positions[1].y(), triangle_positions[2].x(), triangle_positions[2].y()));
    __m128 final_var_to_mul = _mm_shuffle_ps(tmp_to_mul, tmp_to_mul, _MM_SHUFFLE(1, 0, 1, 0));
    alpha_tmp = _mm_mul_ps(alpha_tmp, final_var_to_mul);

    __m128 beta_tmp = _mm_set_ps(target_position.y(), target_position.x(), triangle_positions[1].y(), triangle_positions[1].x());
    beta_tmp = _mm_sub_ps(beta_tmp, _mm_set_ps(triangle_positions[2].y(), triangle_positions[2].x(), triangle_positions[2].y(), triangle_positions[2].x()));
    final_var_to_mul = _mm_shuffle_ps(tmp_to_mul, tmp_to_mul, _MM_SHUFFLE(3, 2, 3, 2));
    beta_tmp = _mm_mul_ps(beta_tmp, final_var_to_mul);
    __m128 m128_result = _mm_hsub_ps(alpha_tmp, beta_tmp);

    Vector4fAlignas16 result{};
    _mm_store_ps(result.GetHead(), m128_result);
    result.SetX(result.x() / result.y());
    result.SetY(result.z() / result.w());
    result.SetZ(1.0f - result.x() - result.y());
    result.SetW(0.0f);
    return result;*/
}

Vector4fAlignas16 SoftRender::Interpolate2D(const Vector4fAlignas16& barycentric_args, const float weight, const std::array<Vector4fAlignas16, 3>& properties)
{
    __m128 m128_args = _mm_load_ps(barycentric_args.GetHead());
    __m128 m128_result = _mm_add_ps(
        _mm_mul_ps(_mm_load_ps(properties[0].GetHead()), _mm_shuffle_ps(m128_args, m128_args, _MM_SHUFFLE(0, 0, 0, 0))),
        _mm_add_ps(
            _mm_mul_ps(_mm_load_ps(properties[1].GetHead()), _mm_shuffle_ps(m128_args, m128_args, _MM_SHUFFLE(1, 1, 1, 1))),
            _mm_mul_ps(_mm_load_ps(properties[2].GetHead()), _mm_shuffle_ps(m128_args, m128_args, _MM_SHUFFLE(2, 2, 2, 2)))));
    __m128 weight_4 = _mm_set_ps1(weight);
    m128_result = _mm_div_ps(m128_result, weight_4);
    Vector4fAlignas16 result{};
    _mm_store_ps(result.GetHead(), m128_result);
    return result;
}

Vector2fAlignas16 SoftRender::Interpolate2D(const Vector4fAlignas16& barycentric_args, const float weight, const std::array<Vector2fAlignas16, 3>& properties)
{
    __m128 m128_args = _mm_load_ps(barycentric_args.GetHead());
    __m128 m128_result = _mm_load_ps(properties[0].GetHead());
    m128_result = _mm_mul_ps(m128_result,
                             _mm_shuffle_ps(m128_args, m128_args, _MM_SHUFFLE(1, 1, 0, 0)));
    m128_result = _mm_shuffle_ps(m128_result, m128_result, _MM_SHUFFLE(3, 1, 2, 0));
    m128_result = _mm_hadd_ps(m128_result, _mm_setzero_ps());
    __m128 tmp = _mm_set_ps(properties[2].x(), properties[2].y(), 0.0f, 0.0f);
    tmp = _mm_mul_ps(tmp,
                     _mm_shuffle_ps(m128_args, m128_args, _MM_SHUFFLE(2, 2, 2, 2)));
    m128_result = _mm_add_ps(tmp, m128_result);
    __m128 weight_4 = _mm_set_ps1(weight);
    m128_result = _mm_div_ps(m128_result, weight_4);
    Vector4fAlignas16 result{};
    _mm_store_ps(result.GetHead(), m128_result);
    return Vector2fAlignas16::ToVector2(result);
}

Vector4fAlignas16 SoftRender::Interpolate3D(const Vector4fAlignas16& barycentric_args, const float weight, const std::array<Vector4fAlignas16, 3>& properties)
{
    __m128 m128_args = _mm_load_ps(barycentric_args.GetHead());
    __m128 m128_result = _mm_add_ps(
        _mm_mul_ps(_mm_load_ps(properties[0].GetHead()), _mm_shuffle_ps(m128_args, m128_args, _MM_SHUFFLE(0, 0, 0, 0))),
        _mm_add_ps(
            _mm_mul_ps(_mm_load_ps(properties[1].GetHead()), _mm_shuffle_ps(m128_args, m128_args, _MM_SHUFFLE(1, 1, 1, 1))),
            _mm_mul_ps(_mm_load_ps(properties[2].GetHead()), _mm_shuffle_ps(m128_args, m128_args, _MM_SHUFFLE(2, 2, 2, 2)))));
    __m128 weight_4 = _mm_set_ps1(weight);
    m128_result = _mm_div_ps(m128_result, weight_4);
    Vector4fAlignas16 result{};
    _mm_store_ps(result.GetHead(), m128_result);
    return result;
}

Vector3fAlignas16 SoftRender::Interpolate3D(const Vector4fAlignas16& barycentric_args, const float weight, const std::array<Vector3fAlignas16, 3>& properties)
{
    Vector3fAlignas16 result =
        properties[0] * barycentric_args.x() + properties[1] * barycentric_args.y() + properties[2] * barycentric_args.z();
    return result;
}

bool SoftRender::IsPointInTriangle(const Vector4fAlignas16& barycentric_args)
{
    //Vector4fAlignas16 v0, v1, v2;
    //v0 = triangle_positions[1].Cross3(triangle_positions[0]);
    //v1 = triangle_positions[2].Cross3(triangle_positions[1]);
    //v2 = triangle_positions[0].Cross3(triangle_positions[2]);
    ////point4f.z()==1
    //if (point4f.Dot(v0)*v0.Dot(triangle_positions[2])>0
    //    && point4f.Dot(v1) * v1.Dot(triangle_positions[0]) > 0
    //    && point4f.Dot(v2) * v2.Dot(triangle_positions[1]) > 0)
    //{
    //    return true;
    //}
    //return false;
    return (barycentric_args.x() > 0 && barycentric_args.x() < 1) && (barycentric_args.y() > 0 && barycentric_args.y() < 1) && (barycentric_args.z() > 0 && barycentric_args.z() < 1);
}

float SoftRender::InterpolateZ(const Vector4fAlignas16& barycentric_args, const std::array<Vector4fAlignas16, 3>& triangle_positions)
{
    float z = 1.0f / (barycentric_args.x() / triangle_positions[0].w() + barycentric_args.y() / triangle_positions[1].w() + barycentric_args.z() / triangle_positions[2].w());
    float z_position = barycentric_args.x() * triangle_positions[0].z() / triangle_positions[0].w() + barycentric_args.y() * triangle_positions[1].z() / triangle_positions[1].w() + barycentric_args.z() * triangle_positions[2].z() / triangle_positions[2].w();
    z_position *= z;
    return z_position;
}

SoftRender::ObjContentType::Vertex SoftRender::GetVertex(const Vector4fAlignas16& barycentric_args, const float x, const float y, const float depth, const ObjContentType::TriangleVector::value_type& triangle)
{
    ObjContentType::Vertex result{
        .position = decltype(result.position){x, y, depth},
        .texture_uv = Interpolate2D(barycentric_args, 1.0f, triangle.texture_uv),
        .normal = Interpolate3D(barycentric_args, 1.0f, triangle.normal),
        .color = Interpolate3D(barycentric_args, 1.0f, triangle.color),
    };
    return result;
}

auto SoftRender::MVPAndViewportTransform(const std::array<Vector4fAlignas16, 3>& positions, const Matrix4fAlignas16& mvp_matrix)
    -> std::tuple<std::array<Vector4fAlignas16, 3>, std::array<Vector3fAlignas16, 3>>
{
    //MVP transform
    std::array<Vector4fAlignas16, 3> full_result{
        mvp_matrix * positions[0],
        mvp_matrix * positions[1],
        mvp_matrix * positions[2]};
    //Homogeneous division and viewport transform
    auto width = static_cast<float>(depth_buffer_.GetWidth());
    auto height = static_cast<float>(depth_buffer_.GetHeight());
    for (auto& vec : full_result)
    {
        vec.HomogeneousDivisionSelf();
    }
    std::array<Vector3fAlignas16, 3> partial_result{};
    std::transform(full_result.begin(), full_result.end(), partial_result.begin(),
                   [](auto&& item)
                   { return Vector3fAlignas16::ToVector3(item); });
    //viewport transform
    for (auto& vec : full_result)
    {
        vec.SetX(0.5f * width * (vec.x() + 1.0f));
        vec.SetY(0.5f * height * (vec.y() + 1.0f));
        constexpr static float C = 1.0f;
        //vec.SetW((2.0f * std::log(C * vec.w() + 1.0f) / std::log(C * 100.0f + 1.0f) - 1.0f) * vec.w());
        //vec.SetW(std::log(C * vec.w() + 1.0f) / std::log(C * 100.0f + 1) * vec.w());
    }
    return std::make_tuple(full_result, partial_result);
}

Matrix4fAlignas16 SoftRender::PrecomputeMVPMatrix()
{
    return projection_matrix_ * view_matrix_ * model_matrix_;
}

void SoftRender::SetViewMatrix(const SoftRender::Camera& camera)
{
    Matrix4fAlignas16 t_view_matrix = {Matrix4fAlignas16::IdentityMatrix()};
    __m128 zero = _mm_setzero_ps();
    _mm_stream_ps(t_view_matrix.GetColumnHead<3>(), _mm_sub_ps(
                                                        zero,
                                                        _mm_load_ps(camera.GetPosition().GetHead())));
    t_view_matrix[3][3] = 1.0f;

    auto gaze_tmp = camera.GetPosition() - camera.GetGazePosition();
    gaze_tmp.NormalizeSelf();
    __m128 gaze_vector = _mm_load_ps(gaze_tmp.GetHead());                     //g
    __m128 up_vector = _mm_load_ps(camera.GetUpDirection().GetHead());        //t
    __m128 gaze_cross_up = Vector4fAlignas16::Cross3(gaze_vector, up_vector); //gxt
    __m128 gxt_t_low = _mm_unpacklo_ps(gaze_cross_up, up_vector);
    __m128 gxt_t_heigh = _mm_unpackhi_ps(gaze_cross_up, up_vector);
    gaze_vector = _mm_sub_ps(zero, gaze_vector);
    __m128 g_zero_low = _mm_unpacklo_ps(gaze_vector, zero);
    __m128 g_zero_heigh = _mm_unpackhi_ps(gaze_vector, zero);
    Matrix4fAlignas16 r_view_matrix{};
    _mm_store_ps(r_view_matrix.GetColumnHead<0>(), _mm_shuffle_ps(gxt_t_low, g_zero_low, _MM_SHUFFLE(1, 0, 1, 0)));
    _mm_store_ps(r_view_matrix.GetColumnHead<1>(), _mm_shuffle_ps(gxt_t_low, g_zero_low, _MM_SHUFFLE(3, 2, 3, 2)));
    _mm_store_ps(r_view_matrix.GetColumnHead<2>(), _mm_shuffle_ps(gxt_t_heigh, g_zero_heigh, _MM_SHUFFLE(1, 0, 1, 0)));
    r_view_matrix[3][3] = 1.0f;
    view_matrix_ = r_view_matrix * t_view_matrix;
}

void SoftRender::SetProjectionMatrix(const float z_near, const float z_far, const std::int32_t width, const std::int32_t height, const float fov_y_degree = 60.0f)
{
    constexpr float pi = 3.14159265358979323846f;
    float radian_theta = fov_y_degree / 180.0f * pi;
    float aspect_ratio = static_cast<float>(width) / static_cast<float>(height);
    float t = tan(radian_theta) * abs(z_near);
    float b = -t;
    float r = aspect_ratio * t;
    float l = -r;
    float cot_half_theta = cos(radian_theta / 2.0f) / sin(radian_theta / 2.0f);
    projection_matrix_ = {
        -(cot_half_theta / aspect_ratio), 0, 0, 0,
        0, -cot_half_theta, 0, 0,
        0, 0, (z_near + z_far) / (z_near - z_far), -(2.0f * z_near * z_far) / (z_near - z_far),
        0, 0, 1.0f, 0};
}

void SoftRender::Set24bppRGBPixelColorUnsafe(void* scan0, size_t pixel_index, const ColorRGB24& color)
{
    void* p_target = reinterpret_cast<decltype(color.r_)*>(scan0) + pixel_index;
    memcpy(p_target, &color, sizeof(decltype(color)));
}

auto SoftRender::Get24bppRGBPixelColorUnsafe(void* scan0, size_t x, size_t y) -> Vector3fAlignasDefault
{
    ColorRGB24 color{};
    size_t color_index = (x + (y * depth_buffer_.GetWidth())) * sizeof(decltype(color));
    void* p_target = reinterpret_cast<decltype(color.r_)*>(scan0) + color_index;
    memcpy(&color, p_target, sizeof(decltype(color)));
    return Vector3fAlignasDefault{
        color.r_ / 255.0f,
        color.g_ / 255.0f,
        color.b_ / 255.0f};
}

SoftRender::Camera::Camera(
    const Vector4fAlignas16& position,
    const Vector4fAlignas16& gaze_direction,
    const Vector4fAlignas16& up_direction,
    Window& window)
    : camera_{position, gaze_direction, up_direction.Normalize()}, ref_window_{window},
      on_key_down_event_guard_{window.on_key_down_, [this](const ceiba::KeyEventArgs& event_args) -> ceiba::EventState
                               { return this->KeyDownEventHandler(event_args); }}
{
}

ceiba::EventState SoftRender::Camera::KeyDownEventHandler(const ceiba::KeyEventArgs& event_args)
{
    float tmp;
    switch (event_args.virtual_key_code_)
    {
    case 0x57: //W Pressed
    {
        MoveWS([](float lhs, float rhs)
               { return lhs - rhs; });
        break;
    }
    case 0x41: //A Pressed
    {
        MoveAD([](float lhs, float rhs)
               { return lhs - rhs; });
        break;
    }
    case 0x44: //D Pressed
    {
        MoveAD([](float lhs, float rhs)
               { return lhs + rhs; });
        break;
    }
    case 0x53: //S Pressed
    {
        MoveWS([](float lhs, float rhs)
               { return lhs + rhs; });
        break;
    }
    case 0x45: //E Pressed
    {
        MoveQE([](float lhs, float rhs)
               { return lhs + rhs; });
        break;
    }
    case 0x51: //Q Pressed
    {
        MoveQE([](float lhs, float rhs)
               { return lhs - rhs; });
        break;
    }
    default:
        break;
    }
    ref_window_.Update();
    return ceiba::EventState::Continue;
}

const Vector4fAlignas16& SoftRender::Camera::GetPosition() const
{
    return camera_[Numerical(DataType::Position)];
}

const Vector4fAlignas16& SoftRender::Camera::GetGazePosition() const
{
    return camera_[Numerical(DataType::GazePostion)];
}

const Vector4fAlignas16& SoftRender::Camera::GetUpDirection() const
{
    return camera_[Numerical(DataType::UpDirection)];
}

void SoftRender::Camera::SetPosition(const Vector4fAlignas16& position)
{
    camera_[Numerical(DataType::Position)] = position;
}

void SoftRender::Camera::SetGazePosition(const Vector4fAlignas16& gaze_direction)
{
    camera_[Numerical(DataType::GazePostion)] = gaze_direction;
}

void SoftRender::Camera::SetUpDirection(const Vector4fAlignas16& up_direction)
{
    camera_[Numerical(DataType::UpDirection)] = up_direction.Normalize();
}

void SoftRender::SetObjContent(const ObjContentType& p_obj_content)
{
    p_obj_content_ = &p_obj_content;
}

ceiba::EventState SoftRender::Rendering(const ceiba::RenderEventArgs& event_args)
{
    if (!p_obj_content_)
    {
        return ceiba::EventState::Continue;
    }
    Gdiplus::Rect rect_to_lock{0, 0, static_cast<INT>(depth_buffer_.GetWidth()), static_cast<INT>(depth_buffer_.GetHeight())};
    Gdiplus::BitmapData render_target_picture_data{};
    up_render_picture_->LockBits(&rect_to_lock, Gdiplus::ImageLockModeWrite, g_softrender_bitmap_pixel_format, &render_target_picture_data);
    //重置背景为黑色
    if (render_target_picture_data.Scan0)
    {
        //memcpy(render_target_picture_data.Scan0, &background_color_, sizeof(background_color_));
        memset(render_target_picture_data.Scan0, /*std::numeric_limits<int>::max()*/ 0,
               static_cast<size_t>(render_target_picture_data.Width) * static_cast<size_t>(render_target_picture_data.Height) * sizeof(SoftRender::ColorRGB24));
    }
    else
    {
        throw std::runtime_error("Soft Render bitmap data pointer invalid");
    }
    //设置并计算MVP矩阵
    SetViewMatrix(camera_);
    SetProjectionMatrix(1.0f, 10.0f,
                        static_cast<INT>(depth_buffer_.GetWidth()), static_cast<INT>(depth_buffer_.GetHeight()));
    Matrix4fAlignas16 normal_matrix = (view_matrix_ * model_matrix_).InverseSelf().TransposeSelf();
    Matrix4fAlignas16 mvp_matrix = PrecomputeMVPMatrix();
    std::array<Vector4fAlignas16, 3> handeled_normal;
    //检查文件是否定义了材质
    if (!p_obj_content_->mtl_range_.empty()) //文件定义了材质
    {
    }
    else //文件未定义材质，使用默认材质
    {
        auto& mtls_in_obj = p_obj_content_->mtls_;
        static MtlContent tmp_mtl{
            {0.0004f, 0.0004f, 0.0004f} /*Ka*/, {1.0f, 1.0f, 1.0f} /*Kd*/, {0.2f, 0.2f, 0.2f} /*Ks*/,
            10.0f, //Ns
            1.0f,  //Ni
            1.0f   //d
        };
        auto& default_mtl = /*!mtls_in_obj.empty() ? mtls_in_obj.begin()->second :*/ tmp_mtl;
        for (auto& t : p_obj_content_->triangles)
        {
            //获得MVP变换后的坐标
            auto [handeled_position, mvp_position] = MVPAndViewportTransform(t.position, mvp_matrix);
            for (size_t i = 0; i < 3; i++)
            {
                handeled_normal[i] = normal_matrix * Vector4fAlignas16(t.normal[i], HomogeneousType::Vector);
            }
            //光栅化阶段
            //确定光栅化范围
            __m128 position_0 = _mm_load_ps(handeled_position[0].GetHead());
            __m128 position_1 = _mm_load_ps(handeled_position[1].GetHead());
            __m128 tmp_result = _mm_max_ps(position_0, position_1);
            __m128 position_2 = _mm_load_ps(handeled_position[2].GetHead());
            tmp_result = _mm_ceil_ps(_mm_max_ps(tmp_result, position_2));
            Vector4fAlignas16 position_result{};
            _mm_store_ps(position_result.GetHead(), tmp_result);
            auto raster_width_max = std::min(depth_buffer_.GetWidth(), static_cast<size_t>(std::max(0.0f, position_result.x())));
            auto raster_height_max = std::min(depth_buffer_.GetHeight(), static_cast<size_t>(std::max(0.0f, position_result.y())));
            tmp_result = _mm_floor_ps(_mm_min_ps(position_2, _mm_min_ps(position_0, position_1)));
            _mm_store_ps(position_result.GetHead(), tmp_result);
            auto raster_width_min = std::min(depth_buffer_.GetWidth(), static_cast<size_t>(std::max(0.0f, position_result.x())));
            auto raster_height_min = std::min(depth_buffer_.GetHeight(), static_cast<size_t>(std::max(0.0f, position_result.y())));
            //默认使用MSAA4x
            for (size_t y = raster_height_min; y < raster_height_max; y++)
            {
                for (size_t x = raster_width_min; x < raster_width_max; x++)
                {
                    //static Vector8f<32> msaa_sample_points{};
                    //static struct
                    //{
                    //    Vector4fAlignas16 barycentric_args_ = {};
                    //    float z_ = 0.0f;
                    //    bool z_test_result_ = false;
                    //} msaa_samples[4]{};
                    ////手动循环展开四个采样点的处理
                    ////处理采样点12
                    //static __m128 offset_12 = _mm_add_ps(
                    //    _mm_set_ps1(0.5f),
                    //    _mm_set_ps(
                    //        -0.1115987872501496f/*y*/, 0.33547833116953445f, //x,第二象限
                    //        0.33547833116953445f/*y*/, 0.1115987872501496f//x,第一象限
                    //    ));
                    //__m128 msaa_sample_12 = _mm_set_ps(
                    //    static_cast<float>(y), static_cast<float>(x),
                    //    static_cast<float>(y), static_cast<float>(x));
                    //msaa_sample_12 = _mm_add_ps(msaa_sample_12, offset_12);
                    //_mm_store_ps(msaa_sample_points.GetHead(), msaa_sample_12);

                    //auto msaa_point_1 = Vector4fAlignas16{ msaa_sample_points.x(),msaa_sample_points.y(),1.0f };
                    //msaa_samples[0].barycentric_args_ = GetBarycentricArgs(msaa_point_1, handeled_position);
                    //if (IsPointInTriangle(msaa_samples[0].barycentric_args_))
                    //{
                    //    float z = InterpolateZ(msaa_samples[0].barycentric_args_, handeled_position);
                    //    msaa_samples[0].z_ = z;
                    //    msaa_samples[0].z_test_result_ = depth_buffer_.TryToSetDepth(x, y, 0, z);
                    //}

                    //auto msaa_point_2 = Vector4fAlignas16{ msaa_sample_points.z(),msaa_sample_points.w(),1.0f };
                    //msaa_samples[1].barycentric_args_ = GetBarycentricArgs(msaa_point_2, handeled_position);
                    //if (IsPointInTriangle(msaa_samples[1].barycentric_args_))
                    //{
                    //    float z = InterpolateZ(msaa_samples[1].barycentric_args_, handeled_position);
                    //    msaa_samples[1].z_ = z;
                    //    msaa_samples[1].z_test_result_ = depth_buffer_.TryToSetDepth(x, y, 1, z);
                    //}

                    ////处理采样点34
                    //static __m128 offset_34 = _mm_add_ps(
                    //    _mm_set_ps1(0.5f),
                    //    _mm_set_ps(
                    //        -0.33547833116953445f/*y*/, 0.1115987872501496f,//x,第四象限
                    //        -0.33547833116953445f/*y*/, -0.1115987872501496f//x,第三象限
                    //    ));
                    //__m128 msaa_sample_34 = _mm_set_ps(
                    //    static_cast<float>(y), static_cast<float>(x),
                    //    static_cast<float>(y), static_cast<float>(x));
                    //msaa_sample_34 = _mm_add_ps(msaa_sample_34, offset_34);
                    //_mm_store_ps(msaa_sample_points.GetCentreHead(), msaa_sample_34);

                    //auto msaa_point_3 = Vector4fAlignas16{ msaa_sample_points.Get<4>(),msaa_sample_points.Get<5>(),1.0f };
                    //msaa_samples[2].barycentric_args_ = GetBarycentricArgs(msaa_point_3, handeled_position);
                    //if (IsPointInTriangle(msaa_samples[2].barycentric_args_))
                    //{
                    //    msaa_samples[2].barycentric_args_ = GetBarycentricArgs(msaa_point_3, handeled_position);
                    //    float z = InterpolateZ(msaa_samples[2].barycentric_args_, handeled_position);
                    //    msaa_samples[2].z_ = z;
                    //    msaa_samples[2].z_test_result_ = depth_buffer_.TryToSetDepth(x, y, 2, z);
                    //}

                    //auto msaa_point_4 = Vector4fAlignas16{ msaa_sample_points.Get<6>(),msaa_sample_points.Get<7>(),1.0f };
                    //msaa_samples[3].barycentric_args_ = GetBarycentricArgs(msaa_point_4, handeled_position);
                    //if (IsPointInTriangle(msaa_samples[3].barycentric_args_))
                    //{
                    //    float z = InterpolateZ(msaa_samples[3].barycentric_args_, handeled_position);
                    //    msaa_samples[3].z_ = z;
                    //    msaa_samples[3].z_test_result_ = depth_buffer_.TryToSetDepth(x, y, 3, z);
                    //}

                    Vector3fAlignasDefault average_color{0.0f, 0.0f, 0.0f};
                    //bool use_single_sample = true;
                    //for (auto& sample : msaa_samples)//MSAA四个采样点均有效则使用中间的点进行计算
                    //{
                    //    use_single_sample = use_single_sample && sample.z_test_result_;
                    //}
                    //ObjContentType::Vertex vertex_to_render{};
                    //if (use_single_sample)
                    //{
                    //    Vector4fAlignas16 centre_barycentric_args{
                    //        GetBarycentricArgs(Vector4fAlignas16{static_cast<float>(x) + 0.5f,static_cast<float>(y) + 0.5f},
                    //        handeled_position) };
                    //    float average_z = 0;
                    //    for (auto& sample : msaa_samples)
                    //    {
                    //        average_z += sample.z_;
                    //    }
                    //    average_z /= 4.0f;
                    //    vertex_to_render = GetVertex(centre_barycentric_args,
                    //        static_cast<float>(x), static_cast<float>(y), average_z, t);
                    //    average_color = DefaultShader(vertex_to_render, default_mtl, camera_.GetPosition());
                    //}
                    //else
                    //{
                    //    Vector3fAlignasDefault back_color = Get24bppRGBPixelColorUnsafe(render_target_picture_data.Scan0, x, y);
                    //    size_t msaa_sample_index = 0;
                    //    for (auto& sample : msaa_samples)
                    //    {
                    //        if (sample.z_test_result_)
                    //        {
                    //            vertex_to_render = GetVertex(sample.barycentric_args_,
                    //                msaa_sample_points.Get(msaa_sample_index),
                    //                msaa_sample_points.Get(msaa_sample_index + 1),
                    //                sample.z_, t);
                    //            msaa_sample_index += 2;
                    //            average_color += DefaultShader(vertex_to_render, default_mtl, camera_.GetPosition());
                    //        }
                    //        else
                    //        {
                    //            average_color += back_color;
                    //        }
                    //    }
                    //    average_color /= 4.0f;
                    //}
                    float centre_x = static_cast<float>(x) + 0.5f;
                    float centre_y = static_cast<float>(y) + 0.5f;
                    Vector4fAlignas16 centre_barycentric_args{
                        GetBarycentricArgs(Vector4fAlignas16{centre_x, centre_y, 1.0f}, handeled_position)};
                    if (IsPointInTriangle(centre_barycentric_args))
                    {
                        float depth = InterpolateZ(centre_barycentric_args, handeled_position);
                        if (depth_buffer_.TryToSetDepth(x, y, 0, depth))
                        {
                            auto vertex_to_render = GetVertex(centre_barycentric_args,
                                                              centre_x, centre_y, depth, t);
                            vertex_to_render.normal = Vector3fAlignas16::ToVector3(Interpolate3D(centre_barycentric_args, 1.0f, handeled_normal));
                            auto interpolated_view_position = Interpolate3D(centre_barycentric_args, 1.0f, mvp_position);
                            average_color = DepthVisualizer(vertex_to_render, default_mtl, camera_.GetPosition(), interpolated_view_position);
                            ColorRGB24 color_to_draw(average_color);
                            size_t color_index = (x + (y * depth_buffer_.GetWidth())) * sizeof(ColorRGB24);
                            Set24bppRGBPixelColorUnsafe(render_target_picture_data.Scan0, color_index, color_to_draw);
                        }
                    }
                }
            }
        }
    }
    up_render_picture_->UnlockBits(&render_target_picture_data);
    std::unique_ptr<Gdiplus::Graphics> up_graphics{Gdiplus::Graphics::FromHDC(event_args.paint_struct_.hdc)};
    up_graphics->DrawImage(up_render_picture_.get(),
                           0, 0);
    depth_buffer_.ResetDepthBuffer();
    return ceiba::EventState::Continue;
}

ceiba::EventState SoftRender::OnWindowSizeChange(const ceiba::SizeChangedEventArgs& event_args)
{
    up_render_picture_.reset();
    auto tmp_up = std::make_unique<Gdiplus::Bitmap>(event_args.new_width_, event_args.new_height_, g_softrender_bitmap_pixel_format);
    up_render_picture_.swap(tmp_up);
    return ceiba::EventState::Continue;
}

SoftRender::DepthBuffer::DepthBuffer(Window& target_window, size_t width, size_t height)
    : width_{width}, height_{height},
      size_change_event_guard_(target_window.on_size_changed_,
                               [this](const ceiba::SizeChangedEventArgs& event_args) -> ceiba::EventState
                               {
                                   ResizeDepthBuffer(event_args.new_width_, event_args.new_height_);
                                   return ceiba::EventState::Continue;
                               })
{
    ResizeDepthBuffer(width, height);
}

void SoftRender::DepthBuffer::ResizeDepthBuffer(size_t width, size_t height)
{
    size_t new_size = width * height * 4; //use MSAA 4x
    auto now = std::chrono::steady_clock::now();
    if (new_size > depth_buffer_.size())
    {
        depth_buffer_.reserve(new_size);
    }
    depth_buffer_.resize(new_size, default_depth_);
    width_ = width;
    height_ = height;
}

void SoftRender::DepthBuffer::ResetDepthBuffer()
{
    std::fill(depth_buffer_.begin(), depth_buffer_.end(), default_depth_);
}

float SoftRender::DepthBuffer::GetDepth(size_t x, size_t y, size_t sample_index) const
{
    return depth_buffer_[x + (y * width_) + sample_index];
}

size_t SoftRender::DepthBuffer::GetWidth() const
{
    return width_;
}

size_t SoftRender::DepthBuffer::GetHeight() const
{
    return height_;
}

void SoftRender::DepthBuffer::SetDepth(size_t x, size_t y, size_t sample_index, float new_depth)
{
    depth_buffer_[x + (y * width_) + sample_index] = new_depth;
}

bool SoftRender::DepthBuffer::TryToSetDepth(size_t x, size_t y, size_t sample_index, float new_depth)
{
    bool result = new_depth > GetDepth(x, y, sample_index);
    if (result)
    {
        SetDepth(x, y, sample_index, new_depth);
    }
    return result;
}

decltype(SoftRender::DepthBuffer::default_depth_) SoftRender::DepthBuffer::default_depth_ = -1.0f * std::numeric_limits<float>::infinity();

SoftRender::ColorRGB24::ColorRGB24(Vector3fAlignasDefault float_color)
{
    for (auto&& number : float_color)
    {
        if (number > 1.0f)
        {
            number = 1.f;
        }
        else if (number < 0.0f)
        {
            number = 0.0f;
        }
    }

    r_ = static_cast<std::uint8_t>(float_color.x() * 255.0f);
    g_ = static_cast<std::uint8_t>(float_color.y() * 255.0f);
    b_ = static_cast<std::uint8_t>(float_color.z() * 255.0f);
}

std::uint8_t SoftRender::ColorRGB24::LimitNumber(std::uint32_t number)
{
    std::uint32_t tmp = std::min(number, static_cast<std::uint32_t>(255));
    tmp = std::max(tmp, static_cast<std::uint32_t>(0));
    return static_cast<std::uint8_t>(tmp);
}
