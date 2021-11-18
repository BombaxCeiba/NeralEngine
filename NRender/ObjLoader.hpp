#pragma once
//***************************************************************************************
//ObjLoader.hpp by Dusk_NM02 (c) 2021 All Rights Reserved.
//***************************************************************************************
#include<string>
#include<functional>
#include<unordered_map>
#include<filesystem>
#include<fstream>
#include<variant>
#include"Triangle.h"

#define THROW_CUSTOM_EXCEPTION(exception_type,exception_content) \
{\
    class exception_type:public std::exception{\
    public:\
        exception_type(std::string content):std::exception(),error_text(content){};\
        const char* what()const noexcept override\
        {\
            return error_text.c_str();\
        };\
    private:\
        std::string error_text;\
    } ex(exception_content);\
    throw ex;\
}\

class MtlContent
{
public:
    MtlContent() :Ns(0.0f), Ni(0.0f), d(0.0f), illum(0) {}
    MtlContent(
        const Vector3fAlignas16& ka,
        const Vector3fAlignas16& kd,
        const Vector3fAlignas16& ks,
        const float ns,
        const float ni,
        const float d,
        const std::wstring& map_ka = {},
        const std::wstring& map_kd = {},
        const std::wstring& map_ks = {},
        const std::wstring& map_ns = {},
        const std::wstring& map_d = {},
        const std::wstring& map_dump = {},
        const int illum = 0
    ) :Ka{ ka }, Kd{ kd }, Ks{ ks }, Ns{ ns }, Ni{ ni }, d{ d }, map_Ka{ map_ka }, map_Kd{ map_kd }
        , map_Ns{ map_ns }, map_d{ map_d }, map_bump{ map_bump }, illum{ illum } {}
    ~MtlContent() = default;
    Vector3fAlignasDefault Ka;//环境反射 rgb
    Vector3fAlignasDefault Kd;//漫反射 rgb
    Vector3fAlignasDefault Ks;//镜面反射 rgb
    float Ns;//反射系数 指定材质的反射指数，定义了反射高光度
    float Ni;//折射值 指定材质表面的光密度，即折射值
    float d;//渐隐指数 表示物体融入背景的数量
    std::wstring map_Ka;//环境反射贴图名
    std::wstring map_Kd;//漫反射贴图名
    std::wstring map_Ks;//镜面反射贴图名
    std::wstring map_Ns;//反射系数贴图名
    std::wstring map_d;//渐隐系数贴图名
    std::wstring map_bump;//凹凸贴图名
    int illum;
};

enum class DataType : size_t
{
    Name,
    First,
    Second,
    Third
};

enum class ObjLoaderState
{
    AllAvailable,
    MtlFileInvalid,
    ObjFileInvalid
};

using MtlRange = std::pair<size_t, std::wstring>;
template<size_t align = alignof(std::max_align_t)>
struct ObjContent
{
    using TriangleVector = std::vector<Triangle<align>>;
    using MTLMap = std::unordered_map<std::wstring, MtlContent>;
    struct alignas(align) Vertex
    {
        Vector4f<align> position;
        Vector2f<align> texture_uv;
        Vector3f<align> normal;
        Vector3f<align> color;
    };
    TriangleVector triangles;
    MTLMap mtls_;
    std::vector<MtlRange> mtl_range_;
};


template<typename ...F>
class OverloadSet :public F ...
{
public:
    using F::operator()...;
};
template<typename ...F>
OverloadSet(F&& ...f)->OverloadSet<F...>;

inline auto InitializeObjFilePath = OverloadSet{
    [](const std::string& path)->std::filesystem::path {return std::filesystem::path{path};},
    [](const std::wstring& wpath)->std::filesystem::path {return std::filesystem::path{wpath};}
};

inline auto InitializeObjLoaderWifstream = OverloadSet{
    [](const std::string& path)->std::wifstream {return std::wifstream{path.c_str()};},
    [](const std::wstring& wpath)->std::wifstream {return std::wifstream{wpath.c_str()};}
};

template<typename Vec2f = Vector2fAlignas16, typename Vec3f = Vector3fAlignas16, size_t align = alignof(Vec3f)>
class ObjLoader
{
private:
    using StringTypes = std::variant<std::string, std::wstring>;
public:
    struct alignas(align) PointInfo
    {
        size_t v_index;
        size_t vt_index;
        size_t vn_index;
    };
    struct alignas(align) F
    {
        PointInfo points[3];
    };

    ObjLoader(StringTypes path);
    ~ObjLoader() = default;
    auto ReadOBJ()->ObjContent<align>;
private:
    auto ReadMTL()->std::unordered_map<std::wstring, MtlContent>;
public:
    ObjLoaderState state_;
private:
    using MTLAnalyzersFunctionParameterType1 = MtlContent*;
    using MTLAnalyzersFunctionParameterType2 = const std::vector<std::wstring>&;
    StringTypes obj_path_;
    std::wstring mtl_path_;
    static std::vector<std::wstring> SplitStringBySpace(const std::wstring& str);
};

template<typename Vec2f, typename Vec3f, size_t align>
inline std::vector<std::wstring> ObjLoader<Vec2f, Vec3f, align>::SplitStringBySpace(const std::wstring& str)
{
    std::vector<std::wstring> result{};
    if (str.empty())
    {
        return result;
    }
    size_t current_index = 0;
    size_t last_index = 0;
    for (auto c : str)
    {
        if (c == L' ') [[unlikely]]//遇到空格则分割字符串
        {
            result.push_back(str.substr(last_index, current_index - last_index));
            current_index++;
            last_index = current_index;
        }
        else if (c == L'#') [[unlikely]]//遇到注释直接跳过
        {
            return {};
        }
        else [[likely]]
        {
            current_index++;
        }
    }
    result.push_back(str.substr(last_index, current_index - last_index));//记录最后一段字符串
    return result;
}

template<typename Vec2f, typename Vec3f, size_t align>
inline ObjLoader<Vec2f, Vec3f, align>::ObjLoader(StringTypes path) :obj_path_(path), mtl_path_(L"\0"), state_(ObjLoaderState::AllAvailable)
{
    std::filesystem::path p_path{ std::visit(InitializeObjFilePath,path) };
    if (!std::filesystem::exists(p_path) || std::filesystem::is_empty(p_path))
    {
        /*throw std::exception("OBJ file not found!");*/
        state_ = ObjLoaderState::ObjFileInvalid;
    }

    std::wifstream obj_file{ std::visit(InitializeObjLoaderWifstream,obj_path_) };
    if (!obj_file.is_open())
    {
        /*throw std::exception("Can not open file!");*/
        state_ = ObjLoaderState::ObjFileInvalid;
    }
    for (std::wstring line; std::getline(obj_file, line);)
    {
        if (line[0] != L'#')
        {
            auto i = line.find_first_of(L' ');
            if (i != std::wstring::npos)
            {
                auto type_name = line.substr(0, i);
                if (type_name == L"mtllib")
                {
                    auto mtl_path = p_path.parent_path();
                    mtl_path /= std::filesystem::path(line.substr(i + 1));//意为mtl_path加斜杠再加文件名
                    if (std::filesystem::exists(mtl_path) || !std::filesystem::is_empty(mtl_path))
                    {
                        mtl_path_ = mtl_path.wstring();
                        break;
                    }
                    else
                    {
                        state_ = ObjLoaderState::MtlFileInvalid;
                    }
                }
            }
        }
    }
}

template<typename Vec2f, typename Vec3f, size_t align>
inline auto ObjLoader<Vec2f, Vec3f, align>::ReadOBJ()->ObjContent<align>
{
    std::locale::global(std::locale(""));
    std::wifstream obj_file{ std::visit(InitializeObjLoaderWifstream,obj_path_) };
    if (!obj_file.is_open())
    {
        /*throw std::exception("Can not open file!");*/
        THROW_CUSTOM_EXCEPTION(OpenOBJFileFailure, "Can not open obj file!");
    }

    std::vector<std::wstring> lines{};
    size_t v_count = 0, vt_count = 0, vn_count = 0, f_count = 0, mtl_count = 0;
    for (std::wstring line{}; std::getline(obj_file, line);)//第一次遍历文件，取得v,vt,vn,f的数量，并缓存文件到内存
    {
        lines.emplace_back(line);
        if (line[0] == L'#')
        {
            continue;
        }
        std::wstring first_token{ line.substr(0, 2) };
        if (first_token == L"v ")//几何顶点
        {
            v_count++;
        }
        else if (first_token == L"vt")//贴图坐标点
        {
            vt_count++;
        }
        else if (first_token == L"vn")//顶点法线
        {
            vn_count++;
        }
        else if (first_token == L"f ")//面
        {
            f_count++;
        }
        else if (first_token == L"usemtl")
        {
            mtl_count++;
        }
    }
    //根据成员数量初始化返回值
    ObjContent<align> result{};
    result.triangles.reserve(f_count);
    result.mtl_range_.reserve(mtl_count);
    //初始化临时变量，根据v,vt,vn,f的数量初始化数组
    std::vector<Vec3f> v_vector(0), vn_vector(0);
    v_vector.reserve(v_count); vn_vector.reserve(vn_count);
    std::vector<Vec2f> vt_vector(0);
    vt_vector.reserve(vt_count);
    std::vector<F> f_vector;
    f_vector.reserve(f_count);
    Vec3f vec3f_tmp{};
    Vec2f vec2f_tmp{};
    F f_tmp{};
    size_t f_index{ 0 };
    size_t last_newmtl = 0;

    for (auto& line : lines)//为成员分配内存后，第二次遍历文件并解析值后存入内存
    {
        auto tokens{ SplitStringBySpace(line) };
        if (!tokens.empty())
        {
            if (tokens[Numerical(DataType::Name)] == L"v")
            {
                vec3f_tmp.SetX(std::stof(tokens[Numerical(DataType::First)]));
                vec3f_tmp.SetY(std::stof(tokens[Numerical(DataType::Second)]));
                vec3f_tmp.SetZ(std::stof(tokens[Numerical(DataType::Third)]));
                v_vector.emplace_back(vec3f_tmp);
            }
            else if (tokens[Numerical(DataType::Name)] == L"vt")
            {
                vec2f_tmp.SetX(std::stof(tokens[Numerical(DataType::First)]));
                vec2f_tmp.SetY(std::stof(tokens[Numerical(DataType::Second)]));
                vt_vector.emplace_back(vec2f_tmp);
            }
            else if (tokens[Numerical(DataType::Name)] == L"vn")
            {
                vec3f_tmp.SetX(std::stof(tokens[Numerical(DataType::First)]));
                vec3f_tmp.SetY(std::stof(tokens[Numerical(DataType::Second)]));
                vec3f_tmp.SetZ(std::stof(tokens[Numerical(DataType::Third)]));
                vn_vector.emplace_back(vec3f_tmp);
            }
            else if (tokens[Numerical(DataType::Name)] == L"f")
            {
                for (size_t i = 1; i < 4; i++)
                {
                    //解析数据，格式 0/0/0
                    auto& current_string = tokens[i];
                    size_t first_slash_index = current_string.find_first_of(L'/');
                    size_t second_slash_index = current_string.find_last_of(L'/');

                    auto& current_point_info = f_tmp.points[i - 1];
                    current_point_info.v_index = std::stoi(current_string.substr(0, first_slash_index));
                    first_slash_index++;
                    current_point_info.vt_index = std::stoi(current_string.substr(first_slash_index, second_slash_index - first_slash_index));
                    current_point_info.vn_index = std::stoi(current_string.substr(second_slash_index + 1));
                }
                f_vector.emplace_back(f_tmp);
                f_index++;
            }
            else if (tokens[Numerical(DataType::Name)] == L"usemtl")
            {
                if (f_index > 0)
                {
                    result.mtl_range_.emplace_back(std::make_pair(f_index, tokens[Numerical(DataType::First)]));
                }
            }
        }
    }

    Triangle<align> t_tmp{};
    for (size_t i = 0; i < f_vector.size(); i++)
    {
        for (size_t j = 0; j < 3; j++)
        {
            auto& point_info = f_vector[i].points[j];
            t_tmp.position[j] = Vector4f<align>(v_vector[point_info.v_index - 1], HomogeneousType::Point);
            t_tmp.texture_uv[j] = vt_vector[point_info.vt_index - 1];
            t_tmp.normal[j] = vn_vector[point_info.vn_index - 1];
        }
        result.triangles.emplace_back(t_tmp);
    }

    result.mtls_ = ReadMTL();

    return result;
}

#define OBJLOADER_MTL_ANALYZERS_FUNCTION_PARAMETER_TYPES(var1,var2) MTLAnalyzersFunctionParameterType1 var1,MTLAnalyzersFunctionParameterType2 var2
#define OBJLOADER_MTL_ANALYZERS_FUNCTION_PARAMETERS OBJLOADER_MTL_ANALYZERS_FUNCTION_PARAMETER_TYPES(mtl_content,tokens)

template<typename Vec2f, typename Vec3f, size_t align>
inline auto ObjLoader<Vec2f, Vec3f, align>::ReadMTL()->std::unordered_map<std::wstring, MtlContent>
{
    std::unordered_map<std::wstring, MtlContent> result{};
    if (!mtl_path_.empty())
    {
        std::wifstream mtl_file(mtl_path_.c_str());
        if (!mtl_file.is_open())
        {
            THROW_CUSTOM_EXCEPTION(MtlFileInvalid, "mtl file invalid");
        }
        MtlContent* p_initializing_mtl = nullptr;
        for (std::wstring line; std::getline(mtl_file, line);)
        {
            auto tokens{ SplitStringBySpace(line) };
            if (tokens.empty())
            {
                continue;
            }
            else if (tokens[Numerical(DataType::Name)] == L"newmtl")
            {
                auto mtl_name = tokens[Numerical(DataType::First)];
                result.insert(std::make_pair<>(mtl_name, MtlContent{}));
                p_initializing_mtl = &result[mtl_name];
            }
            static const std::unordered_map<std::wstring, std::function<void(MTLAnalyzersFunctionParameterType1, MTLAnalyzersFunctionParameterType2)>> MTL_analyzers{
                 {L"Ka",[](OBJLOADER_MTL_ANALYZERS_FUNCTION_PARAMETERS) {
                    mtl_content->Ka.SetX(std::stof(tokens[Numerical(DataType::First)]));
                    mtl_content->Ka.SetY(std::stof(tokens[Numerical(DataType::Second)]));
                    mtl_content->Ka.SetZ(std::stof(tokens[Numerical(DataType::Third)]));
                }},
                {L"Kd",[](OBJLOADER_MTL_ANALYZERS_FUNCTION_PARAMETERS) {
                    mtl_content->Kd.SetX(std::stof(tokens[Numerical(DataType::First)]));
                    mtl_content->Kd.SetY(std::stof(tokens[Numerical(DataType::Second)]));
                    mtl_content->Kd.SetZ(std::stof(tokens[Numerical(DataType::Third)]));
                }},
                {L"Ks",[](OBJLOADER_MTL_ANALYZERS_FUNCTION_PARAMETERS) {
                    mtl_content->Ks.SetX(std::stof(tokens[Numerical(DataType::First)]));
                    mtl_content->Ks.SetY(std::stof(tokens[Numerical(DataType::Second)]));
                    mtl_content->Ks.SetZ(std::stof(tokens[Numerical(DataType::Third)]));
                }},
                {L"Ns",[](OBJLOADER_MTL_ANALYZERS_FUNCTION_PARAMETERS) {
                    mtl_content->Ns = std::stof(tokens[Numerical(DataType::First)]);
                }},
                {L"Ni",[](OBJLOADER_MTL_ANALYZERS_FUNCTION_PARAMETERS) {
                    mtl_content->Ni = std::stof(tokens[Numerical(DataType::First)]);
                }},
                {L"d",[](OBJLOADER_MTL_ANALYZERS_FUNCTION_PARAMETERS) {
                    mtl_content->d = std::stof(tokens[Numerical(DataType::First)]);
                }},
                {L"illum",[](OBJLOADER_MTL_ANALYZERS_FUNCTION_PARAMETERS) {
                    mtl_content->illum = std::stoi(tokens[Numerical(DataType::First)]);
                }},
                {L"map_Ka",[](OBJLOADER_MTL_ANALYZERS_FUNCTION_PARAMETERS) {
                    mtl_content->map_Ka = tokens[Numerical(DataType::First)];
                }},
                {L"map_Kd",[](OBJLOADER_MTL_ANALYZERS_FUNCTION_PARAMETERS) {
                    mtl_content->map_Kd = tokens[Numerical(DataType::First)];
                }},
                {L"map_Ks",[](OBJLOADER_MTL_ANALYZERS_FUNCTION_PARAMETERS) {
                    mtl_content->map_Ks = tokens[Numerical(DataType::First)];
                }},
                {L"map_Ns",[](OBJLOADER_MTL_ANALYZERS_FUNCTION_PARAMETERS) {
                    mtl_content->map_Ns = tokens[Numerical(DataType::First)];
                }},
                {L"map_d",[](OBJLOADER_MTL_ANALYZERS_FUNCTION_PARAMETERS) {
                    mtl_content->map_d = tokens[Numerical(DataType::First)];
                }},
                {L"map_bump",[](OBJLOADER_MTL_ANALYZERS_FUNCTION_PARAMETERS) {
                    mtl_content->map_bump = tokens[Numerical(DataType::First)];
                }}
            };

            if (p_initializing_mtl == nullptr)
            {
                continue;
            }

            auto it = MTL_analyzers.find(tokens[Numerical(DataType::Name)]);
            if (it != MTL_analyzers.end())
            {
                it->second(p_initializing_mtl, tokens);
            }
        }
    }
    return result;
}