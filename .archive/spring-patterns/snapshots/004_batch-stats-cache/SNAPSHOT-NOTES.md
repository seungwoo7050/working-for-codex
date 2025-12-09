# 스냅샷 004: Batch Stats Cache (v1.2.0)

## 개요
- **대응 버전**: v1.2.0
- **패치 범위**: 1.2.1 ~ 1.2.5
- **빌드 가능**: ⚠️ 일부 파일 누락 (코드 복사 필요)
- **테스트 가능**: ⚠️ 일부 파일 누락

## 포함된 기능
- v1.1.0의 모든 기능 (Team & RBAC)
- DailyIssueStats 엔티티
- @Scheduled 배치 작업
- @Cacheable 캐시 처리
- ExternalApiService

## 새로 추가된 컴포넌트
### Entity
- DailyIssueStats

### Service
- StatsService (@Scheduled 배치)
- ExternalApiService (@Cacheable)

### Configuration
- CacheConfig (캐시 설정)

## 주요 어노테이션
- `@EnableScheduling`: 스케줄링 활성화
- `@EnableCaching`: 캐시 활성화
- `@Scheduled`: 배치 작업 스케줄링
- `@Cacheable`: 메서드 결과 캐싱

## 빌드 방법
```bash
cd snapshots/004_batch-stats-cache
./gradlew build
```

## 테스트 방법
```bash
./gradlew test
```

## 참고 사항
- 전체 코드가 필요한 경우 메인 프로젝트의 `src/` 디렉토리에서 복사하세요.
