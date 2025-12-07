# Snapshot 011: Rate Limiting

## 버전 정보
- **버전**: v2.2.0
- **기반**: v2.1.0 (Virtual Threads)
- **날짜**: 2025-01

## 핵심 변경 사항

### 1. Token Bucket Rate Limiter
- 버스트 트래픽 허용
- 클라이언트별 독립적 한도
- 자동 토큰 리필

### 2. Rate Limiting 필터
- 모든 API 요청에 적용
- 헬스체크, H2 콘솔 등 제외
- 429 Too Many Requests 응답

### 3. 표준 Rate Limit 헤더
- `X-RateLimit-Limit`: 최대 요청 수
- `X-RateLimit-Remaining`: 남은 요청 수
- `X-RateLimit-Reset`: 리셋 시간

## 새로 추가된 파일

```
src/main/java/com/example/training/ratelimit/
├── RateLimiter.java              # 인터페이스
├── RateLimitResult.java          # 결과 객체
├── TokenBucketRateLimiter.java   # Token Bucket 구현
├── ClientKeyResolver.java        # 클라이언트 식별
└── RateLimitingFilter.java       # 서블릿 필터

src/test/java/com/example/training/ratelimit/
└── RateLimitingTest.java         # 테스트
```

## Token Bucket 알고리즘

```
버킷 용량: 100 토큰
리필 속도: 10 토큰/초
리필 간격: 1초
```

### 동작 방식
1. 요청 도착 → 토큰 1개 소비
2. 토큰 있음 → 요청 허용
3. 토큰 없음 → 429 응답
4. 시간 경과 → 토큰 리필

## 클라이언트 식별

| 우선순위 | 식별 방법 | 키 형식 |
|----------|-----------|---------|
| 1 | Bearer Token | `user:token_prefix` |
| 2 | X-Forwarded-For | `ip:원본_IP` |
| 3 | Remote Address | `ip:클라이언트_IP` |

## 응답 예시

### 정상 응답 (200)
```http
HTTP/1.1 200 OK
X-RateLimit-Limit: 100
X-RateLimit-Remaining: 95
X-RateLimit-Reset: 1640000060
```

### 한도 초과 (429)
```http
HTTP/1.1 429 Too Many Requests
X-RateLimit-Limit: 100
X-RateLimit-Remaining: 0
X-RateLimit-Reset: 1640000060
Retry-After: 5

{
  "error": "Too Many Requests",
  "message": "Rate limit exceeded. Please retry after 5 seconds.",
  "retryAfter": 5
}
```

## 테스트 검증

- `shouldAllowRequestsWithinLimit`: 한도 내 요청 허용
- `shouldDenyRequestsExceedingLimit`: 한도 초과 거부
- `shouldTrackDifferentClientsIndependently`: 클라이언트 독립
- `shouldReturnCorrectRateLimitHeaders`: 헤더 검증
- `shouldRefillTokensAfterTime`: 토큰 리필 검증

## 실행 방법

```bash
./gradlew bootRun
./gradlew test
```

## 다음 버전
- v2.3.0: Caching & Compression (캐싱 & 압축)
