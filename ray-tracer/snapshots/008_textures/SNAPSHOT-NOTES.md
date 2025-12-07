# Snapshot 008: Textures (v2.0.3-2.0.5)

## 패치 범위
- 2.0.3: SolidColor Texture
- 2.0.4: Checker Texture
- 2.0.5: Image Texture

## 포함된 기능
- Texture 인터페이스
- SolidColor: 단색 텍스처
- CheckerTexture: 3D 체커보드 패턴
- NoiseTexture: Perlin 노이즈
- ImageTexture: 이미지 텍스처 (placeholder)
- Perlin: 노이즈 생성기
  - trilinear interpolation
  - turbulence

## 이전 스냅샷 대비 변화
- textures/ 디렉토리 추가
- Lambertian이 Texture를 받도록 확장
- HitRecord에 u, v 텍스처 좌표 추가

## 빌드/테스트 상태
✅ 빌드 가능
✅ 테스트 가능

## 다음 단계
- Quad 프리미티브 추가
- Motion Blur 구현
