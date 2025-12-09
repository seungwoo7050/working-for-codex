#pragma once
#include <string>

// [FILE]
// - 목적: 몬스터 데이터를 정의한다.
// - 주요 역할: 체력, 공격력 속성을 보관하고 상태를 노출한다.
// - 관련 토이 버전: [CPP-C0.1]
// - 권장 읽는 순서: 필드 -> 메서드.
//
// [LEARN] Player와 동일한 인터페이스를 갖춰 전투 시스템에서 다루기 쉽게 만든다.
class Monster {
public:
    Monster(std::string name, int hp, int attack);
    void takeDamage(int amount);
    int getHp() const;
    int getAttack() const;
    const std::string& getName() const;
private:
    std::string name_;
    int hp_;
    int attack_;
};
