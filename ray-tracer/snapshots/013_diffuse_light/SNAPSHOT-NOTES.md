# Snapshot 013: Importance Sampling Setup (v3.0.5)

## 패치 범위
- 3.0.5: DiffuseLight (광원 중요도 샘플링)

## 포함된 기능
- ScatterRecord 구조체
  - attenuation: 감쇠율
  - pdfPtr: 산란 PDF
  - skipPdf: PDF 생략 여부 (Specular용)
  - skipPdfRay: PDF 생략 시 광선
- Material 확장
  - scatter(ScatterRecord) 오버로드
  - scatteringPdf() 메서드
- Lambertian PDF 버전
- Metal/Dielectric skipPdf 구현

## 이전 스냅샷 대비 변화
- ScatterRecord 추가
- Material 인터페이스 확장
- Lambertian에 CosinePDF 사용
- Metal/Dielectric에 skipPdf 설정

## 빌드/테스트 상태
✅ 빌드 가능
✅ 테스트 가능

## 다음 단계 (현재 루트)
- Camera::rayColor 중요도 샘플링 버전
- Camera::render(world, lights) 오버로드
- Cornell Box 분산 감소 확인
