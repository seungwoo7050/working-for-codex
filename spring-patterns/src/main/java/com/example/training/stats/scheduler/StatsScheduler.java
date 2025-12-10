package com.example.training.stats.scheduler;

import com.example.training.stats.service.StatsService;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.scheduling.annotation.Scheduled;
import org.springframework.stereotype.Component;

import java.time.LocalDate;

// [FILE]
// - 목적: 일별 통계 집계 배치 작업 스케줄링
// - 주요 역할: 매일 새벽 3시에 전날 통계 자동 집계
// - 관련 클론 가이드 단계: [CG-v1.2.0] Batch, Stats, Cache
// - 권장 읽는 순서: 이 파일 → StatsService.aggregateDailyStats()
//
// [LEARN] C 개발자를 위한 설명:
// 이 클래스는 "스케줄러(cron job)"를 Spring 내에서 구현한 것이다.
//
// 전통적인 방식:
// 1. 별도의 배치 스크립트 작성 (shell/python)
// 2. crontab에 등록: 0 3 * * * /opt/scripts/stats.sh
// 3. 스크립트가 DB 접속 → 쿼리 실행 → 결과 저장
//
// Spring Scheduler 방식:
// 1. @Scheduled 어노테이션으로 스케줄 정의
// 2. 기존 Service 메서드 호출
// 3. 애플리케이션과 함께 배포/관리
//
// 장점: 코드 통합, 트랜잭션/로깅 일관성, 단일 배포 단위

@Component
// [LEARN] @Component: Spring Bean으로 등록
// @Scheduled가 동작하려면 이 클래스가 Bean이어야 함
public class StatsScheduler {

    private static final Logger log = LoggerFactory.getLogger(StatsScheduler.class);

    private final StatsService statsService;

    public StatsScheduler(StatsService statsService) {
        this.statsService = statsService;
    }

    // [LEARN] @Scheduled: 이 메서드를 주기적으로 실행
    // cron 표현식: 초 분 시 일 월 요일
    // "0 0 3 * * *" = 매일 03:00:00에 실행
    //
    // 다른 옵션들:
    // - fixedRate = 5000: 5초마다 실행 (이전 실행 완료 여부 무관)
    // - fixedDelay = 5000: 이전 실행 완료 후 5초 후 실행
    // - initialDelay = 10000: 애플리케이션 시작 후 10초 후 첫 실행
    @Scheduled(cron = "0 0 3 * * *")
    public void aggregateDailyStats() {
        // [LEARN] try-catch로 예외 처리
        // 스케줄러 메서드에서 예외가 발생하면 다음 실행에 영향을 줄 수 있으므로
        // 반드시 예외를 잡아서 로깅하고 정상 종료해야 함
        try {
            // [LEARN] 어제 날짜 계산
            // 새벽 3시에 실행되므로 "어제" 데이터를 집계
            LocalDate yesterday = LocalDate.now().minusDays(1);
            log.info("Starting daily stats aggregation for: {}", yesterday);
            
            statsService.aggregateDailyStats(yesterday);
            
            log.info("Completed daily stats aggregation for: {}", yesterday);
        } catch (Exception e) {
            // [LEARN] 배치 작업 실패 시 로깅
            // 프로덕션에서는 알림(Slack, PagerDuty 등) 연동 필요
            log.error("Error during daily stats aggregation", e);
        }
    }
}

// [Reader Notes]
// ═══════════════════════════════════════════════════════════════════════════════
// 이 파일에서 처음 등장한 개념: Spring Scheduler, cron 표현식
//
// Spring Scheduler (C 개발자 관점):
// ───────────────────────────────────────────────────────────────────────────────
// 1. 활성화 방법:
//    메인 Application 클래스에 @EnableScheduling 추가 필요
//    (이 프로젝트에서는 설정 클래스에 있을 수 있음)
//
// 2. cron 표현식 비교:
//    [Unix crontab]     분 시 일 월 요일
//    0 3 * * *          매일 03:00
//
//    [Spring @Scheduled] 초 분 시 일 월 요일
//    0 0 3 * * *         매일 03:00:00
//
//    Spring은 "초" 필드가 추가됨!
//
// 3. 주의사항:
//    - 기본적으로 단일 스레드에서 실행 (동시 실행 안 됨)
//    - 실행 시간이 길면 다음 스케줄에 영향
//    - 분산 환경에서는 여러 인스턴스가 동시에 실행될 수 있음
//      → ShedLock 등으로 분산 락 필요
//
// 4. 대안:
//    - Spring Batch: 대용량 데이터 처리, 재시작/롤백 지원
//    - Quartz: 더 복잡한 스케줄링, 클러스터링 지원
//    - 외부 스케줄러: Kubernetes CronJob, AWS EventBridge
//
// 이 파일을 이해한 다음 볼 파일:
// ───────────────────────────────────────────────────────────────────────────────
// 1. reactive/service/ReactiveProductService.java - WebFlux 리액티브 패턴
// 2. ratelimit/RateLimitingFilter.java - Rate Limiting 구현
// ═══════════════════════════════════════════════════════════════════════════════
