# TUTORIAL_CPP – C++ 토이 모음

## CPP-C0.1 – battle-game
### 1. 빌드
```bash
cd cpp/battle-game
g++ -std=c++17 -O2 -Wall src/*.cpp -o battle-game
```
### 2. 실행 예시
```bash
./battle-game
# 예시 출력: 공격/반격 로그와 승리 메시지
```
### 3. 이 단계에서 배우는 것
- 클래스 분리, 간단한 게임 루프

## CPP-C0.2 – echo-server
### 1. 빌드
```bash
cd cpp/echo-server
g++ -std=c++17 -O2 -Wall server.cpp -o echo-server
```
별도 터미널에서 클라이언트 빌드:
```bash
g++ -std=c++17 -O2 -Wall client.cpp -o echo-client
```
### 2. 실행 예시
```bash
./echo-server &
./echo-client
```
### 3. 이 단계에서 배우는 것
- POSIX 소켓 기반 서버/클라이언트 통신

## CPP-C0.3 – multi-chat-server
### 1. 빌드
```bash
cd cpp/multi-chat-server
g++ -std=c++17 -O2 -Wall src/*.cpp -lpthread -o multi-chat-server
```
### 2. 실행 예시
```bash
./multi-chat-server
# 여러 netcat 세션에서 9100 포트로 접속 후 메시지를 주고받는다.
```
### 3. 이 단계에서 배우는 것
- 다중 클라이언트 처리, 뮤텍스 활용 브로드캐스트

## 태그 예시
```bash
git checkout CPP-C0.1
```
