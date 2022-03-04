/**
 * @author: Dusk
 * @date: 2022-03-04 19:07:21
 * @last modified by:   Dusk
 * @last modified time: 2022-03-04 19:07:21
 * @copyright Copyright (c) 2022 Dusk.
*/
#include "TestMain.h"
#include "../include/Vectors.hpp"
#include <sal.h>
#include <DirectXMath.h>

TEST(Vectors, VectorTest)
{
    DirectX::XMMATRIX l_matrix(
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 1);
    DirectX::XMMATRIX r_matrix(
        1, 0, 0, 100,
        0, 1, 0, 100,
        0, 0, 1, 100,
        0, 0, 0, 1);
    auto xmm_result = DirectX::XMMatrixMultiply(l_matrix, r_matrix);

    Vector4fAlignas16 a{1.0f, 2.0f, 3.0f, 4.0f};
    Vector4fAlignas16 b{2.3f, 2.3f, 5.5f, 2.3f};
    auto cross_result = a.Cross3(b); //Pass
    Vector4fAlignas16 target_result{3.3f, 4.3f, 8.5f, 6.3f};
    auto z = a.y();
    auto c = a + b;
    auto m = std::make_unique<Matrix4fAlignas16>(a, b, a, b);
    Matrix4fAlignas16 m4{
        1, 1, 2, 2,
        3, 3, 4, 4,
        5, 5, 6, 6,
        7, 7, 8, 8};
    Matrix4fAlignas16 m4_1{
        11, 12, 13, 14,
        15, 16, 17, 18,
        19, 20, 21, 22,
        23, 24, 25, 26};
    auto result = m4 * m4_1;
    Matrix4fAlignas16 target_matrix{
        110, 116, 122, 128,
        246, 260, 274, 288,
        382, 404, 426, 448,
        518, 548, 578, 608};
    auto presult_vec = std::make_unique<Vector4fAlignas16>(
        1220, 2740, 4260, 5780);
    EXPECT_EQ(result, target_matrix);
    auto result_vec_calcuate = target_matrix * a;
    EXPECT_EQ(result_vec_calcuate, *(presult_vec));
    auto e{m->GetColumn<1>()};
    auto f{m->GetColumn(0)};
    EXPECT_EQ(e,b);
    EXPECT_EQ(f,a);
    a.SetZ(0.289450f);
    a += b;
}