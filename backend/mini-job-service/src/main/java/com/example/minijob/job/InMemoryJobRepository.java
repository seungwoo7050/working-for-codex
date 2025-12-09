package com.example.minijob.job;

import java.time.Instant;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.Optional;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.atomic.AtomicLong;

import org.springframework.stereotype.Repository;

// [FILE]
// - 목적: JobRepository의 인메모리 구현체를 제공한다.
// - 주요 역할: 동시성을 고려한 Map 기반 저장, ID 시퀀스 관리, CRUD 제공.
// - 관련 토이 버전: [BE-v0.2]
// - 권장 읽는 순서: 필드 정의 -> save 동작 -> find/delete.
//
// [LEARN] 동시성 컬렉션을 이용한 간단한 리포지토리 구현을 경험한다.
@Repository
public class InMemoryJobRepository implements JobRepository {
    private final Map<Long, Job> storage = new ConcurrentHashMap<>();
    private final AtomicLong idSequence = new AtomicLong(0L);

    @Override
    public List<Job> findAll() {
        return new ArrayList<>(storage.values());
    }

    @Override
    public Optional<Job> findById(Long id) {
        return Optional.ofNullable(storage.get(id));
    }

    @Override
    public Job save(Job job) {
        // [Order 1] ID 없으면 시퀀스로 생성한다.
        if (job.getId() == null) {
            job.setId(idSequence.incrementAndGet());
            job.setCreatedAt(Instant.now());
        }
        storage.put(job.getId(), job);
        return job;
    }

    @Override
    public void deleteById(Long id) {
        storage.remove(id);
    }
}
