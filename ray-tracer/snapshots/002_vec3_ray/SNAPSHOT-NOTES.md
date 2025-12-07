# Snapshot 002: Vec3 & Ray Classes (v1.0.1-1.0.2)

## 패치 범위
- 1.0.1: Vec3 클래스
- 1.0.2: Ray 클래스

## 포함된 기능
- Vec3 클래스 (3D 벡터/점/색상)
  - 기본 연산: +, -, *, /
  - 내적(dot), 외적(cross)
  - 정규화(unitVector)
  - 유틸리티: nearZero, random
- Ray 클래스
  - origin, direction
  - at(t) 파라메트릭 위치 계산
- common.h 유틸리티

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
✅ 테스트 통과 (Vec3, Ray 단위 테스트)

## 다음 단계
- Sphere Hittable 추가
- HittableList 구현
