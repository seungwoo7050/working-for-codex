# Snapshot 011: PDF Basic (v3.0.1-3.0.2)

## 패치 범위
- 3.0.1: PDF 기본 클래스
- 3.0.2: CosinePDF

## 포함된 기능
- PDF 인터페이스
  - value(): 방향의 PDF 값 계산
  - generate(): PDF에 따른 랜덤 방향 생성
- CosinePDF: 코사인 가중 샘플링
  - Lambertian 표면에 적합
  - p(ω) = cos(θ) / π
- SpherePDF: 균일 구면 샘플링
- ONB (정규 직교 기저) 시스템

## 이전 스냅샷 대비 변화
- utils/pdf.h 추가
- utils/onb.h 추가
- randomCosineDirection() 함수 추가

## 빌드/테스트 상태
✅ 빌드 가능
✅ 테스트 가능

## 다음 단계
- HittablePDF 추가
- MixturePDF 구현
