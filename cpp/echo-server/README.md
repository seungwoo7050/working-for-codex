> ⚠️ 이 리포지토리는 **학습용 토이 프로젝트**입니다.
>   실서비스 운영/보안/장애 대응을 전제로 설계되지 않았습니다.

# echo-server (CPP-C0.2)
단일 클라이언트를 지원하는 TCP 에코 서버/클라이언트 예제입니다.

## Version Roadmap
- CPP-C0.2: 서버-클라이언트 통신 및 단순 에코 처리

## Implementation Order (Files)
1. `server.cpp`
2. `client.cpp`

## Troubleshooting
- 포트 사용 중 오류: 다른 포트로 변경하거나 사용 중인 프로세스를 종료
- 클라이언트 연결 실패: 서버가 먼저 실행되었는지, 동일한 호스트/포트를 사용하는지 확인
- Windows 빌드 시 `-lws2_32` 옵션 필요
