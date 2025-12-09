# TUTORIAL_BE – mini-job-service

이 문서는 BE-v0.1 ~ BE-v0.2 흐름을 CLI 중심으로 따라가는 튜토리얼입니다.

## 공통 준비
```bash
cd backend/mini-job-service
chmod +x gradlew
./gradlew clean build
```

## BE-v0.1 – 헬스 체크
1. 서버 실행
```bash
./gradlew bootRun
```
2. 새 터미널에서 상태 확인
```bash
curl http://localhost:8080/health
# 기대 출력: {"status":"OK","timestamp":"..."}
```

## BE-v0.2 – Job CRUD (인메모리)
1. 서버 실행 후 Job 생성
```bash
curl -X POST -H "Content-Type: application/json" \
  -d '{"title":"demo"}' http://localhost:8080/jobs
```
2. 목록 조회
```bash
curl http://localhost:8080/jobs
```
3. 단건 조회 및 삭제
```bash
curl http://localhost:8080/jobs/1
curl -X DELETE http://localhost:8080/jobs/1
```

## 태그 예시
```bash
git checkout BE-v0.2
```
