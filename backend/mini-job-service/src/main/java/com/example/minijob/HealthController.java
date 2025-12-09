package com.example.minijob;

import java.time.Instant;
import java.util.HashMap;
import java.util.Map;

import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RestController;

// [FILE]
// - 목적: 서비스 헬스체크 엔드포인트를 제공한다.
// - 주요 역할: 간단한 상태 정보와 타임스탬프를 반환하여 서버 가동 여부를 확인한다.
// - 관련 토이 버전: [BE-v0.1]
// - 권장 읽는 순서: status 응답 생성 흐름 -> 향후 모니터링 확장 아이디어.
//
// [LEARN] REST 컨트롤러 작성 패턴과 ResponseEntity 사용법을 익힌다.
@RestController
public class HealthController {

    // [Order 1] 헬스 체크 응답을 구성한다.
    // [LEARN] 간단한 Map 기반 JSON 응답을 만드는 방법을 학습.
    @GetMapping("/health")
    public ResponseEntity<Map<String, Object>> health() {
        Map<String, Object> payload = new HashMap<>();
        payload.put("status", "OK");
        payload.put("timestamp", Instant.now().toString());
        return ResponseEntity.ok(payload);
    }
}
