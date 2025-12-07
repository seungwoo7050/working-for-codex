# Snapshot 010: Volume Rendering (v2.0.0)

## 패치 범위
- 2.0.1 ~ 2.0.8: 고급 기능 및 최적화 전체

## 포함된 기능

### v1.0.0 기능 (모두 포함)
- Vec3, Ray, Camera, Color
- Sphere, HittableList, Hittable
- Lambertian, Metal, Dielectric

### v2.0.0 추가 기능

- **가속 구조**
  - AABB: 축 정렬 바운딩 박스
  - BVHNode: Bounding Volume Hierarchy

- **텍스처 시스템**
  - Texture: 텍스처 인터페이스
  - SolidColor: 단색 텍스처
  - CheckerTexture: 체커보드 패턴
  - NoiseTexture: Perlin 노이즈
  - ImageTexture: 이미지 텍스처 (placeholder)
  - Perlin: 노이즈 생성기

- **고급 프리미티브**
  - Quad: 사각형 평면
  - MovingSphere: Motion Blur용 이동 구체
  - Translate: 객체 이동 인스턴싱
  - RotateY: Y축 회전 인스턴싱
  - ConstantMedium: 볼륨 렌더링 (안개, 연기)

- **광원**
  - DiffuseLight: 발광 재질

- **추가 재질**
  - Isotropic: 등방성 산란 (볼륨용)

- **Camera 확장**
  - time 파라미터 (Motion Blur)
  - emitted() 호출 (광원 지원)

## 빌드 방법
```bash
mkdir build && cd build
cmake ..
make
./raytracer > output.ppm
./unit_tests
./benchmark_bvh
```

## 빌드/테스트 상태
✅ 빌드 가능
✅ 테스트 통과
✅ BVH 벤치마크 실행 가능

## 성능
- BVH 적용 시 500개 객체 기준 ~30배 이상 속도 향상
- Motion Blur, Cornell Box 렌더링 가능

## 제외된 기능 (v3.0.0에서 추가)
- PDF 시스템 (CosinePDF, HittablePDF, MixturePDF)
- ONB 클래스
- ScatterRecord
- 중요도 샘플링

## 다음 단계
- PDF 인터페이스 구현
- 중요도 샘플링 적용
