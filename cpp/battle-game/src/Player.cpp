#include "Player.h"

// [FILE]
// - 목적: Player 클래스 구현부.
// - 주요 역할: 생성자 초기화, 데미지 처리 로직 제공.
// - 관련 토이 버전: [CPP-C0.1]
// - 권장 읽는 순서: 생성자 -> takeDamage -> getter.
//
// [LEARN] 헤더 선언을 분리한 뒤 소스 구현을 완성하는 패턴을 익힌다.
Player::Player(std::string name, int hp, int attack)
    : name_(std::move(name)), hp_(hp), attack_(attack) {}

void Player::takeDamage(int amount) {
    hp_ -= amount;
    if (hp_ < 0) hp_ = 0;
}

int Player::getHp() const { return hp_; }
int Player::getAttack() const { return attack_; }
const std::string& Player::getName() const { return name_; }
