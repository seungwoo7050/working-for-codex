# Snapshot 005: Materials Basic (v1.0.6-1.0.8)

## 패치 범위
- 1.0.6: Lambertian Material
- 1.0.7: Metal Material
- 1.0.8: Dielectric Material

## 포함된 기능
- Material 인터페이스
- Lambertian: 확산 반사 (난반사)
- Metal: 금속 반사 (정반사 + fuzz)
- Dielectric: 굴절 (유리, 물)
  - Snell의 법칙
  - Schlick 근사

## 이전 스냅샷 대비 변화
- materials/ 디렉토리 추가
- material.h, lambertian.h, metal.h, dielectric.h 추가
- HitRecord에 mat 포인터 추가

## 빌드/테스트 상태
✅ 빌드 가능
✅ 테스트 가능

## 다음 단계
- Anti-aliasing 추가
