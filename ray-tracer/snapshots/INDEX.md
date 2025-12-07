# Ray Tracer 스냅샷 인덱스

> Monte Carlo 레이 트레이서 - 단계별 스냅샷 코드 목록

이 문서는 CLONE-GUIDE.md의 패치 단위를 기준으로 생성된 스냅샷 디렉토리들을 정리합니다.
각 스냅샷은 해당 패치 시점까지의 코드만 포함하며, 독립적으로 빌드/테스트가 가능합니다.

---

## 스냅샷 목록

| 스냅샷 | 대응 패치 | 버전 | 주요 포함 기능 | 빌드 | 테스트 |
|--------|----------|------|---------------|------|--------|
| [`001_cmake_setup/`](./001_cmake_setup/) | 0.1.1~0.1.3 | v0.1.0 | CMake 설정, PPM 출력 | ✅ | - |
| [`002_vec3_ray/`](./002_vec3_ray/) | 1.0.1~1.0.2 | v1.0 (일부) | Vec3, Ray 클래스 | ✅ | ✅ |
| [`003_sphere_hittable/`](./003_sphere_hittable/) | 1.0.3~1.0.4 | v1.0 (일부) | Sphere, HittableList | ✅ | ✅ |
| [`004_camera_basic/`](./004_camera_basic/) | 1.0.5 | v1.0 (일부) | Camera 기본 | ✅ | ✅ |
| [`005_materials_basic/`](./005_materials_basic/) | 1.0.6~1.0.8 | v1.0 (일부) | Lambertian, Metal, Dielectric | ✅ | ✅ |
| [`006_antialiasing/`](./006_antialiasing/) | 1.0.9 | **v1.0.0** | Anti-aliasing, Defocus Blur | ✅ | ✅ |
| [`007_aabb_bvh/`](./007_aabb_bvh/) | 2.0.1~2.0.2 | v2.0 (일부) | AABB, BVHNode | ✅ | ✅ |
| [`008_textures/`](./008_textures/) | 2.0.3~2.0.5 | v2.0 (일부) | SolidColor, Checker, Noise, Image Texture | ✅ | ✅ |
| [`009_quad_motion/`](./009_quad_motion/) | 2.0.6~2.0.7 | v2.0 (일부) | Quad, MovingSphere, Motion Blur | ✅ | ✅ |
| [`010_volume/`](./010_volume/) | 2.0.8 | **v2.0.0** | ConstantMedium, DiffuseLight | ✅ | ✅ |
| [`011_pdf_basic/`](./011_pdf_basic/) | 3.0.1~3.0.2 | v3.0 (일부) | PDF 인터페이스, CosinePDF | ✅ | ✅ |
| [`012_pdf_hittable/`](./012_pdf_hittable/) | 3.0.3~3.0.4 | v3.0 (일부) | HittablePDF, MixturePDF | ✅ | ✅ |
| [`013_diffuse_light/`](./013_diffuse_light/) | 3.0.5 | v3.0 (일부) | 광원 중요도 샘플링 | ✅ | ✅ |

> **참고:** v3.0.0 최종 버전(3.0.6: Importance Sampling)은 현재 프로젝트 루트의 코드 상태이므로 스냅샷을 생성하지 않습니다.

---

## 버전별 마일스톤

### v0.1.0: Initial Setup
- **스냅샷:** `001_cmake_setup`
- **주요 내용:** CMake 프로젝트 초기화, PPM 이미지 출력

### v1.0.0: Basic Raytracer
- **스냅샷:** `002_vec3_ray` ~ `006_antialiasing`
- **주요 내용:**
  - Vec3, Ray, Camera 핵심 클래스
  - Sphere 프리미티브
  - Lambertian, Metal, Dielectric 재질
  - Anti-aliasing, Defocus Blur

### v2.0.0: Advanced Features
- **스냅샷:** `007_aabb_bvh` ~ `010_volume`
- **주요 내용:**
  - BVH 가속 구조 (O(log N) 교차 판정)
  - 텍스처 시스템 (Solid, Checker, Perlin Noise)
  - Quad, MovingSphere, ConstantMedium
  - DiffuseLight 광원

### v3.0.0: Monte Carlo Integration
- **스냅샷:** `011_pdf_basic` ~ `013_diffuse_light`
- **최종 상태:** 프로젝트 루트
- **주요 내용:**
  - PDF 시스템 (CosinePDF, HittablePDF, MixturePDF)
  - ONB (정규 직교 기저)
  - 중요도 샘플링
  - 분산 4~5배 감소

---

## 스냅샷 사용 방법

각 스냅샷 디렉토리는 독립적인 프로젝트입니다:

```bash
cd snapshots/006_antialiasing
mkdir build && cd build
cmake ..
make

# 렌더링
./raytracer > output.ppm

# 테스트 실행
./unit_tests
```

---

## 파일 구조

```
snapshots/
├── INDEX.md                 # 이 파일
├── 001_cmake_setup/
│   ├── CMakeLists.txt
│   ├── src/main.cpp
│   └── SNAPSHOT-NOTES.md
├── 002_vec3_ray/
│   ├── CMakeLists.txt
│   ├── src/
│   │   ├── main.cpp
│   │   ├── core/
│   │   │   ├── vec3.h
│   │   │   └── ray.h
│   │   └── utils/common.h
│   ├── tests/unit/
│   │   ├── test_vec3.cpp
│   │   └── test_ray.cpp
│   └── SNAPSHOT-NOTES.md
├── ...
└── 013_diffuse_light/
    └── ...
```

---

## SNAPSHOT-NOTES.md 포맷

각 스냅샷에는 `SNAPSHOT-NOTES.md` 파일이 포함되어 다음 정보를 제공합니다:
- 패치 범위
- 포함된 기능
- 빌드 방법
- 빌드/테스트 상태
- 제외된 기능 (이후 버전에서 추가)
- 다음 단계

---

## 문서와의 관계

| 스냅샷 범위 | 관련 Design 문서 |
|------------|-----------------|
| 001 | `design/0.0-initial-design.md` |
| 002~006 | `design/v1.0.0-basic-raytracer.md` |
| 007~010 | `design/v2.0.0-advanced-features.md` |
| 011~013 | `design/v3.0.0-monte-carlo-integration.md` |

---

## 생성 일자

- 2025년 12월 7일
