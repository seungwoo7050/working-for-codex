#pragma once
#include <string>

// [FILE]
// - 목적: 플레이어 캐릭터 데이터를 정의한다.
// - 주요 역할: 체력, 공격력 속성을 보관하고 데미지 계산을 수행한다.
// - 관련 토이 버전: [CPP-C0.1]
// - 권장 읽는 순서: 필드 -> 메서드 -> 사용 예시(main.cpp).
//
// [LEARN] 간단한 클래스 인터페이스와 헤더 분리 구조를 익힌다.
class Player {
public:
    Player(std::string name, int hp, int attack);
    void takeDamage(int amount);
    int getHp() const;
    int getAttack() const;
    const std::string& getName() const;
private:
    std::string name_;
    int hp_;
    int attack_;
};
