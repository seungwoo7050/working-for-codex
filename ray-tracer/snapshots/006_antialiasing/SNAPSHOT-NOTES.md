# Snapshot 006: Anti-aliasing Complete (v1.0.0)

## 패치 범위
- 1.0.1 ~ 1.0.9: 기본 레이트레이서 전체

## 포함된 기능
- **Core 클래스**
  - Vec3: 3D 벡터, 점, 색상 통합 타입
  - Ray: 광선 파라메트릭 표현
  - Camera: 가상 카메라 시스템
  - Color: PPM 출력, 감마 보정

- **Geometry**
  - Hittable: 교차 판정 인터페이스
  - HittableList: 객체 목록 관리
  - Sphere: 구체 프리미티브

- **Materials**
  - Lambertian: 확산 반사 (난반사)
  - Metal: 금속 반사 (정반사 + fuzz)
  - Dielectric: 굴절 (유리, 물)

- **렌더링 기법**
  - 역방향 광선 추적 (Backward Tracing)
  - 재귀적 광선 반사/굴절
  - Anti-aliasing (다중 샘플링)
  - Defocus Blur (피사계 심도)
  - 감마 보정 (γ = 2.0)

## 빌드 방법
```bash
mkdir build && cd build
cmake ..
make
./raytracer > output.ppm
./unit_tests
```

## 빌드/테스트 상태
✅ 빌드 가능
✅ 테스트 통과 (Vec3, Ray, Sphere, Materials)
✅ 렌더링 가능 (Three Spheres 씬)

## 제외된 기능 (v2.0.0+에서 추가)
- BVH 가속 구조
- 텍스처 시스템
- Quad/MovingSphere/ConstantMedium
- PDF 시스템

## 다음 단계
- AABB 클래스 추가
- BVH 노드 구현
