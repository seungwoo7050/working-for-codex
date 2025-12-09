# TUTORIAL_FE – mini-job-dashboard

React 대시보드를 로컬에서 실행하는 절차를 요약한다.

## FE-F0.1 ~ FE-F0.3 준비
```bash
cd frontend/mini-job-dashboard
npm install
npm run dev -- --host --port 3000
```
브라우저에서 `http://localhost:3000` 접속 후 "Mini Job Dashboard" 문구가 보이면 성공.

## 백엔드 연동 테스트 (FE-F0.3)
백엔드가 8080 포트에서 실행 중이라면:
```bash
curl http://localhost:8080/jobs
```
프론트 화면에서 "작업 추가" 폼에 제목을 입력 후 추가하면 목록이 업데이트된다.

## 태그 예시
```bash
git checkout FE-F0.3
```
