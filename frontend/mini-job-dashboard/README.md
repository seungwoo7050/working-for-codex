> ⚠️ 이 리포지토리는 **학습용 토이 프로젝트**입니다.
>   실서비스 운영/보안/장애 대응을 전제로 설계되지 않았습니다.

# mini-job-dashboard
React 기반 대시보드로 백엔드 mini-job-service API를 호출해 잡 목록을 표시하고 상태를 폴링합니다.

## Version Roadmap
- FE-F0.1: Vite + React 기본 렌더링
- FE-F0.2: Job 타입 정의 및 목업 표시
- FE-F0.3: 백엔드 API 연동
- FE-F0.4: 폴링 및 상태 배지 추가
- FE-F0.5: 에러 처리 및 UX 다듬기

## Implementation Order (Files)
1. `src/main.tsx`
2. `src/App.tsx`
3. `src/types.ts`, `src/api.ts`
4. `src/components/JobList.tsx` 등 구성 요소 확장

## Troubleshooting
- 의존성 설치 실패: Node 버전(>=18) 확인 후 `npm install` 재시도
- dev 서버 포트 충돌: `npm run dev -- --port 3001`로 변경 가능
- CORS 오류: 백엔드 `application.yml`에서 CORS 허용을 설정하거나 프록시 사용

## Tutorials
CLI 실행 및 화면 확인 절차는 `TUTORIAL_FE.md`를 참조합니다.
