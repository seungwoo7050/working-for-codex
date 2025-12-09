package com.example.minijob.job;

import java.util.List;
import java.util.Map;

import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.DeleteMapping;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

// [FILE]
// - 목적: Job 관련 REST 엔드포인트를 제공한다.
// - 주요 역할: 목록 조회, 단건 조회, 생성, 삭제를 HTTP 인터페이스로 노출한다.
// - 관련 토이 버전: [BE-v0.2]
// - 권장 읽는 순서: 라우팅 -> 요청/응답 DTO 구조 -> 서비스 호출.
//
// [LEARN] REST 컨트롤러에서 서비스 레이어를 호출하고 간단한 DTO를 매핑하는 방법을 익힌다.
@RestController
@RequestMapping("/jobs")
public class JobController {
    private final JobService jobService;

    public JobController(JobService jobService) {
        this.jobService = jobService;
    }

    @GetMapping
    public ResponseEntity<List<Job>> list() {
        return ResponseEntity.ok(jobService.list());
    }

    @GetMapping("/{id}")
    public ResponseEntity<Job> get(@PathVariable Long id) {
        return ResponseEntity.ok(jobService.get(id));
    }

    @PostMapping
    public ResponseEntity<Job> create(@RequestBody Map<String, String> payload) {
        String title = payload.getOrDefault("title", "untitled");
        return ResponseEntity.ok(jobService.create(title));
    }

    @DeleteMapping("/{id}")
    public ResponseEntity<Void> delete(@PathVariable Long id) {
        jobService.delete(id);
        return ResponseEntity.noContent().build();
    }
}
