#pragma once
#include "Monster.h"
#include "Player.h"

// [FILE]
// - 목적: 플레이어와 몬스터 간 전투 흐름을 처리한다.
// - 주요 역할: 턴 진행, 공격 로직, 종료 조건 판정.
// - 관련 토이 버전: [CPP-C0.1]
// - 권장 읽는 순서: runBattle 시나리오 -> 내부 전투 루프.
//
// [LEARN] 간단한 게임 루프를 함수로 구성하는 방법을 익힌다.
class BattleSystem {
public:
    void runBattle(Player& player, Monster& monster);
};
