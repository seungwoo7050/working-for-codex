> ⚠️ 이 리포지토리는 **학습용 토이 프로젝트**입니다.
>   실서비스 운영/보안/장애 대응을 전제로 설계되지 않았습니다.

# battle-game (CPP-C0.1)
턴제 콘솔 배틀 미니 게임.

## Version Roadmap
- CPP-C0.1: 플레이어/몬스터 전투 로직과 기본 입출력

## Implementation Order (Files)
1. `Player.h`, `Player.cpp`
2. `Monster.h`, `Monster.cpp`
3. `BattleSystem.h`, `BattleSystem.cpp`
4. `main.cpp`

## Troubleshooting
- 컴파일 시 C++17 옵션을 포함했는지 확인: `g++ -std=c++17 ...`
- Windows에서 소켓 예제 실행 시 winsock2 초기화 필요 (해당 단계에서는 콘솔만 사용)
