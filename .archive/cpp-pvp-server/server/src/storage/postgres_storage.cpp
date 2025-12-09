// [FILE]
// - 목적: PostgreSQL 스토리지 (영구 데이터 저장)
// - 주요 역할: 세션 이벤트, 플레이어 데이터 등 영구 저장
// - 관련 클론 가이드 단계: [v1.0.0] 기본 서버 - 데이터 계층
// - 권장 읽는 순서: Connect → RecordSessionEvent → MetricsSnapshot
//
// [LEARN] PostgreSQL 특징:
//         - ACID 보장: 게임 결과, 결제 등 중요 데이터
//         - 관계형: 플레이어-매치 관계 등 복잡한 쿼리
//         - libpq: C API (C++에서도 사용 가능)
//         - RAII 패턴: unique_ptr + custom deleter로 연결 관리

#include "pvpserver/storage/postgres_storage.h"

#include <chrono>
#include <iostream>
#include <sstream>

namespace pvpserver {

// [Order 1] Custom Deleter - 연결 자원 해제
// [LEARN] unique_ptr에 커스텀 삭제자 지정.
//         C에서 수동 PQfinish() 호출하던 것을 자동화.
void PostgresStorage::ConnDeleter::operator()(PGconn* conn) const noexcept {
    if (conn) {
        PQfinish(conn);  // libpq 연결 종료
    }
}

PostgresStorage::PostgresStorage(std::string dsn) : dsn_(std::move(dsn)) {}

PostgresStorage::~PostgresStorage() { Disconnect(); }

// [Order 2] Connect - 데이터베이스 연결
// [LEARN] DSN(Data Source Name): "host=localhost dbname=game user=..." 형식
bool PostgresStorage::Connect() {
    if (connection_) {
        return true;  // 이미 연결됨
    }
    PGconn* raw = PQconnectdb(dsn_.c_str());
    if (PQstatus(raw) != CONNECTION_OK) {
        std::cerr << "postgres connection failed: " << PQerrorMessage(raw);
        PQfinish(raw);
        return false;
    }
    connection_.reset(raw);  // unique_ptr에 저장 (RAII)
    return true;
}

void PostgresStorage::Disconnect() { connection_.reset(); }

bool PostgresStorage::IsConnected() const noexcept { return connection_ != nullptr; }

// [Order 3] RecordSessionEvent - 세션 이벤트 기록
// [LEARN] Prepared Statement 패턴:
//         - $1, $2: 파라미터 바인딩 (SQL Injection 방지)
//         - param_values: 바인딩할 값 배열
//         - 쿼리 시간 측정 → 모니터링용
bool PostgresStorage::RecordSessionEvent(const std::string& player_id, const std::string& event) {
    if (!connection_) {
        std::cerr << "postgres write skipped: no connection" << std::endl;
        return false;
    }
    const auto start = std::chrono::steady_clock::now();
    
    // 파라미터 바인딩 준비
    const char* param_values[2] = {player_id.c_str(), event.c_str()};
    const int param_lengths[2] = {static_cast<int>(player_id.size()),
                                  static_cast<int>(event.size())};
    const int param_formats[2] = {0, 0};  // 0 = 텍스트 형식

    // PQexecParams: 파라미터화된 쿼리 실행
    PGresult* result = PQexecParams(connection_.get(),
                                    "INSERT INTO session_events(player_id, event_type, created_at)"
                                    " VALUES($1, $2, NOW())",
                                    2, nullptr, param_values, param_lengths, param_formats, 0);
    const auto finish = std::chrono::steady_clock::now();
    const double duration = std::chrono::duration<double>(finish - start).count();
    last_query_seconds_.store(duration, std::memory_order_relaxed);
    
    if (PQresultStatus(result) != PGRES_COMMAND_OK) {
        std::cerr << "postgres insert failed: " << PQerrorMessage(connection_.get());
        PQclear(result);
        return false;
    }
    PQclear(result);  // 결과 객체 해제 (필수!)
    return true;
}

// [Order 4] MetricsSnapshot - Prometheus 메트릭 출력
// [LEARN] Prometheus 텍스트 형식:
//         # TYPE metric_name gauge
//         metric_name value
std::string PostgresStorage::MetricsSnapshot() const {
    std::ostringstream oss;
    oss << "# TYPE database_query_duration_seconds gauge\n";
    oss << "database_query_duration_seconds " << last_query_seconds_.load(std::memory_order_relaxed)
        << "\n";
    return oss.str();
}

double PostgresStorage::LastQueryDurationSeconds() const noexcept {
    return last_query_seconds_.load(std::memory_order_relaxed);
}

// ========================================================================
// [Reader Notes] PostgreSQL 스토리지
// ========================================================================
// 1. libpq (C API)
//    - PQconnectdb: 연결
//    - PQexecParams: 파라미터화된 쿼리
//    - PQclear/PQfinish: 자원 해제
//    - C++에서는 RAII로 감싸서 안전하게 사용
//
// 2. unique_ptr + custom deleter
//    - std::unique_ptr<PGconn, ConnDeleter>
//    - 소멸 시 자동으로 PQfinish() 호출
//    - C의 수동 자원 관리 → C++ 자동 관리
//
// 3. std::atomic (last_query_seconds_)
//    - 멀티스레드 환경에서 안전한 읽기/쓰기
//    - memory_order_relaxed: 최소 동기화 (성능 우선)
//
// 4. 메트릭 수집
//    - 쿼리 시간 측정 → 슬로우 쿼리 감지
//    - Prometheus + Grafana로 시각화
//
// [v1.0.0] 데이터 계층 - PostgreSQL 영구 저장소
// ========================================================================

}  // namespace pvpserver
