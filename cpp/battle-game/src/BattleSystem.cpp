#include "BattleSystem.h"
#include <chrono>
#include <iostream>
#include <thread>

// [FILE]
// - 목적: BattleSystem 구현.
// - 주요 역할: 턴 별 데미지 계산과 로그 출력, 승패 판정.
// - 관련 토이 버전: [CPP-C0.1]
// - 권장 읽는 순서: while 루프 -> 피해 처리 -> 종료 조건.
//
// [LEARN] 표준 라이브러리로 간단한 게임 루프와 대기 시간을 구현하는 방법을 익힌다.
void BattleSystem::runBattle(Player& player, Monster& monster) {
    std::cout << "배틀 시작! " << player.getName() << " vs " << monster.getName() << "\n";
    while (player.getHp() > 0 && monster.getHp() > 0) {
        // Player turn
        monster.takeDamage(player.getAttack());
        std::cout << player.getName() << "의 공격! " << monster.getName() << " HP=" << monster.getHp() << "\n";
        if (monster.getHp() <= 0) break;

        // Monster turn
        player.takeDamage(monster.getAttack());
        std::cout << monster.getName() << "의 반격! " << player.getName() << " HP=" << player.getHp() << "\n";

        std::this_thread::sleep_for(std::chrono::milliseconds(400));
    }

    if (player.getHp() > 0) {
        std::cout << "플레이어 승리!\n";
    } else {
        std::cout << "몬스터 승리...\n";
    }
}
