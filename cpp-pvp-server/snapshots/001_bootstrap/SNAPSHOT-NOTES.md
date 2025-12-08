# 스냅샷 001: Bootstrap (v0.1.0)

## 패치 범위
- 0.1.1: CMakeLists.txt 설정
- 0.1.2: vcpkg.json 의존성 정의
- 0.1.3: 디렉토리 구조 생성
- 0.1.4: 빌드 스크립트 (이 스냅샷에서는 생략)

## 포함 기능
- CMake 빌드 시스템 (v3.20+)
- vcpkg 의존성 관리 (boost-system, boost-asio, boost-beast, gtest)
- 기본 프로젝트 구조 (src/, include/)
- "Hello cpp-pvp-server" 출력 프로그램

## 빌드 방법
```bash
cd server
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake
cmake --build .
./pvpserver
```

## 완료 기준
- [x] CMake 빌드 성공
- [x] vcpkg 의존성 설치 (boost)
- [x] main.cpp "Hello cpp-pvp-server" 출력

## 빌드 가능 여부
✅ 빌드 가능
