package com.example.training.team.repository;

import com.example.training.team.domain.Team;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * [FILE] TeamRepository - 팀 데이터 접근 계층
 * ─────────────────────────────────────────────────────────────────────────────────
 * - 목적: 팀 엔티티의 CRUD 기능 제공
 * - 관련 클론 가이드 단계: [CG-v1.1.0] 1.1.1 팀 도메인
 * 
 * [LEARN] 기본 Repository:
 * JpaRepository를 상속만 해도 다음 메서드 자동 제공:
 * 
 * CRUD:
 * - save(entity): 저장/수정
 * - findById(id): ID로 조회 (Optional 반환)
 * - findAll(): 전체 조회
 * - delete(entity): 삭제
 * - deleteById(id): ID로 삭제
 * - count(): 개수 조회
 * - existsById(id): 존재 여부
 * 
 * 페이징:
 * - findAll(Pageable): 페이징 조회
 * - findAll(Sort): 정렬 조회
 * 
 * JPA 특화:
 * - flush(): 영속성 컨텍스트 → DB 동기화
 * - saveAndFlush(): 저장 후 즉시 flush
 * 
 * C 관점:
 *   // C에서 각각 구현해야 할 것들
 *   int team_save(Team* t);
 *   Team* team_find_by_id(int id);
 *   int team_delete(int id);
 *   // ... 많은 보일러플레이트
 *   
 *   → JpaRepository 상속 한 줄로 모두 자동 생성
 * ═══════════════════════════════════════════════════════════════════════════════
 */
@Repository
public interface TeamRepository extends JpaRepository<Team, Long> {
    // [LEARN] 커스텀 쿼리 메서드가 없어도 기본 CRUD는 모두 사용 가능
    // 필요시 findByName(String name) 등 추가 가능
}
