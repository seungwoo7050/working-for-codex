# Snapshot 001: CMake Setup (v0.1.0)

## 패치 범위
- 0.1.1: CMakeLists.txt 설정
- 0.1.2: 디렉토리 구조
- 0.1.3: PPM 출력 테스트

## 포함된 기능
- CMake 3.20+ 프로젝트 구성
- C++17 표준 설정
- 기본 컴파일러 플래그
- PPM 이미지 포맷 출력 (256x256 그라데이션)

## 빌드 방법
```bash
mkdir build && cd build
cmake ..
make
./raytracer > output.ppm
```

## 빌드/테스트 상태
✅ 빌드 가능
✅ 실행 가능 (PPM 이미지 생성)

## 다음 단계
- Vec3 클래스 추가
- Ray 클래스 추가
