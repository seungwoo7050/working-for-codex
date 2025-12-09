#include "Monster.h"

// [FILE]
// - 목적: Monster 클래스 구현부.
// - 주요 역할: 생성, 피해 처리, 상태 조회.
// - 관련 토이 버전: [CPP-C0.1]
// - 권장 읽는 순서: 생성자 -> takeDamage -> getter.
//
// [LEARN] 간단한 상태 변화를 함수로 묶어 가독성을 높인다.
Monster::Monster(std::string name, int hp, int attack)
    : name_(std::move(name)), hp_(hp), attack_(attack) {}

void Monster::takeDamage(int amount) {
    hp_ -= amount;
    if (hp_ < 0) hp_ = 0;
}

int Monster::getHp() const { return hp_; }
int Monster::getAttack() const { return attack_; }
const std::string& Monster::getName() const { return name_; }
