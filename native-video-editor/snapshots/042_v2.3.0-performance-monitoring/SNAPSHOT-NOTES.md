# 042_v2.3.0-performance-monitoring 스냅샷

## 📋 패치 정보

| 항목 | 값 |
|------|-----|
| 패치 범위 | 2.3.1 ~ 2.3.3 |
| 버전 태그 | v2.3.0 |
| 이름 | Performance Monitoring |
| 이전 단계 | v2.2.0 (Metadata Analysis) |

## 🎯 이 스냅샷의 목표

Prometheus 메트릭 수집 및 Grafana 대시보드를 통한 성능 모니터링 시스템을 구축합니다.

### 추가된 기능
- **MetricsService**: prom-client 기반 메트릭 수집
  - Histogram: 처리 시간 분포
  - Counter: 요청 수, 에러 수
  - Gauge: 메모리 사용량, 캐시 히트율
- **/metrics 엔드포인트**: Prometheus 스크래핑용
- **Grafana 대시보드**: 시각화 설정

## 📁 포함되어야 할 파일

### v2.2.0에서 상속
- 모든 v2.2.0 파일들

### 신규 추가 파일
```
backend/src/
├── services/
│   └── metrics.service.ts     # 메트릭 수집 서비스
└── routes/
    └── metrics.ts             # /metrics 엔드포인트

monitoring/
├── prometheus/
│   └── prometheus.yml         # Prometheus 설정
└── grafana/
    ├── provisioning/
    │   ├── datasources/
    │   │   └── prometheus.yml
    │   └── dashboards/
    │       └── default.yml
    └── dashboards/
        └── video-editor.json  # 대시보드 정의
```

## 🔧 추가 요구사항

- **Prometheus** (Docker 또는 로컬 설치)
- **Grafana** (Docker 또는 로컬 설치)

### Docker로 실행
```bash
docker run -d --name prometheus -p 9090:9090 \
  -v $(pwd)/monitoring/prometheus:/etc/prometheus \
  prom/prometheus

docker run -d --name grafana -p 3000:3000 \
  -e GF_SECURITY_ADMIN_PASSWORD=admin \
  grafana/grafana
```

## 🚀 빌드 및 실행

```bash
cd backend && npm install && npm run dev

# 메트릭 확인
curl http://localhost:3001/metrics
```

## ✅ 테스트

1. 여러 API 호출 수행
2. `/metrics` 엔드포인트에서 메트릭 확인
3. Grafana (http://localhost:3000) 에서 대시보드 확인
4. Prometheus (http://localhost:9090) 에서 쿼리 테스트

## 📝 완료 기준

- [x] 썸네일 추출 히스토그램 메트릭
- [x] 메타데이터 추출 히스토그램 메트릭
- [x] API 지연 메트릭
- [x] Grafana 대시보드 조회 가능

## 📝 다음 단계

→ 현재 레포지토리 최종 상태 (v3.0.0): Production Deployment
