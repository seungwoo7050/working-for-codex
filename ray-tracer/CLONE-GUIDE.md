# 📒 ray-tracer (ray-tracer/) 클론코딩 가이드

> Monte Carlo 레이 트레이서 - 그래픽스 프로그래밍 입문

## v0.1.0: Initial Design (0.5일)

> 📅 **권장 기간**: 2025년 4월 16일

### 📖 사전 학습
```
📚 읽어야 할 Prerequisite
├── prerequisite/c++/01-cpp17-server-ffmpeg-minimum-basics.md
│   └── C++17 기본 문법
└── prerequisite/c++/03-cpp17-cmake-project-structure-debugging.md
    └── CMake, 프로젝트 구조
```

### 📋 작업 내용
| 순서 | 작업 | Design 문서 | 핵심 파일 |
|------|------|-------------|----------|
| 0.1.1 | CMakeLists.txt 설정 | `design/0.0-initial-design.md` | `CMakeLists.txt` |
| 0.1.2 | 디렉토리 구조 | 〃 | `src/`, `tests/` |
| 0.1.3 | PPM 출력 테스트 | 〃 | `src/main.cpp` |

### 🔖 커밋 포인트
```bash
# [📅 2025-04-16 20:00:00]
GIT_AUTHOR_DATE="2025-04-16 20:00:00" GIT_COMMITTER_DATE="2025-04-16 20:00:00" \
git commit -m "chore: initialize ray-tracer project with CMake"
GIT_COMMITTER_DATE="2025-04-16 20:15:00" git tag -a v0.1.0 -m "Project setup"
```

---

## v1.0.0: Basic Raytracer (3일)

> 📅 **권장 기간**: 2025년 4월 17일 ~ 4월 24일

### 📖 사전 학습
```
📚 읽어야 할 Prerequisite (수학 집중)
├── prerequisite/c++/graphics/G01-vector-math-fundamentals.md
│   └── Vec3, 내적, 외적, 정규화 ⭐⭐
├── prerequisite/c++/graphics/G02-ray-intersection.md
│   └── Ray-Sphere 교차, 법선 벡터 ⭐⭐
└── prerequisite/c++/graphics/G03-materials-light.md
    └── 반사, 굴절, Fresnel ⭐
```

### 📋 작업 내용
| 순서 | 작업 | Design 문서 | 핵심 파일 |
|------|------|-------------|----------|
| 1.0.1 | Vec3 클래스 | `design/v1.0.0-basic-raytracer.md` | `src/core/vec3.h` |
| 1.0.2 | Ray 클래스 | 〃 | `src/core/ray.h` |
| 1.0.3 | Sphere Hittable | 〃 | `src/geometry/sphere.h` |
| 1.0.4 | HittableList | 〃 | `src/geometry/hittable_list.h` |
| 1.0.5 | Camera 클래스 | 〃 | `src/core/camera.h` |
| 1.0.6 | Lambertian Material | 〃 | `src/materials/lambertian.h` |
| 1.0.7 | Metal Material | 〃 | `src/materials/metal.h` |
| 1.0.8 | Dielectric Material | 〃 | `src/materials/dielectric.h` |
| 1.0.9 | Anti-aliasing | 〃 | `src/core/camera.h` |

### 🔖 커밋 포인트
```bash
# [📅 2025-04-17 21:00:00]
GIT_AUTHOR_DATE="2025-04-17 21:00:00" GIT_COMMITTER_DATE="2025-04-17 21:00:00" \
git commit -m "feat(core): implement Vec3 with operator overloading"

# [📅 2025-04-18 19:30:00]
GIT_AUTHOR_DATE="2025-04-18 19:30:00" GIT_COMMITTER_DATE="2025-04-18 19:30:00" \
git commit -m "feat(core): add Ray class"

# [📅 2025-04-19 15:00:00]
GIT_AUTHOR_DATE="2025-04-19 15:00:00" GIT_COMMITTER_DATE="2025-04-19 15:00:00" \
git commit -m "feat(geometry): implement Sphere with ray intersection"

# [📅 2025-04-20 16:30:00]
GIT_AUTHOR_DATE="2025-04-20 16:30:00" GIT_COMMITTER_DATE="2025-04-20 16:30:00" \
git commit -m "feat(core): add Camera with viewport and ray generation"

# [📅 2025-04-21 20:00:00]
GIT_AUTHOR_DATE="2025-04-21 20:00:00" GIT_COMMITTER_DATE="2025-04-21 20:00:00" \
git commit -m "feat(materials): implement Lambertian diffuse material"

# [📅 2025-04-22 21:15:00]
GIT_AUTHOR_DATE="2025-04-22 21:15:00" GIT_COMMITTER_DATE="2025-04-22 21:15:00" \
git commit -m "feat(materials): add Metal with reflection"

# [📅 2025-04-23 19:45:00]
GIT_AUTHOR_DATE="2025-04-23 19:45:00" GIT_COMMITTER_DATE="2025-04-23 19:45:00" \
git commit -m "feat(materials): implement Dielectric with refraction"

# [📅 2025-04-24 21:00:00]
GIT_AUTHOR_DATE="2025-04-24 21:00:00" GIT_COMMITTER_DATE="2025-04-24 21:00:00" \
git commit -m "feat(core): add anti-aliasing with multi-sampling"
GIT_COMMITTER_DATE="2025-04-24 21:15:00" git tag -a v1.0.0 -m "Basic raytracer with materials"
```

### ✅ 완료 기준
- [ ] 구체 렌더링 (diffuse + specular)
- [ ] 반사/굴절 동작
- [ ] Anti-aliasing 적용

---

## v2.0.0: Advanced Features (3일)

> 📅 **권장 기간**: 2025년 4월 25일 ~ 5월 3일

### 📖 사전 학습
```
📚 읽어야 할 Prerequisite
├── prerequisite/c++/graphics/G04-bvh-acceleration.md
│   └── BVH, AABB, 공간 분할 ⭐⭐
└── prerequisite/c++/graphics/G05-textures-uv.md
    └── UV 매핑, 이미지 텍스처
```

### 📋 작업 내용
| 순서 | 작업 | Design 문서 | 핵심 파일 |
|------|------|-------------|----------|
| 2.0.1 | AABB 클래스 | `design/v2.0.0-advanced-features.md` | `src/geometry/aabb.h` |
| 2.0.2 | BVH 노드 | 〃 | `src/geometry/bvh_node.h` |
| 2.0.3 | SolidColor Texture | 〃 | `src/textures/solid_color.h` |
| 2.0.4 | Checker Texture | 〃 | `src/textures/checker.h` |
| 2.0.5 | Image Texture | 〃 | `src/textures/image_texture.h` |
| 2.0.6 | Quad Primitive | 〃 | `src/geometry/quad.h` |
| 2.0.7 | Motion Blur | 〃 | `src/core/camera.h` |
| 2.0.8 | Volume (Fog) | 〃 | `src/geometry/constant_medium.h` |

### 🔖 커밋 포인트
```bash
# [📅 2025-04-26 14:00:00]
GIT_AUTHOR_DATE="2025-04-26 14:00:00" GIT_COMMITTER_DATE="2025-04-26 14:00:00" \
git commit -m "feat(geometry): implement AABB for bounding volumes"

# [📅 2025-04-27 16:30:00]
GIT_AUTHOR_DATE="2025-04-27 16:30:00" GIT_COMMITTER_DATE="2025-04-27 16:30:00" \
git commit -m "feat(geometry): add BVH tree for ray acceleration"

# [📅 2025-04-28 20:00:00]
GIT_AUTHOR_DATE="2025-04-28 20:00:00" GIT_COMMITTER_DATE="2025-04-28 20:00:00" \
git commit -m "feat(textures): implement solid color and checker textures"

# [📅 2025-04-30 21:15:00]
GIT_AUTHOR_DATE="2025-04-30 21:15:00" GIT_COMMITTER_DATE="2025-04-30 21:15:00" \
git commit -m "feat(textures): add image texture with stb_image"

# [📅 2025-05-01 15:00:00]
GIT_AUTHOR_DATE="2025-05-01 15:00:00" GIT_COMMITTER_DATE="2025-05-01 15:00:00" \
git commit -m "feat(geometry): implement Quad primitive"

# [📅 2025-05-02 19:30:00]
GIT_AUTHOR_DATE="2025-05-02 19:30:00" GIT_COMMITTER_DATE="2025-05-02 19:30:00" \
git commit -m "feat(core): add motion blur with time-based ray"

# [📅 2025-05-03 17:00:00]
GIT_AUTHOR_DATE="2025-05-03 17:00:00" GIT_COMMITTER_DATE="2025-05-03 17:00:00" \
git commit -m "feat(geometry): implement constant medium for volumetric fog"
GIT_COMMITTER_DATE="2025-05-03 17:15:00" git tag -a v2.0.0 -m "Advanced features: BVH, textures, motion blur"
```

### ✅ 완료 기준
- [ ] BVH 활성화 시 렌더링 속도 10x+ 향상
- [ ] 체커/이미지 텍스처 매핑
- [ ] 모션 블러 효과
- [ ] 볼류메트릭 안개

---

## v3.0.0: Monte Carlo Integration (2일)

> 📅 **권장 기간**: 2025년 5월 4일 ~ 5월 10일

### 📖 사전 학습
```
📚 읽어야 할 Prerequisite
├── prerequisite/c++/graphics/G06-monte-carlo-basics.md
│   └── 확률, 적분, 기대값 ⭐⭐
└── prerequisite/c++/graphics/G07-importance-sampling.md
    └── PDF, 중요도 샘플링 ⭐⭐
```

### 📋 작업 내용
| 순서 | 작업 | Design 문서 | 핵심 파일 |
|------|------|-------------|----------|
| 3.0.1 | PDF 기본 클래스 | `design/v3.0.0-monte-carlo-integration.md` | `src/pdf/pdf.h` |
| 3.0.2 | CosinePDF | 〃 | `src/pdf/cosine_pdf.h` |
| 3.0.3 | HittablePDF | 〃 | `src/pdf/hittable_pdf.h` |
| 3.0.4 | MixturePDF | 〃 | `src/pdf/mixture_pdf.h` |
| 3.0.5 | DiffuseLight | 〃 | `src/materials/diffuse_light.h` |
| 3.0.6 | Importance Sampling | 〃 | `src/core/renderer.cpp` |

### 🔖 커밋 포인트
```bash
# [📅 2025-05-05 20:00:00]
GIT_AUTHOR_DATE="2025-05-05 20:00:00" GIT_COMMITTER_DATE="2025-05-05 20:00:00" \
git commit -m "feat(pdf): implement PDF base class and CosinePDF"

# [📅 2025-05-06 21:30:00]
GIT_AUTHOR_DATE="2025-05-06 21:30:00" GIT_COMMITTER_DATE="2025-05-06 21:30:00" \
git commit -m "feat(pdf): add HittablePDF for light sampling"

# [📅 2025-05-07 19:45:00]
GIT_AUTHOR_DATE="2025-05-07 19:45:00" GIT_COMMITTER_DATE="2025-05-07 19:45:00" \
git commit -m "feat(pdf): implement MixturePDF for combined sampling"

# [📅 2025-05-09 20:15:00]
GIT_AUTHOR_DATE="2025-05-09 20:15:00" GIT_COMMITTER_DATE="2025-05-09 20:15:00" \
git commit -m "feat(materials): add DiffuseLight emissive material"

# [📅 2025-05-10 16:00:00]
GIT_AUTHOR_DATE="2025-05-10 16:00:00" GIT_COMMITTER_DATE="2025-05-10 16:00:00" \
git commit -m "feat(core): implement importance sampling in renderer"
GIT_COMMITTER_DATE="2025-05-10 16:15:00" git tag -a v3.0.0 -m "Monte Carlo importance sampling"
```

### ✅ 완료 기준
- [ ] 동일 샘플 수에서 노이즈 50% 감소
- [ ] 광원 중요도 샘플링 동작
- [ ] Cornell Box 렌더링