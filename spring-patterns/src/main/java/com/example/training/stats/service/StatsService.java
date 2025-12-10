package com.example.training.stats.service;

import com.example.training.issue.repository.CommentRepository;
import com.example.training.issue.repository.IssueRepository;
import com.example.training.stats.domain.DailyIssueStats;
import com.example.training.stats.repository.DailyIssueStatsRepository;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.time.LocalDate;
import java.time.LocalDateTime;
import java.time.LocalTime;
import java.util.List;

// [FILE]
// - 목적: 일별 이슈 통계 집계 및 조회 서비스
// - 주요 역할: 배치 작업(aggregateDailyStats), 통계 조회
// - 관련 클론 가이드 단계: [CG-v1.2.0] Batch, Stats, Cache
// - 권장 읽는 순서: Order 1(통계 집계) → Order 2(통계 조회)
//
// [LEARN] C 개발자를 위한 설명:
// 이 서비스는 "배치 처리(Batch Processing)"를 구현한다.
// 실시간으로 통계를 계산하면 성능 문제가 발생하므로,
// 미리 계산해서 별도 테이블에 저장해두는 패턴이다.
//
// C에서 cron job으로 실행하는 집계 스크립트와 유사:
// ./aggregate_daily_stats.sh 2025-04-01
// 
// 이 스크립트가 하루치 데이터를 집계해서 summary 테이블에 저장하는 것처럼,
// aggregateDailyStats() 메서드가 같은 역할을 수행한다.

@Service
public class StatsService {

    // [LEARN] SLF4J Logger: 로깅 추상화 라이브러리
    // C의 printf/fprintf 대신 레벨별 로깅 (debug, info, warn, error)
    // 프로덕션에서는 로그 레벨로 출력 제어, 파일/원격 전송 등 설정 가능
    private static final Logger log = LoggerFactory.getLogger(StatsService.class);

    private final DailyIssueStatsRepository dailyStatsRepository;
    private final IssueRepository issueRepository;
    private final CommentRepository commentRepository;

    public StatsService(DailyIssueStatsRepository dailyStatsRepository,
                       IssueRepository issueRepository,
                       CommentRepository commentRepository) {
        this.dailyStatsRepository = dailyStatsRepository;
        this.issueRepository = issueRepository;
        this.commentRepository = commentRepository;
    }

    // ═══════════════════════════════════════════════════════════════════════
    // [Order 1] 일별 통계 집계 - 배치 작업용 메서드
    // StatsScheduler에서 cron으로 매일 호출됨
    // ═══════════════════════════════════════════════════════════════════════

    @Transactional
    public void aggregateDailyStats(LocalDate date) {
        log.info("Aggregating daily stats for date: {}", date);
        // [LEARN] {} 플레이스홀더: SLF4J에서 파라미터를 안전하게 삽입
        // printf("Aggregating for %s", date)와 유사하지만,
        // 로그 레벨이 비활성화되면 문자열 연결 자체를 수행하지 않아 효율적

        // [LEARN] LocalDate, LocalTime, LocalDateTime: Java 8+ 날짜/시간 API
        // C의 struct tm과 유사하지만 불변(immutable)이고 스레드 안전
        LocalDateTime startOfDay = date.atStartOfDay();           // 00:00:00
        LocalDateTime endOfDay = date.atTime(LocalTime.MAX);      // 23:59:59.999999999

        // [LEARN] 집계 쿼리 실행
        // 각 Repository 메서드가 COUNT(*) 쿼리를 실행
        int createdCount = issueRepository.countByCreatedAtBetween(startOfDay, endOfDay);
        int resolvedCount = issueRepository.countResolvedBetween(startOfDay, endOfDay);
        int commentCount = commentRepository.countByCreatedAtBetween(startOfDay, endOfDay);

        // [LEARN] Upsert 패턴: 있으면 UPDATE, 없으면 INSERT
        // C에서 SELECT로 조회 후 조건 분기하던 패턴을 함수형으로 표현
        DailyIssueStats stats = dailyStatsRepository.findByDate(date)
            .map(existing -> {
                // [LEARN] 이미 존재하면 업데이트
                existing.update(createdCount, resolvedCount, commentCount);
                return existing;
            })
            .orElseGet(() -> new DailyIssueStats(date, createdCount, resolvedCount, commentCount));
            // [LEARN] orElseGet: 값이 없으면 새 객체 생성
            // orElse vs orElseGet: orElseGet은 필요할 때만 람다 실행 (지연 평가)

        dailyStatsRepository.save(stats);
        log.info("Daily stats aggregated: created={}, resolved={}, comments={}",
                 createdCount, resolvedCount, commentCount);
    }

    // ═══════════════════════════════════════════════════════════════════════
    // [Order 2] 기간별 통계 조회
    // ═══════════════════════════════════════════════════════════════════════

    @Transactional(readOnly = true)
    public List<DailyIssueStats> getDailyStats(LocalDate from, LocalDate to) {
        // [LEARN] 미리 집계된 데이터를 조회하므로 빠름
        // 실시간으로 GROUP BY 하는 것보다 훨씬 효율적
        return dailyStatsRepository.findByDateBetweenOrderByDateAsc(from, to);
    }
}

// [Reader Notes]
// ═══════════════════════════════════════════════════════════════════════════════
// 이 파일에서 처음 등장한 개념: 배치 처리, 집계 패턴
//
// 배치 처리 패턴 (C 개발자 관점):
// ───────────────────────────────────────────────────────────────────────────────
// 1. 왜 배치로 처리하나?
//    - 실시간 집계: SELECT COUNT(*) FROM issues WHERE date = '2025-04-01'
//      → 데이터가 많아지면 느려짐, 매 요청마다 테이블 풀스캔
//    - 배치 집계: 미리 계산해서 summary 테이블에 저장
//      → 조회는 인덱스 1회만, 일정한 성능
//
// 2. cron vs Spring Scheduler:
//    [전통적 C/쉘 방식]
//    # crontab
//    0 3 * * * /opt/scripts/aggregate_stats.sh  # 매일 새벽 3시
//
//    [Spring 방식]
//    @Scheduled(cron = "0 0 3 * * *")  // 동일한 표현
//    public void aggregateStats() { ... }
//
//    장점: 애플리케이션 코드와 통합, 트랜잭션/로깅 일관성, 배포 편의성
//
// 3. Upsert (MERGE) 패턴:
//    [C/SQL 방식]
//    result = query("SELECT * FROM stats WHERE date = ?", date);
//    if (result) {
//        query("UPDATE stats SET count = ? WHERE date = ?", count, date);
//    } else {
//        query("INSERT INTO stats (date, count) VALUES (?, ?)", date, count);
//    }
//
//    [Spring/Java 방식]
//    repository.findByDate(date)
//        .map(existing -> { existing.setCount(count); return existing; })
//        .orElseGet(() -> new Stats(date, count));
//    repository.save(stats);
//
// 이 파일을 이해한 다음 볼 파일:
// ───────────────────────────────────────────────────────────────────────────────
// 1. stats/scheduler/StatsScheduler.java - 배치 작업 스케줄링
// 2. reactive/service/ReactiveProductService.java - 리액티브 패턴
// ═══════════════════════════════════════════════════════════════════════════════
