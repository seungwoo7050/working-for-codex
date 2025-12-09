#include "BattleSystem.h"
#include "Monster.h"
#include "Player.h"
#include <iostream>

// [FILE]
// - 목적: battle-game 실행 엔트리.
// - 주요 역할: 플레이어/몬스터 생성 후 전투 시스템 실행.
// - 관련 토이 버전: [CPP-C0.1]
// - 권장 읽는 순서: 엔티티 생성 -> BattleSystem 실행.
//
// [LEARN] 여러 헤더를 포함해 작은 게임을 구성하는 전체 흐름을 익힌다.
int main() {
    Player player("Hero", 40, 8);
    Monster monster("Slime", 25, 4);

    BattleSystem system;
    system.runBattle(player, monster);

    std::cout << "게임 종료" << std::endl;
    return 0;
}
