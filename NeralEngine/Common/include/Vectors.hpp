/**
 * @author: Dusk
 * @date: 2022-03-04 19:13:27
 * @last modified by:   Dusk
 * @last modified time: 2022-03-04 19:13:27
 * @copyright Copyright (c) 2022 Dusk.
*/
#ifndef NERAL_ENGINE_COMMON_VECTORS_HPP
#define NERAL_ENGINE_COMMON_VECTORS_HPP

#include<cstddef>
#include<cmath>
#include<array>
#include<emmintrin.h>
#include<immintrin.h>

#define COMPLIE_WHEN_TYPE_ALIGNAS_16(FunctionReturnType) \
template<typename RetTy = FunctionReturnType> requires std::is_same_v<RetTy, FunctionReturnType> \
std::enable_if_t<align == 16, RetTy>\

#define SHUFFLE_SELF_PS(self, parameter) _mm_shuffle_ps((self), (self), parameter)
#define SHUFFLE_0101_PS(vec0,vec1) _mm_movelh_ps((vec0),(vec1))
#define SHUFFLE_2323_PS(vec0,vec1) _mm_movehl_ps((vec0),(vec1))
#define SHUFFLE_0022_PS(vec) _mm_moveldup_ps((vec))
#define SHUFFLE_1133_PS(vec) _mm_movehdup_ps((vec))

#define ENABLE_WHEN_ALIGNAS_2_ARGS(x, align) template<typename = std::enable_if_t<(align == (x))>>
#define ENABLE_WHEN_ALIGNAS_1_ARGS(x) ENABLE_WHEN_ALIGNAS_2_ARGS(x, align)

inline static __m128 g_m128Infinity = { 0x7F800000, 0x7F800000, 0x7F800000, 0x7F800000 };
inline static __m128 g_m128QNaN = { 0x7FC00000, 0x7FC00000, 0x7FC00000, 0x7FC00000 };

template<typename Enum> requires std::is_enum_v<Enum>
inline auto Numerical(Enum e) -> typename std::underlying_type<Enum>::type
{
    return static_cast<typename std::underlying_type<Enum>::type>(e);
}

template<typename __FromTy,typename __ToTy>
struct change_type
{
    using Type = __ToTy;
};
template<typename __FromTy, typename __ToTy>
using change_type_t = typename change_type<__FromTy, __ToTy>::Type;

enum class HomogeneousType
{
    Vector = 0,
    Point = 1
};

//对于下面的结构体，使用C++17及其以上的标准保证不需要使用自定义std::allocator分配对齐内存
template<typename __Ty, size_t __Size, size_t align, typename __TargetType>requires std::is_integral_v<__Ty> || std::is_floating_point_v<__Ty>
class alignas(align) VectorBase
{
    static_assert(__Size >= 0);//size_t本身大于等于0
public:
    using Type = __Ty;
    VectorBase() :numbers_{} {}
    template<typename... Ts> requires (std::is_convertible_v<Ts,__Ty> && ...)
    VectorBase(Ts... parameters) :numbers_{ {static_cast<__Ty>(std::forward<Ts>(parameters))...} } {};
protected:
    ~VectorBase() = default;
public:
    auto begin() noexcept
    {
        return numbers_.begin();
    }
    auto end() noexcept
    {
        return numbers_.end();
    }
#if (__cplusplus >= 201402L)
    auto cbegin()const noexcept
    {
        return numbers_.cbegin();
    }
    auto cend()const noexcept
    {
        return numbers_.cend();
    }
#endif
    auto rbegin() noexcept
    {
        return numbers_.rbegin();
    }
    auto rend() noexcept
    {
        return numbers_.rend();
    }
    __Ty& operator[](size_t i) noexcept
    {
        return numbers_[i];
    }
    const __Ty& operator[](size_t i) const noexcept
    {
        return numbers_[i];
    }
    constexpr size_t size() noexcept
    {
        return numbers_.size();
    }
    __Ty x() const
    {
        return numbers_[0];
    }
    template<typename Ret = __Ty>
    std::enable_if_t<(__Size >= 2), Ret> y() const
    {
        return numbers_[1];
    }
    template<typename Ret = __Ty>
    std::enable_if_t<(__Size >= 3), Ret> z() const
    {
        return numbers_[2];
    }
    template<typename Ret = __Ty>
    std::enable_if_t<(__Size >= 4), Ret> w() const
    {
        return numbers_[3];
    }
    void SetX(const __Ty num)
    {
        numbers_[0] = num;
    }
    template<typename Ret = __Ty>
    void SetY(const std::enable_if_t<(__Size >= 2), Ret> num)
    {
        numbers_[1] = num;
    }
    template<typename Ret = __Ty>
    void SetZ(const std::enable_if_t<(__Size >= 3), Ret> num)
    {
        numbers_[2] = num;
    }
    template<typename Ret = __Ty>
    void SetW(const std::enable_if_t<(__Size >= 4), Ret> num)
    {
        numbers_[3] = num;
    }
    template<size_t i, typename Ret = __Ty> requires std::is_same_v<Ret, __Ty>
    std::enable_if_t <(i < __Size), Ret> Get() const
    {
        return numbers_[i];
    }
    __Ty Get(const size_t i) const
    {
        return numbers_[i];
    }
    template<size_t i>
    void Set(const __Ty num)
    {
        numbers_[i] = num;
    }
    void Set(const size_t i, const __Ty num)
    {
        numbers_[i] = num;
    }
    const __Ty* GetHead() const noexcept
    {
        return numbers_.data();
    }
    __Ty* GetHead() noexcept
    {
        return numbers_.data();
    }
    __TargetType operator+(const __TargetType& rhs_vec) const
    {
        __TargetType result{};
        for (size_t i = 0; i < __Size; i++)
        {
            result.numbers_[i] = numbers_[i] + rhs_vec.numbers_[i];
        }
        return result;
    }
    __TargetType operator-(const __TargetType& rhs_vec) const
    {
        __TargetType result{};
        for (size_t i = 0; i < __Size; i++)
        {
            result.numbers_[i] = numbers_[i] - rhs_vec.numbers_[i];
        }
        return result;
    }
    __TargetType operator*(const __Ty rhs_num) const
    {
        __TargetType result{};
        for (size_t i = 0; i < __Size; i++)
        {
            result.numbers_[i] = numbers_[i] * rhs_num;
        }
        return result;
    }
    __TargetType operator*(const __TargetType& rhs_vec) const
    {
        __TargetType result{};
        for (size_t i = 0; i < __Size; i++)
        {
            result.numbers_[i] = numbers_[i] * rhs_vec.numbers_[i];
        }
        return result;
    }
    __TargetType& operator*=(const __Ty rhs_num)
    {
        for (size_t i = 0; i < __Size; i++)
        {
            numbers_[i] *= rhs_num;
        }
        return *(reinterpret_cast<__TargetType*>(this));
    }
    __TargetType& operator+=(const __TargetType rhs_vec)
    {
        for (size_t i = 0; i < __Size; i++)
        {
            numbers_[i] += rhs_vec.numbers_[i];
        }
        return *(reinterpret_cast<__TargetType*>(this));
    }
    __TargetType operator/(const __Ty rhs_num) const
    {
        __TargetType result{};
        for (size_t i = 0; i < __Size; i++)
        {
            result.numbers_[i] = numbers_[i] / rhs_num;
        }
        return result;
    }
    __TargetType& operator/=(const __Ty rhs_num)
    {
        for (size_t i = 0; i < __Size; i++)
        {
            numbers_[i] /= rhs_num;
        }
        return *(reinterpret_cast<__TargetType*>(this));
    }
    __Ty Dot(const __TargetType& rhs_vec) const
    {
        __Ty result = static_cast<__Ty>(0);
        for (size_t i = 0; i < __Size; i++)
        {
            result += numbers_[i] * rhs_vec.numbers_[i];
        }
        return result;
    }
    __TargetType Normalize() const
    {
        __Ty sum = static_cast<__Ty>(0);
        __TargetType result = *(this);
        for (size_t i = 0; i < __Size; i++)
        {
            sum += result.numbers_[i];
        }
        for (size_t i = 0; i < __Size; i++)
        {
            result.numbers_[i] /= sum;
        }
        return result;
    }
protected:
    std::array<__Ty, __Size> numbers_;
};

template<size_t align = alignof(std::max_align_t)>
class alignas(align) Vector2f :public VectorBase<float, 2, align, Vector2f<align>>
{
public:
    using Base = VectorBase<float, 2, align, Vector2f<align>>;
    using FigureType = typename Base::Type;
    using Base::Base;
    template<typename RHSFigureTy, typename RHS, size_t RHSSize, size_t RHSAlign, typename RetTy = Vector2f>
    requires std::is_convertible_v<RHSFigureTy, float>&& std::is_same_v<RetTy, Vector2f>
    static std::enable_if_t < (RHSSize > 2), RetTy > ToVector2(const VectorBase<RHSFigureTy, RHSSize, RHSAlign, RHS>& rhs_vec)
    {
        Vector2f result{
            rhs_vec.template Get<0>(),
            rhs_vec.template Get<1>(),
        };
        return result;
    }

};

template<size_t align = alignof(std::max_align_t)>
class alignas(align) Vector3f :public VectorBase<float, 3, align, Vector3f<align>>
{
public:
    using Base = VectorBase<float, 3, align, Vector3f<align>>;
    using FigureType = typename Base::Type;
    using Base::Base;
    template<typename RHSFigureTy, typename RHS,size_t RHSSize, size_t RHSAlign,typename RetTy = Vector3f>
    requires std::is_convertible_v<RHSFigureTy, float> && std::is_same_v<RetTy, Vector3f>
    static std::enable_if_t < (RHSSize > 3), RetTy > ToVector3(const VectorBase<RHSFigureTy, RHSSize, RHSAlign, RHS>& rhs_vec)
    {
        Vector3f result{
            rhs_vec.template Get<0>(),
            rhs_vec.template Get<1>(),
            rhs_vec.template Get<2>()
        };
        return result;
    }
    template<size_t RHSAlign, typename RHS>
    Vector3f(const VectorBase<FigureType, 3, RHSAlign, RHS>& rhs_vec3)
    {
        Base::numbers_[0] = rhs_vec3.template Get<0>();
        Base::numbers_[1] = rhs_vec3.template Get<1>();
        Base::numbers_[2] = rhs_vec3.template Get<2>();
    }
    /*Vector3f& operator+=(const Vector3f& rhs_vec)
    {
        return (reinterpret_cast<Base*>(this))->operator+=(rhs_vec);
    }*/
};

template<size_t align = alignof(std::max_align_t)>
class alignas(align) Vector4f :public VectorBase<float, 4, align, Vector4f<align>>
{
public:
    using Base = VectorBase<float, 4, align, Vector4f<align>>;
    using FigureType = typename Base::Type;
    using Base::Base;
    template<typename T>
    Vector4f(const T& vec3f, const HomogeneousType vector_type)
    {
        Base::numbers_[0] = vec3f.x();
        Base::numbers_[1] = vec3f.y();
        Base::numbers_[2] = vec3f.z();
        Base::numbers_[3] = static_cast<FigureType>(Numerical(vector_type));
    }

    Vector4f<16>& operator*=(const float rhs_num)
    {
        __m128 lhs = _mm_load_ps(this->GetHead());
        __m128 rhs = _mm_set_ps1(rhs_num);
        lhs = _mm_mul_ps(lhs, rhs);
        _mm_store_ps(this->GetHead(), lhs);
        return *this;
    }

    Vector4f<16> operator/(const float rhs_num) const
    {
        __m128 lhs = _mm_load_ps(this->GetHead());
        __m128 rhs = _mm_set_ps1(rhs_num);
        Vector4f<16> ret{};
        _mm_store_ps(ret.GetHead(), _mm_div_ps(lhs, rhs));
        return ret;
    }

    Vector4f<16> operator+(const Vector4f<16>& rhs_vec) const
    {
        __m128 lhs = _mm_load_ps(this->GetHead());
        __m128 rhs = _mm_load_ps(rhs_vec.GetHead());
        Vector4f<16> ret{};
        _mm_store_ps(ret.GetHead(), _mm_add_ps(lhs, rhs));
        return ret;
    }

    Vector4f<16> operator-(const Vector4f<16>& rhs_vec) const
    {
        __m128 lhs = _mm_load_ps(this->GetHead());
        __m128 rhs = _mm_load_ps(rhs_vec.GetHead());
        Vector4f<16> ret{};
        _mm_store_ps(ret.GetHead(), _mm_sub_ps(lhs, rhs));
        return ret;
    }

    Vector4f<16> Cross3(const Vector4f<16>& rhs_vec) const
    {
        __m128 vec0 = _mm_load_ps(this->GetHead());
        __m128 vec1 = _mm_load_ps(rhs_vec.GetHead());

        __m128 tmp0 = _mm_shuffle_ps(vec0, vec0, _MM_SHUFFLE(3, 0, 2, 1));
        __m128 tmp1 = _mm_shuffle_ps(vec1, vec1, _MM_SHUFFLE(3, 1, 0, 2));
        __m128 tmp2 = _mm_mul_ps(tmp0, vec1);
        __m128 tmp3 = _mm_mul_ps(tmp0, tmp1);
        __m128 tmp4 = _mm_shuffle_ps(tmp2, tmp2, _MM_SHUFFLE(3, 0, 2, 1));
        Vector4f<16> result{};
        _mm_store_ps(result.GetHead(), _mm_sub_ps(tmp3, tmp4));
        return result;
    }

    ENABLE_WHEN_ALIGNAS_1_ARGS(16)
    static __m128 Cross3(const __m128 lhs_vec, const __m128 rhs_vec)
    {
        __m128 tmp0 = _mm_shuffle_ps(lhs_vec, lhs_vec, _MM_SHUFFLE(3, 0, 2, 1));
        __m128 tmp1 = _mm_shuffle_ps(rhs_vec, rhs_vec, _MM_SHUFFLE(3, 1, 0, 2));
        __m128 tmp2 = _mm_mul_ps(tmp0, rhs_vec);
        __m128 tmp3 = _mm_mul_ps(tmp0, tmp1);
        __m128 tmp4 = _mm_shuffle_ps(tmp2, tmp2, _MM_SHUFFLE(3, 0, 2, 1));
        return  _mm_sub_ps(tmp3, tmp4);
    }

    bool operator==(const Vector4f<16>& rhs_vec) const
    {
        __m128 lhs = _mm_load_ps(this->GetHead());
        __m128 rhs = _mm_load_ps(rhs_vec.GetHead());
        __m128 result = _mm_cmpeq_ps(lhs, rhs);
        std::int32_t mask = _mm_movemask_ps(result);
        return mask == 0xf;
    }

    FigureType Dot(const Vector4f<16>& rhs_vec) const
    {
        __m128 lhs = _mm_load_ps(this->GetHead());
        __m128 rhs = _mm_load_ps(rhs_vec.GetHead());
        Vector4f<16> tmp_result{};
        _mm_store_ps(tmp_result.GetHead(), _mm_mul_ps(lhs, rhs));
        float result = 0.0f;
        for (auto num:tmp_result)
        {
            result += num;
        }
        return result;
    }
    [[nodiscard]]
    FigureType Norm() const
    {
        __m128 all_numbers = _mm_load_ps(this->GetHead());
        all_numbers = _mm_mul_ps(all_numbers, all_numbers);
        all_numbers = _mm_hadd_ps(all_numbers, all_numbers);
        all_numbers = _mm_hadd_ps(all_numbers, all_numbers);
        Vector4f<align> result{};
        _mm_store_ps(result.GetHead(), all_numbers);
        return std::sqrt(result.x());
    }
    ENABLE_WHEN_ALIGNAS_1_ARGS(16)
    void NormalizeSelf()
    {
        __m128 source_square = _mm_load_ps(this->GetHead());
        __m128 source_numbers = source_square;
        source_square = _mm_mul_ps(source_square, source_square);
        source_square = _mm_hadd_ps(source_square, source_square);
        source_square = _mm_hadd_ps(source_square, source_square);
        source_square = _mm_shuffle_ps(source_square, source_square, _MM_SHUFFLE(0, 0, 0, 0));
        __m128 all_numbers_be_div = _mm_sqrt_ps(source_square);
        __m128 zero_mask = _mm_setzero_ps();
        zero_mask = _mm_cmpneq_ps(zero_mask, all_numbers_be_div);
        source_square = _mm_cmpneq_ps(source_square, g_m128Infinity);
        source_numbers = _mm_div_ps(source_numbers, all_numbers_be_div);
        source_numbers = _mm_and_ps(source_numbers, zero_mask);//
        _mm_store_ps(this->GetHead(), _mm_or_ps(
            _mm_andnot_ps(source_square, g_m128QNaN),
            _mm_and_ps(source_numbers, source_square)));
    }
    ENABLE_WHEN_ALIGNAS_1_ARGS(16)
    [[nodiscard]]
    Vector4f<16> Normalize() const
    {
        __m128 source_square = _mm_load_ps(this->GetHead());
        __m128 source_numbers = source_square;
        source_square = _mm_mul_ps(source_square, source_square);
        source_square = _mm_hadd_ps(source_square, source_square);
        source_square = _mm_hadd_ps(source_square, source_square);
        source_square = _mm_shuffle_ps(source_square, source_square, _MM_SHUFFLE(0, 0, 0, 0));
        __m128 all_numbers_be_div = _mm_sqrt_ps(source_square);
        __m128 zero_mask = _mm_setzero_ps();
        zero_mask = _mm_cmpneq_ps(zero_mask, all_numbers_be_div);
        source_square = _mm_cmpneq_ps(source_square, g_m128Infinity);
        source_numbers = _mm_div_ps(source_numbers, all_numbers_be_div);
        source_numbers = _mm_and_ps(source_numbers, zero_mask);
        Vector4f<16> return_value{};
        _mm_store_ps(return_value.GetHead(), _mm_or_ps(
            _mm_andnot_ps(source_square, g_m128QNaN),
            _mm_and_ps(source_numbers, source_square)));
        return return_value;
    }
    ENABLE_WHEN_ALIGNAS_1_ARGS(16)
    void HomogeneousDivisionSelf()
    {
        //FigureType w = this->w();
        if (this->w() != 0)
        {
            __m128 source = _mm_load_ps(this->GetHead());
            __m128 divisor = SHUFFLE_SELF_PS(source, _MM_SHUFFLE(3, 3, 3, 3));//w
            _mm_store_ps(this->GetHead(), _mm_div_ps(source, divisor));
        }
        //this->SetW(1.0f / w);
    }

   /* bool operator!=(const Vector4f<16>& rhs_vec)
    {
        __m128 lhs = _mm_load_ps(&(this->x));
        __m128 rhs = _mm_load_ps(&rhs_vec.x);
        __m128 result = _mm_cmpeq_ps(lhs, rhs);
        std::int32_t mask = _mm_movemask_ps(result);
        return mask != 0xf;
    }*/
};

template<size_t align = alignof(std::max_align_t)>
class alignas(align) Vector8f :public VectorBase<float, 8, align, Vector8f<align>>
{
public:
    using Base = VectorBase<float, 8, align, Vector8f<align>>;
    using FigureType = typename Base::Type;
    using Base::Base;
    const FigureType* GetCentreHead() const
    {
        return &(Base::numbers_[4]);
    }
    FigureType* GetCentreHead()
    {
        return &(Base::numbers_[4]);
    }
};

template<typename __VectorType, size_t __Size,typename __TargetType>
class alignas(alignof(__VectorType)) MatrixBase
{
public:
    using VectorTy = __VectorType;
    using FigureTy = typename __VectorType::FigureType;
private:
    template<size_t index, typename T, typename... Ts>
    constexpr inline void InputHandler(T first_parameter, Ts... others)
    {
        //index mod __Size 得到的值为列数，index / __Size得到的值为行数
        if constexpr (sizeof...(Ts) == 0)//递归结束
        {
            column_[index % __Size][index / __Size] = first_parameter;
        }
        else
        {
            column_[index % __Size][index / __Size] = first_parameter;
            InputHandler<index + 1, Ts...>(others...);//类型已统一，不再完美转发
        }
    }
public:
    MatrixBase() = default;
    //仅初始化用户指定的数字，按行优先顺序排列，构造函数会自动进行类型转换，未指定数字应默认初始化为0（此行为实际由矩阵内部的Vector类型定义）
    template<typename... Ts> requires (std::is_convertible_v<Ts, FigureTy> && ...)
        constexpr MatrixBase(Ts... parameters)
    {
        InputHandler<0, change_type_t<Ts, FigureTy>...>(static_cast<FigureTy>(std::forward<Ts>(parameters))...);
    }
protected:
    ~MatrixBase() = default;
public:
    auto begin() noexcept
    {
        return column_.begin();
    }
    auto end() noexcept
    {
        return column_.end();
    }
    auto rbegin() noexcept
    {
        return column_.rbegin();
    }
    auto rend() noexcept
    {
        return column_.rend();
    }
    VectorTy& operator[](const size_t i) noexcept
    {
        return column_[i];
    }
    const VectorTy& operator[](const size_t i) const noexcept
    {
        return column_[i];
    }
    constexpr size_t size() noexcept
    {
        return column_.size();
    }
    const FigureTy GetByRowMajor(const size_t row, const size_t col)const noexcept
    {
        return column_[col][row];
    }
    void SetByRowMajor(const size_t row, const size_t col,const FigureTy value)
    {
        column_[col][row] = value;
    }

    template<size_t i>
    const __VectorType& GetColumn() const
    {
        static_assert(i < __Size && i >= 0);
        return column_[i];
    }
    const __VectorType& GetColumn(const size_t i) const
    {
        return column_[i];
    }
    template<size_t i, typename RetTy = FigureTy*>
    auto GetColumnHead()->std::enable_if_t<(i < __Size&& i >= 0), RetTy>
    {
        return column_[i].GetHead();
    }
    FigureTy* GetColumnHead(const size_t i)
    {
        return column_[i].GetHead();
    }
    template<size_t i, typename RetTy = const FigureTy*>
    auto GetColumnHead() const->std::enable_if_t<(i < __Size&& i >= 0), RetTy>
    {
        return column_[i].GetHead();
    }
    const FigureTy* GetColumnHead(const size_t i) const
    {
        return column_[i].GetHead();
    }
    template<size_t i, typename RetTy = void>
    auto SetColumn(const __VectorType& vec)->std::enable_if_t<(i < __Size&& i >= 0), RetTy>
    {
        column_[i] = vec;
    }
    void SetColumn(const __VectorType& vec, size_t i)
    {
        column_[i] = vec;
    }

    bool operator==(const MatrixBase& rhs) const
    {
        for (size_t i = 0; i < 4; i++)
        {
            if (this->column_[i] != rhs.column_[i])
            {
                return false;
            }
        }
        return true;
    }
    constexpr static __TargetType IdentityMatrix()
    {
        __TargetType identity_matrix = {};
        for (auto i = 0; i < identity_matrix.size(); i++)
        {
            identity_matrix[i][i] = static_cast<typename VectorTy::Type>(1);
        }
        return identity_matrix;
    }
protected:
    std::array<__VectorType, __Size> column_;
};

template<size_t align = alignof(std::max_align_t)>
class alignas(align) Matrix2f : public MatrixBase<Vector2f<align>, 2, Matrix2f<align>>
{
public:
    using Base = MatrixBase<Vector2f<align>, 2, Matrix2f>;
    using Base::Base;
    using VectorType = typename Base::VectorTy;
    using FigureType = typename Base::FigureTy;
    Matrix2f() = default;
    Matrix2f(const VectorType& vec0, const VectorType& vec1)
    {
        Base::column_[0] = vec0;
        Base::column_[1] = vec1;
    }
};

template<size_t align = alignof(std::max_align_t)>
class alignas(align) Matrix3f : public MatrixBase<Vector3f<align>, 3 , Matrix3f<align>>
{
public:
    using Base = MatrixBase<Vector3f<align>, 3, Matrix3f>;
    using Base::Base;
    using VectorType = typename Base::VectorTy;
    using FigureType = typename Base::FigureTy;
    Matrix3f() = default;
    Matrix3f(const VectorType& vec0, const VectorType& vec1, const VectorType& vec2)
    {
        Base::column_[0] = vec0;
        Base::column_[1] = vec1;
        Base::column_[2] = vec2;
    }
};

template<size_t align = alignof(std::max_align_t)>
class alignas(align) Matrix4f : public MatrixBase<Vector4f<align>, 4 , Matrix4f<align>>
{
public:
    using Base = MatrixBase<Vector4f<align>, 4, Matrix4f>;
    using Base::Base;
    using VectorType = typename Base::VectorTy;
    using FigureType = typename Base::FigureTy;
    Matrix4f() = default;
    Matrix4f(const VectorType& vec0, const VectorType& vec1, const VectorType& vec2, const VectorType& vec3)
    {
        Base::column_[0] = vec0;
        Base::column_[1] = vec1;
        Base::column_[2] = vec2;
        Base::column_[3] = vec3;
    }
//reference from https://lxjk.github.io/2017/09/03/Fast-4x4-Matrix-Inverse-with-SSE-SIMD-Explained.html
private:
    ENABLE_WHEN_ALIGNAS_1_ARGS(16)
    inline __m128 Matrix2fMultiply(__m128 lhs, __m128 rhs)
    {
        return _mm_add_ps(
            _mm_mul_ps(lhs, SHUFFLE_SELF_PS(rhs, _MM_SHUFFLE(0, 3, 0, 3))),
            _mm_mul_ps(SHUFFLE_SELF_PS(lhs, _MM_SHUFFLE(1, 0, 3, 2)), SHUFFLE_SELF_PS(rhs, _MM_SHUFFLE(2, 1, 2, 1))));
    }
    ENABLE_WHEN_ALIGNAS_1_ARGS(16)
    inline __m128 Matrix2fMulAdjMatrix2f(__m128 lhs, __m128 rhs)
    {
        return _mm_sub_ps(
            _mm_mul_ps(SHUFFLE_SELF_PS(lhs, _MM_SHUFFLE(3, 3, 0, 0)), rhs),
            _mm_mul_ps(SHUFFLE_SELF_PS(lhs, _MM_SHUFFLE(1, 1, 2, 2)), SHUFFLE_SELF_PS(rhs, _MM_SHUFFLE(2, 3, 0, 1))));
    }
    ENABLE_WHEN_ALIGNAS_1_ARGS(16)
    inline __m128 AdjMatrix2fMulMatrix2f(__m128 lhs, __m128 rhs)
    {
        return _mm_sub_ps(
            _mm_mul_ps(lhs, SHUFFLE_SELF_PS(rhs, _MM_SHUFFLE(3, 0, 3, 0))),
            _mm_mul_ps(SHUFFLE_SELF_PS(lhs, _MM_SHUFFLE(1, 0, 3, 2)), SHUFFLE_SELF_PS(rhs, _MM_SHUFFLE(2, 1, 2, 1))));
    }
public:
    ENABLE_WHEN_ALIGNAS_1_ARGS(16)
    Matrix4f InverseSelf()
    {
        //use __m128 as matrix2f
        //the data in memory looks like A1 A2 A3 A4
        //this can be seem as |A B|
        //                    |C D|
        __m128 column_0 = _mm_load_ps(this->template GetColumnHead<0>());
        __m128 column_1 = _mm_load_ps(this->template GetColumnHead<1>());
        __m128 column_2 = _mm_load_ps(this->template GetColumnHead<2>());
        __m128 column_3 = _mm_load_ps(this->template GetColumnHead<3>());

        __m128 A = SHUFFLE_0101_PS(column_0, column_1);
        __m128 C = SHUFFLE_2323_PS(column_0, column_1);
        __m128 B = SHUFFLE_0101_PS(column_2, column_3);
        __m128 D = SHUFFLE_2323_PS(column_2, column_3);

        A = SHUFFLE_SELF_PS(A, _MM_SHUFFLE(0, 2, 1, 3));
        C = SHUFFLE_SELF_PS(B, _MM_SHUFFLE(0, 2, 1, 3));
        B = SHUFFLE_SELF_PS(C, _MM_SHUFFLE(0, 2, 1, 3));
        D = SHUFFLE_SELF_PS(D, _MM_SHUFFLE(0, 2, 1, 3));

        // determinant as (|A| |B| |C| |D|)
        __m128 dSub = _mm_sub_ps(
            _mm_mul_ps(_mm_shuffle_ps(column_0, column_2, _MM_SHUFFLE(0, 2, 0, 2)), _mm_shuffle_ps(column_1, column_3, _MM_SHUFFLE(1, 3, 1, 3))),
            _mm_mul_ps(_mm_shuffle_ps(column_0, column_2, _MM_SHUFFLE(1, 3, 1, 3)), _mm_shuffle_ps(column_1, column_3, _MM_SHUFFLE(0, 2, 0, 2)))
        );
        __m128 dA = SHUFFLE_SELF_PS(dSub, _MM_SHUFFLE(0, 0, 0, 0));
        __m128 dB = SHUFFLE_SELF_PS(dSub, _MM_SHUFFLE(1, 1, 1, 1));
        __m128 dC = SHUFFLE_SELF_PS(dSub, _MM_SHUFFLE(2, 2, 2, 2));
        __m128 dD = SHUFFLE_SELF_PS(dSub, _MM_SHUFFLE(3, 3, 3, 3));

        __m128 DAsteriskC = Matrix2fMulAdjMatrix2f(D, C);
        __m128 AAsteriskB = Matrix2fMulAdjMatrix2f(A, B);
        __m128 X = _mm_sub_ps(_mm_mul_ps(dD, A), Matrix2fMultiply(B, DAsteriskC));
        __m128 W = _mm_sub_ps(_mm_mul_ps(dA, D), Matrix2fMultiply(C, AAsteriskB));

        __m128 dM = _mm_mul_ps(dA, dD);

        __m128 Y = _mm_sub_ps(_mm_mul_ps(dB, C), Matrix2fMulAdjMatrix2f(D, AAsteriskB));
        __m128 Z = _mm_sub_ps(_mm_mul_ps(dC, B), Matrix2fMulAdjMatrix2f(A, DAsteriskC));

        dM = _mm_add_ps(dM, _mm_mul_ps(dB, dC));

        __m128 tr = _mm_mul_ps(AAsteriskB, SHUFFLE_SELF_PS(DAsteriskC, _MM_SHUFFLE(0, 2, 1, 3)));
        tr = _mm_hadd_ps(tr, tr);
        tr = _mm_hadd_ps(tr, tr);
        dM = _mm_sub_ps(dM, tr);

        const __m128 adjSignMask = _mm_setr_ps(1.0f, -1.0f, -1.0f, 1.0f);
        __m128 rDetM = _mm_div_ps(adjSignMask, dM);

        X = _mm_mul_ps(X, rDetM);
        Y = _mm_mul_ps(Y, rDetM);
        Z = _mm_mul_ps(Z, rDetM);
        W = _mm_mul_ps(W, rDetM);

        column_0 = _mm_shuffle_ps(X, Z, _MM_SHUFFLE(3, 2, 3, 2));
        column_1 = _mm_shuffle_ps(X, Z, _MM_SHUFFLE(1, 0, 1, 0));
        column_2 = _mm_shuffle_ps(Z, W, _MM_SHUFFLE(3, 2, 3, 2));
        column_3 = _mm_shuffle_ps(Z, W, _MM_SHUFFLE(1, 0, 1, 0));

        _mm_store_ps(this->template GetColumnHead<0>(), column_0);
        _mm_store_ps(this->template GetColumnHead<1>(), column_1);
        _mm_store_ps(this->template GetColumnHead<2>(), column_2);
        _mm_store_ps(this->template GetColumnHead<3>(), column_3);

        return *this;
    }
    ENABLE_WHEN_ALIGNAS_1_ARGS(16)
    Matrix4f TransposeSelf()
    {
        __m128 c0 = _mm_load_ps(this->template GetColumnHead<0>());
        __m128 c1 = _mm_load_ps(this->template GetColumnHead<1>());
        __m128 c01L = _mm_unpacklo_ps(c0, c1);
        __m128 c01H = _mm_unpackhi_ps(c0, c1);

        __m128 c2 = _mm_load_ps(this->template GetColumnHead<2>());
        __m128 c3 = _mm_load_ps(this->template GetColumnHead<3>());
        __m128 c23L = _mm_unpacklo_ps(c2, c3);
        __m128 c23H = _mm_unpackhi_ps(c2, c3);

        _mm_store_ps(this->template GetColumnHead<0>(), _mm_shuffle_ps(c01L, c23L, _MM_SHUFFLE(1, 0, 1, 0)));
        _mm_store_ps(this->template GetColumnHead<1>(), _mm_shuffle_ps(c01L, c23L, _MM_SHUFFLE(3, 2, 3, 2)));
        _mm_store_ps(this->template GetColumnHead<2>(), _mm_shuffle_ps(c01H, c23H, _MM_SHUFFLE(1, 0, 1, 0)));
        _mm_store_ps(this->template GetColumnHead<3>(), _mm_shuffle_ps(c01H, c23H, _MM_SHUFFLE(3, 2, 3, 2)));

        return *this;
    }
    Matrix4f<16> operator*(const Matrix4f<16>& rhs) const
    {
        Matrix4f<16> result{};

        __m128 lhs_column_0 = _mm_load_ps(this->template GetColumnHead<0>());
        __m128 lhs_column_1 = _mm_load_ps(this->template GetColumnHead<1>());
        __m128 lhs_column_2 = _mm_load_ps(this->template GetColumnHead<2>());
        __m128 lhs_column_3 = _mm_load_ps(this->template GetColumnHead<3>());
        //使用current_w作为缓存
        //手动循环展开，第一轮
        __m128 current_w = _mm_load_ps(rhs.GetColumnHead<0>());
        __m128 current_x = SHUFFLE_SELF_PS(current_w, _MM_SHUFFLE(0, 0, 0, 0));
        __m128 current_y = SHUFFLE_SELF_PS(current_w, _MM_SHUFFLE(1, 1, 1, 1));
        __m128 current_z = SHUFFLE_SELF_PS(current_w, _MM_SHUFFLE(2, 2, 2, 2));
        current_w = SHUFFLE_SELF_PS(current_w, _MM_SHUFFLE(3, 3, 3, 3));
        current_x = _mm_mul_ps(current_x, lhs_column_0);
        current_y = _mm_mul_ps(current_y, lhs_column_1);
        current_z = _mm_mul_ps(current_z, lhs_column_2);
        current_w = _mm_mul_ps(current_w, lhs_column_3);

        current_x = _mm_add_ps(current_x, current_y);
        current_z = _mm_add_ps(current_z, current_w);

        _mm_store_ps(result.GetColumnHead<0>(), _mm_add_ps(current_x, current_z));

        //第二轮
        current_w = _mm_load_ps(rhs.GetColumnHead<1>());
        current_x = SHUFFLE_SELF_PS(current_w, _MM_SHUFFLE(0, 0, 0, 0));
        current_y = SHUFFLE_SELF_PS(current_w, _MM_SHUFFLE(1, 1, 1, 1));
        current_z = SHUFFLE_SELF_PS(current_w, _MM_SHUFFLE(2, 2, 2, 2));
        current_w = SHUFFLE_SELF_PS(current_w, _MM_SHUFFLE(3, 3, 3, 3));
        current_x = _mm_mul_ps(current_x, lhs_column_0);
        current_y = _mm_mul_ps(current_y, lhs_column_1);
        current_z = _mm_mul_ps(current_z, lhs_column_2);
        current_w = _mm_mul_ps(current_w, lhs_column_3);

        current_x = _mm_add_ps(current_x, current_y);
        current_z = _mm_add_ps(current_z, current_w);
        _mm_store_ps(result.GetColumnHead<1>(), _mm_add_ps(current_x, current_z));

        //第三轮
        current_w = _mm_load_ps(rhs.GetColumnHead<2>());
        current_x = SHUFFLE_SELF_PS(current_w, _MM_SHUFFLE(0, 0, 0, 0));
        current_y = SHUFFLE_SELF_PS(current_w, _MM_SHUFFLE(1, 1, 1, 1));
        current_z = SHUFFLE_SELF_PS(current_w, _MM_SHUFFLE(2, 2, 2, 2));
        current_w = SHUFFLE_SELF_PS(current_w, _MM_SHUFFLE(3, 3, 3, 3));
        current_x = _mm_mul_ps(current_x, lhs_column_0);
        current_y = _mm_mul_ps(current_y, lhs_column_1);
        current_z = _mm_mul_ps(current_z, lhs_column_2);
        current_w = _mm_mul_ps(current_w, lhs_column_3);

        current_x = _mm_add_ps(current_x, current_y);
        current_z = _mm_add_ps(current_z, current_w);
        _mm_store_ps(result.GetColumnHead<2>(), _mm_add_ps(current_x, current_z));
        //第四轮
        current_w = _mm_load_ps(rhs.GetColumnHead<3>());
        current_x = SHUFFLE_SELF_PS(current_w, _MM_SHUFFLE(0, 0, 0, 0));
        current_y = SHUFFLE_SELF_PS(current_w, _MM_SHUFFLE(1, 1, 1, 1));
        current_z = SHUFFLE_SELF_PS(current_w, _MM_SHUFFLE(2, 2, 2, 2));
        current_w = SHUFFLE_SELF_PS(current_w, _MM_SHUFFLE(3, 3, 3, 3));
        current_x = _mm_mul_ps(current_x, lhs_column_0);
        current_y = _mm_mul_ps(current_y, lhs_column_1);
        current_z = _mm_mul_ps(current_z, lhs_column_2);
        current_w = _mm_mul_ps(current_w, lhs_column_3);

        current_x = _mm_add_ps(current_x, current_y);
        current_z = _mm_add_ps(current_z, current_w);
        _mm_store_ps(result.GetColumnHead<3>(), _mm_add_ps(current_x, current_z));

        return result;
    }
    ENABLE_WHEN_ALIGNAS_1_ARGS(16)
    VectorType operator*(const VectorType& rhs) const
    {
        VectorType result{};

        __m128 lhs_row_0 = _mm_load_ps(this->template GetColumnHead<0>());
        __m128 lhs_row_1 = _mm_load_ps(this->template GetColumnHead<1>());
        __m128 lhs_row_2 = _mm_load_ps(this->template GetColumnHead<2>());
        __m128 lhs_row_3 = _mm_load_ps(this->template GetColumnHead<3>());
        //使用current_w作为缓存
        //手动循环展开，第一轮
        __m128 current_w = _mm_load_ps(rhs.GetHead());
        __m128 current_x = SHUFFLE_SELF_PS(current_w, _MM_SHUFFLE(0, 0, 0, 0));
        __m128 current_y = SHUFFLE_SELF_PS(current_w, _MM_SHUFFLE(1, 1, 1, 1));
        __m128 current_z = SHUFFLE_SELF_PS(current_w, _MM_SHUFFLE(2, 2, 2, 2));
        current_w = SHUFFLE_SELF_PS(current_w, _MM_SHUFFLE(3, 3, 3, 3));
        current_x = _mm_mul_ps(current_x, lhs_row_0);
        current_y = _mm_mul_ps(current_y, lhs_row_1);
        current_z = _mm_mul_ps(current_z, lhs_row_2);
        current_w = _mm_mul_ps(current_w, lhs_row_3);

        current_x = _mm_add_ps(current_x, current_y);
        current_z = _mm_add_ps(current_z, current_w);

        _mm_store_ps(result.GetHead(), _mm_add_ps(current_x, current_z));
        return result;
    }
};

#define MAKE_VECTOR_AND_MATRIX_TYPE(align) \
using Vector2fAlignas##align = Vector2f<align>; \
using Point2fAlignas##align = Vector2fAlignas##align; \
using Vector3fAlignas##align = Vector3f<align>; \
using Vector4fAlignas##align = Vector4f<align>; \
using Matrix2fAlignas##align = Matrix2f<align>; \
using Matrix3fAlignas##align = Matrix3f<align>; \
using Matrix4fAlignas##align = Matrix4f<align>; \

using Vector2fAlignasDefault = Vector2f<>;
using Point2fAlignasDefault = Vector2fAlignasDefault;
using Vector3fAlignasDefault = Vector3f<>;
using Vector4fAlignasDefault = Vector4f<>;
using Matrix2fAlignasDefault = Matrix2f<>;
using Matrix3fAlignasDefault = Matrix3f<>;
using Matrix4fAlignasDefault = Matrix4f<>;

MAKE_VECTOR_AND_MATRIX_TYPE(16)
MAKE_VECTOR_AND_MATRIX_TYPE(32)

#endif // NERAL_ENGINE_COMMON_VECTORS_HPP
