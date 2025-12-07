# Snapshot 012: HittablePDF & MixturePDF (v3.0.3-3.0.4)

## 패치 범위
- 3.0.3: HittablePDF
- 3.0.4: MixturePDF

## 포함된 기능
- HittablePDF: 특정 객체(광원) 방향 샘플링
  - pdfValue(): 객체의 PDF 값
  - generate(): 객체 방향 랜덤 생성
- MixturePDF: 두 PDF 50:50 혼합
  - BRDF + 광원 샘플링 결합
- Hittable에 pdfValue(), random() 메서드 추가
- Sphere, Quad에 PDF 메서드 구현

## 이전 스냅샷 대비 변화
- HittablePDF, MixturePDF 추가
- Sphere::pdfValue(), Sphere::random() 구현
- Quad::pdfValue(), Quad::random() 구현
- HittableList에 PDF 메서드 구현

## 빌드/테스트 상태
✅ 빌드 가능
✅ 테스트 가능

## 다음 단계
- ScatterRecord 구조체 추가
- Material 확장 (scatteringPdf)
