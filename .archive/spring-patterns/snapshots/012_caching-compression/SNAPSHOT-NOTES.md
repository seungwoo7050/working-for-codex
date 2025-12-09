# 012_caching-compression

## 스냅샷 정보
- **버전**: v2.3.0
- **이전 스냅샷**: 011_rate-limiting
- **테마**: Caching & Compression

## 주요 변경사항

### 1. 캐싱 설정 (CachingConfig)
- `@EnableCaching` 활성화
- `ConcurrentMapCacheManager` 기반 인메모리 캐시
- 캐시 목록: products, product, categories, hot-items, popularIssues
- `@Primary` 어노테이션으로 기본 CacheManager 지정

### 2. 캐시 적용 서비스 (CachedProductService)
- `@Cacheable`: 조회 결과 캐싱 (findById, findAll, search)
- `@CacheEvict`: 변경 시 캐시 무효화 (create, update, delete)
- `@Caching`: 복합 캐시 작업 지원
- 100ms 지연 시뮬레이션으로 캐시 효과 확인

### 3. 캐시 관리 서비스 (CacheManagementService)
- 특정 캐시/키 무효화
- 전체 캐시 무효화
- 캐시 통계 조회
- `@Scheduled`로 주기적 캐시 정리 (hot-items: 1분)

### 4. HTTP 압축 (CompressionConfig)
- Gzip 압축 활성화
- 최소 응답 크기: 1KB
- 지원 MIME 타입: JSON, XML, HTML, Text, CSS, JavaScript

### 5. REST API
- `/api/cache/products` - 캐시 적용 상품 CRUD
- `/api/cache/admin` - 캐시 관리 API

## 파일 목록

### 설정
- `config/CachingConfig.java` - 캐시 설정
- `config/CompressionConfig.java` - HTTP 압축 설정

### 서비스
- `cache/service/CachedProductService.java` - 캐시 적용 상품 서비스
- `cache/service/CacheManagementService.java` - 캐시 관리 서비스

### 컨트롤러
- `cache/controller/CachedProductController.java` - 상품 API
- `cache/controller/CacheAdminController.java` - 캐시 관리 API

### 테스트
- `cache/service/CachedProductServiceTest.java`
- `cache/service/CacheManagementServiceTest.java`
- `cache/controller/CachedProductControllerTest.java`
- `cache/controller/CacheAdminControllerTest.java`

## 테스트 실행
```bash
./gradlew test
```

## 캐시 API 사용 예시
```bash
# 상품 조회 (캐시 적용)
curl http://localhost:8080/api/cache/products/1

# 캐시 통계 조회
curl http://localhost:8080/api/cache/admin/stats

# 전체 캐시 무효화
curl -X DELETE http://localhost:8080/api/cache/admin
```

## 주의사항
- ConcurrentMapCache는 TTL 미지원 (운영환경에서는 Redis/Caffeine 권장)
- 압축은 1KB 이상 응답에만 적용
- `@Scheduled`는 `@EnableScheduling` 필요 (AsyncConfig에서 활성화)
