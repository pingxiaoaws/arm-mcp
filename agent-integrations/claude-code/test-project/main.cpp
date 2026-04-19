#include <iostream>
#include <immintrin.h>  // x86 SSE/AVX intrinsics
#include <cstring>
#include <vector>

// x86-specific SIMD function
void vector_add_sse(float* a, float* b, float* result, int n) {
    for (int i = 0; i < n; i += 4) {
        __m128 va = _mm_loadu_ps(&a[i]);
        __m128 vb = _mm_loadu_ps(&b[i]);
        __m128 vr = _mm_add_ps(va, vb);
        _mm_storeu_ps(&result[i], vr);
    }
}

// x86 AVX2 dot product
float dot_product_avx2(float* a, float* b, int n) {
    __m256 sum = _mm256_setzero_ps();
    for (int i = 0; i < n; i += 8) {
        __m256 va = _mm256_loadu_ps(&a[i]);
        __m256 vb = _mm256_loadu_ps(&b[i]);
        sum = _mm256_fmadd_ps(va, vb, sum);
    }
    // horizontal sum
    __m128 hi = _mm256_extractf128_ps(sum, 1);
    __m128 lo = _mm256_castps256_ps128(sum);
    __m128 s = _mm_add_ps(hi, lo);
    s = _mm_hadd_ps(s, s);
    s = _mm_hadd_ps(s, s);
    return _mm_cvtss_f32(s);
}

int main() {
    const int N = 1024;
    std::vector<float> a(N, 1.0f), b(N, 2.0f), result(N);
    
    vector_add_sse(a.data(), b.data(), result.data(), N);
    std::cout << "SSE add result[0]: " << result[0] << std::endl;
    
    float dp = dot_product_avx2(a.data(), b.data(), N);
    std::cout << "AVX2 dot product: " << dp << std::endl;
    
    return 0;
}
