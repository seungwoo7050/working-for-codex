# Toy Project Suite – ROOT_ROADMAP

본 문서는 백엔드, 프론트엔드, C++ 토이 프로젝트 전체 진행 로드맵을 요약한다. 각 단계는 Git 태그로 관리하며, 태그를 checkout 하면 해당 버전의 완성된 상태를 바로 확인할 수 있다.

## 버전 태그 개요
- 백엔드(Spring Boot): BE-v0.1 ~ BE-v0.8
- 프론트엔드(React): FE-F0.1 ~ FE-F0.5
- C++ 토이: CPP-C0.1(battle-game), CPP-C0.2(echo-server), CPP-C0.3(multi-chat-server)

## 공통 진행 순서
1. 전역 환경 셋업: `ENV_SETUP.md`를 참고하여 Java, Node.js, g++, Git을 준비한다.
2. 각 서브 프로젝트 README의 Implementation Order에 따라 파일을 작성한다.
3. 튜토리얼 문서(`TUTORIAL_BE.md`, `TUTORIAL_FE.md`, `TUTORIAL_CPP.md`)의 CLI 명령어를 실행하며 기능을 확인한다.
4. 단계가 끝나면 `git tag <버전>` 으로 태그를 남기고 다음 단계로 이동한다.

## 태그 사용 예시
```bash
git checkout BE-v0.3
```

## 추천 학습 흐름
1. 백엔드 BE-v0.1 ~ BE-v0.8 순서로 API와 인증을 익힌다.
2. 프론트엔드 FE-F0.1 ~ FE-F0.5 순서로 React 상태/폴링을 익힌다.
3. C++ CPP-C0.1 ~ CPP-C0.3 순서로 콘솔/네트워크를 익힌다.

## 최종 목표
각 서브 프로젝트가 독립 실행 가능하며, README와 튜토리얼만으로 재현 가능한 학습용 토이 묶음을 완성한다.
