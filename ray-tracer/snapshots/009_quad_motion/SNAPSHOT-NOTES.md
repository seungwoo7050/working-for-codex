# Snapshot 009: Quad & Motion Blur (v2.0.6-2.0.7)

## 패치 범위
- 2.0.6: Quad Primitive
- 2.0.7: Motion Blur

## 포함된 기능
- Quad: 사각형 평면 프리미티브
  - 평면 방정식 교차 판정
  - 알파/베타 좌표 내부 판정
- MovingSphere: 시간에 따라 이동하는 구체
- Translate: 객체 이동 인스턴싱
- RotateY: Y축 회전 인스턴싱
- Ray에 time 파라미터 추가

## 이전 스냅샷 대비 변화
- quad.h 추가
- moving_sphere.h 추가
- translate.h, rotate_y.h 추가
- Ray에 time() 메서드 추가
- Camera에서 랜덤 시간 광선 생성

## 빌드/테스트 상태
✅ 빌드 가능
✅ 테스트 가능
✅ Motion Blur 렌더링 가능

## 다음 단계
- ConstantMedium (볼륨) 추가
