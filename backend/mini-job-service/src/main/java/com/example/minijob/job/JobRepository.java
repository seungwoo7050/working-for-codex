package com.example.minijob.job;

import java.util.List;
import java.util.Optional;

// [FILE]
// - 목적: Job 데이터 액세스 인터페이스를 정의한다.
// - 주요 역할: Job CRUD 계약을 표준화하여 구현체 교체를 쉽게 한다.
// - 관련 토이 버전: [BE-v0.2]
// - 권장 읽는 순서: 메서드 시그니처 확인 후 InMemory 구현을 살펴본다.
//
// [LEARN] 저장소 인터페이스를 정의해 의존성 역전을 연습한다.
public interface JobRepository {
    List<Job> findAll();

    Optional<Job> findById(Long id);

    Job save(Job job);

    void deleteById(Long id);
}
