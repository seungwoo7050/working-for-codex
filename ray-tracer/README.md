# Ray Tracer

![CI Status](https://github.com/seungwoo7050/study-ray-tracer/workflows/Ray%20Tracer%20CI/badge.svg)

> 테스트/CI: Google Test 기반 단위 테스트를 `ctest`로 실행하며 GitHub Actions(`.github/workflows/ci.yml`)에서 CMake 구성 후 자동화됩니다. 포트폴리오 요약은 [PORTFOLIO.md](../PORTFOLIO.md)에서 확인 가능합니다.

Peter Shirley의 ["Ray Tracing in One Weekend"](https://raytracing.github.io/) 시리즈를 C++17로 완전 구현한 프로젝트입니다. Monte Carlo 적분과 중요도 샘플링을 포함한 고급 렌더링 기법을 학습합니다.

---

## 프로젝트 개요

### 목표
컴퓨터 그래픽스의 핵심인 **레이트레이싱(Ray Tracing)** 알고리즘을 처음부터 구현하며, 물리 기반 렌더링(PBR)의 수학적 원리와 최적화 기법을 학습합니다.

### 기술 스택
| 분류 | 기술 |
|------|------|
| **언어** | C++17 |
| **빌드** | CMake 3.20+ |
| **테스트** | Google Test |
| **CI/CD** | GitHub Actions |
| **출력** | PPM 이미지 포맷 |

---

## 구현 단계

### v1.0.0: Ray Tracing in One Weekend ✅

기본적인 레이트레이서의 핵심 구조를 구현합니다.

**구현 내용:**
- 벡터 수학 (Vec3 클래스)
- Ray 클래스 (광선 시뮬레이션)
- 카메라 시스템 (FOV 조절 가능)
- Sphere 프리미티브 (광선-구 교차)
- 재질 시스템:
  - Lambertian (확산 표면)
  - Metal (퍼지니스 지원 금속)
  - Dielectric (굴절 유리)
- 안티앨리어싱 (멀티샘플링)
- Defocus blur (피사계 심도)

### v2.0.0: Ray Tracing: The Next Week ✅

고급 기능과 성능 최적화를 추가합니다.

**구현 내용:**
- **BVH (Bounding Volume Hierarchy)**: 가속 구조
- **텍스처 시스템**:
  - 단색 텍스처
  - 체커 텍스처
  - Perlin 노이즈 텍스처
  - 이미지 텍스처
- **Motion blur**: 움직이는 구체
- **사각형 프리미티브 (Quad)**
- **조명 시스템**: 발광 재질
- **인스턴스**: 이동/회전 변환
- **볼륨**: 일정 밀도 매체 (안개, 연기)
- 종합 단위 테스트
- 성능 벤치마크

### v3.0.0: Ray Tracing: The Rest of Your Life ✅

Monte Carlo 적분과 중요도 샘플링으로 렌더링 품질을 극대화합니다.

**구현 내용:**
- **Monte Carlo 적분**: 중요도 샘플링
- **PDF (확률 밀도 함수)**:
  - CosinePDF: 확산 표면용 코사인 가중 샘플링
  - SpherePDF: 균일 구 샘플링
  - HittablePDF: 광원 방향 샘플링
  - MixturePDF: 최적 분산 감소를 위한 혼합 PDF
- **Orthonormal Basis (ONB)**: 좌표 변환
- **확장된 재질 시스템**:
  - PDF 기반 산란 (Lambertian)
  - Specular 처리 (Metal/Dielectric)
  - ScatterRecord 구조체
- **지오메트리별 중요도 샘플링**:
  - Sphere 중요도 샘플링
  - Quad 면적 광원 샘플링
  - HittableList 가중 샘플링
- **고급 카메라**:
  - 이중 렌더 모드 (표준/중요도 샘플링)
  - 명시적 광원 처리
  - Mixture PDF 평가

---

## v3.0.0 핵심 알고리즘

### Monte Carlo 적분

렌더링 방정식:
$$L_o = L_e + \int f(\omega_i) \cdot L_i(\omega_i) \cdot \cos(\theta_i) \, d\omega_i$$

- $L_o$: 나가는 복사휘도
- $L_e$: 방출 복사휘도
- $f(\omega_i)$: BRDF (산란 함수)
- $L_i(\omega_i)$: 들어오는 복사휘도
- $\cos(\theta_i)$: 코사인 항

### 중요도 샘플링

균일 샘플링 대신 PDF $p(\omega)$에 따라 샘플링:
$$L_o \approx L_e + \frac{1}{N} \sum \frac{f(\omega_i) \cdot L_i(\omega_i) \cdot \cos(\theta_i)}{p(\omega_i)}$$

**장점:**
- $p(\omega)$가 피적분 함수에 비례할 때 분산 감소
- 정확한 결과로 빠른 수렴
- 최종 이미지 노이즈 감소

### Mixture 샘플링

최적의 결과를 위해 두 PDF를 혼합:
$$p_{mixed}(\omega) = 0.5 \cdot p_{material}(\omega) + 0.5 \cdot p_{light}(\omega)$$

이를 통해 다음 두 방향을 모두 샘플링:
1. 재질이 잘 산란하는 방향 (코사인 가중)
2. 광원을 향하는 방향

### 렌더링 알고리즘 (중요도 샘플링)

```cpp
Color rayColor(ray, depth, world, lights):
  if depth <= 0: return black

  rec = world.hit(ray)
  if !hit: return background

  emission = rec.material.emitted()
  srec = rec.material.scatter(ray, rec)
  if !srec: return emission

  if srec.skipPdf:  // Specular 재질
    return srec.attenuation * rayColor(srec.skipPdfRay, depth-1)

  // 중요도 샘플링
  light_pdf = HittablePDF(lights, rec.point)
  mixed_pdf = MixturePDF(srec.pdfPtr, light_pdf)

  scattered = Ray(rec.point, mixed_pdf.generate())
  pdf_val = mixed_pdf.value(scattered.direction)
  scattering_pdf = rec.material.scatteringPdf(ray, rec, scattered)

  return emission +
         srec.attenuation * scattering_pdf *
         rayColor(scattered, depth-1) / pdf_val
```

---

## 성능 분석

### BVH 가속 (Phase 2)
| 방식 | 시간 복잡도 |
|------|------------|
| BVH 미사용 | O(n) |
| BVH 사용 | O(log n) |

### 중요도 샘플링 (Phase 3)

| 씬 | Phase 2 노이즈 | Phase 3 노이즈 | 개선율 |
|----|---------------|---------------|--------|
| Cornell Box (100 spp) | 높음 | 낮음 | ~5배 감소 |
| Cornell Box (200 spp) | 중간 | 매우 낮음 | ~4배 감소 |

**수렴 속도:**
- Phase 2: 400 샘플 → 수용 가능한 품질
- Phase 3: 100 샘플 → 동등한 품질

---

## 빌드 및 실행

### 요구사항
- CMake 3.20 이상
- C++17 호환 컴파일러 (GCC 7+ / Clang 5+)

### 빌드
```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -j$(nproc)
```

### 실행
```bash
./raytracer > output/image.ppm
```

> **참고**: 렌더링은 하드웨어와 샘플 수에 따라 수 분이 소요될 수 있습니다.

### 씬 선택

`src/main.cpp`의 `sceneChoice` 변수를 수정하여 씬을 선택합니다:

| 번호 | 씬 | 설명 |
|------|-----|------|
| 1 | Random Spheres | Motion blur, 체커 텍스처, BVH |
| 2 | Two Spheres | 체커 텍스처 데모 |
| 3 | Earth | Perlin 노이즈 텍스처 |
| 4 | Simple Light | 발광 재질과 조명 |
| 5 | **Cornell Box** (기본) | Phase 3 중요도 샘플링 |
| 6 | Cornell Smoke | 볼륨 (안개/연기) 데모 |

### 이미지 보기

PPM 파일 뷰어:
- **Linux**: `display image.ppm` (ImageMagick) 또는 `eog image.ppm`
- **macOS**: `open image.ppm`
- **Windows**: IrfanView 또는 PNG로 변환

```bash
# PNG 변환 (ImageMagick 필요)
convert output/image.ppm output/image.png
```

### 품질 설정

`src/main.cpp`에서 수정:

```cpp
// 빠른 미리보기 (저품질)
cam.imageWidth = 200;
cam.samplesPerPixel = 5;

// 고품질 (느림)
cam.imageWidth = 1200;
cam.samplesPerPixel = 100;
```

---

## 테스트

### 테스트 실행
```bash
cd build
ctest --output-on-failure
```

또는 직접 실행:
```bash
./unit_tests
```

### 벤치마크 실행
```bash
./benchmark_bvh
```

### 테스트 항목 (총 69개)
- **Vec3**: 벡터 연산, 내적/외적, 정규화
- **Ray**: 광선 생성 및 점 평가
- **Sphere**: 광선-구 교차, 법선 계산
- **Material**: 모든 재질 유형의 산란 동작
- **BVH**: AABB 생성, 바운딩 박스, BVH 히트
- **Texture**: 단색, 체커, 노이즈 텍스처 샘플링
- **Geometry**: Quad, 움직이는 구, 이동, 회전
- **ONB**: 정규직교기저 생성 및 직교성
- **PDF**: PDF 정규화, 중요도 샘플링, 수렴

---

## 프로젝트 구조

```
ray-tracer/
├── src/
│   ├── core/          # 핵심 클래스 (Vec3, Ray, Camera, Color)
│   ├── geometry/      # 기하 프리미티브 (Sphere, Quad, BVH 등)
│   ├── materials/     # 재질 클래스 (Lambertian, Metal 등)
│   ├── textures/      # 텍스처 클래스 (Solid, Checker, Noise 등)
│   ├── utils/         # 유틸리티 함수, ONB, PDF
│   └── main.cpp       # 메인 프로그램 및 데모 씬
├── tests/
│   ├── unit/          # 단위 테스트
│   └── benchmark/     # 성능 벤치마크
├── design/            # 설계 문서
├── output/            # 생성된 이미지
├── CMakeLists.txt     # 빌드 설정
└── README.md
```

---

## CI/CD

GitHub Actions를 사용한 지속적 통합:
- **빌드 & 테스트**: Ubuntu, macOS에서 실행
- **코드 품질**: Clang-format 검사
- **아티팩트**: 샘플 이미지 업로드 (30일 보관)

---

## 학습 포인트

### 컴퓨터 그래픽스
- 광선-물체 교차 알고리즘
- 물리 기반 렌더링 (PBR) 원리
- BRDF와 재질 모델링
- 공간 분할 자료구조 (BVH)

### 수학적 기초
- 벡터/행렬 연산
- Monte Carlo 적분
- 확률 밀도 함수 (PDF)
- 중요도 샘플링 이론

### 소프트웨어 엔지니어링
- 모던 C++ (C++17) 활용
- 객체지향 설계 패턴
- 테스트 주도 개발 (TDD)
- 성능 최적화 기법

---

## 포트폴리오 가치

### 기술 역량 증명
- **저수준 프로그래밍**: C++로 렌더링 파이프라인 직접 구현
- **수학적 사고**: Monte Carlo 적분, 확률 이론 실제 적용
- **최적화 능력**: BVH로 O(n) → O(log n) 성능 개선, 중요도 샘플링으로 4-5배 분산 감소
- **체계적 개발**: 3단계 점진적 구현, 69개 단위 테스트

### 적용 분야
- 게임 엔진 개발
- 영화/애니메이션 VFX
- 시뮬레이션 소프트웨어
- GPU 프로그래밍 (CUDA/OptiX 확장 가능)

---

## 핵심 인사이트

1. **작은 광원에서 중요도 샘플링은 필수**
   - 광원 직접 샘플링이 분산을 극적으로 감소
   - Cornell Box가 이를 명확히 증명

2. **Mixture PDF로 탐색과 활용의 균형**
   - 재질 PDF: 올바른 재질 반응 보장
   - 광원 PDF: 중요한 곳에 샘플 집중

3. **Specular 재질은 PDF 샘플링 우회**
   - 델타 분포 (거울, 유리)는 샘플링 불가
   - `skipPdf` 플래그로 처리

4. **Lambertian 재질은 코사인 가중 샘플링**
   - 산란광의 자연스러운 분포와 일치
   - 균일 반구 샘플링 대비 분산 감소

---

## 참고 자료

- [Ray Tracing in One Weekend](https://raytracing.github.io/books/RayTracingInOneWeekend.html)
- [Ray Tracing: The Next Week](https://raytracing.github.io/books/RayTracingTheNextWeek.html)
- [Ray Tracing: The Rest of Your Life](https://raytracing.github.io/books/RayTracingTheRestOfYourLife.html)
- [scratchapixel.com](https://www.scratchapixel.com/)
- [Physically Based Rendering Book](https://www.pbr-book.org/)

---

## 설계 문서

상세한 설계 문서는 `design/` 디렉토리를 참조하세요.

---

## 라이선스

이 프로젝트는 교육 목적으로 제작되었습니다.

## 감사의 말

Peter Shirley의 ["Ray Tracing in One Weekend"](https://raytracing.github.io/) 시리즈를 기반으로 합니다.
