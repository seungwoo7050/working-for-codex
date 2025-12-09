package com.example.minijob.job;

import java.util.List;
import java.util.NoSuchElementException;

import org.springframework.stereotype.Service;

// [FILE]
// - 목적: Job 도메인의 비즈니스 로직을 캡슐화한다.
// - 주요 역할: 목록 조회, 단건 조회, 생성/삭제, 예외 처리.
// - 관련 토이 버전: [BE-v0.2]
// - 권장 읽는 순서: 생성 -> 조회 -> 삭제 순서로 읽으며 예외 흐름을 확인.
//
// [LEARN] 서비스 레이어에서 리포지토리를 사용하는 기본 패턴과 예외 처리 방식을 익힌다.
@Service
public class JobService {
    private final JobRepository repository;

    public JobService(JobRepository repository) {
        this.repository = repository;
    }

    public List<Job> list() {
        return repository.findAll();
    }

    public Job get(Long id) {
        return repository.findById(id).orElseThrow(() -> new NoSuchElementException("Job not found"));
    }

    public Job create(String title) {
        Job job = new Job(null, title, "PENDING", null);
        return repository.save(job);
    }

    public void delete(Long id) {
        repository.deleteById(id);
    }
}
