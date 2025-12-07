# Snapshot 007: AABB & BVH (v2.0.1-2.0.2)

## 패치 범위
- 2.0.1: AABB 클래스
- 2.0.2: BVH 노드

## 포함된 기능
- AABB (Axis-Aligned Bounding Box)
  - Slab method 교차 판정
  - surroundingBox 합집합 계산
- BVHNode
  - 랜덤 축 선택 분할
  - 재귀적 트리 구축
  - O(log N) 교차 판정

## 이전 스냅샷 대비 변화
- aabb.h 추가
- bvh_node.h 추가
- Hittable에 boundingBox() 메서드 추가
- Sphere, HittableList에 boundingBox() 구현

## 빌드/테스트 상태
✅ 빌드 가능
✅ 테스트 가능
✅ 벤치마크 실행 가능

## 성능
- 500개 객체 기준 ~30배 이상 속도 향상

## 다음 단계
- 텍스처 시스템 추가
