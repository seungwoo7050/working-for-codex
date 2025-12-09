# 코딩 에이전트 작업 지시서 – Toy Project Suite

## 0. 목적

이 리포지토리는 **학습용 토이 프로젝트 묶음**이다.  
목표는 다음 세 가지 영역의 기본기를 단계적으로 익힐 수 있게 만드는 것이다.

1. **백엔드 (Spring Boot)** – `mini-job-service`
2. **프론트엔드 (React)** – `mini-job-dashboard` (1과 HTTP로 연동)
3. **C++ (언어/네트워크 토이)**  
   - `battle-game`
   - `echo-server`
   - (선택) `multi-chat-server`

모든 코드는 **튜토리얼처럼 그대로 따라 할 수 있는 수준**으로 작성되어야 한다.  
코드 + 주석 + README + 튜토리얼 문서까지 포함한다.

---

## 1. 리포지토리 구조

리포지토리는 아래 구조를 기본으로 한다.

```text
root/
  ROOT_ROADMAP.md        # 전체 로드맵 (전역)
  ENV_SETUP.md           # 환경 셋업 가이드 (전역)

  backend/
    mini-job-service/
      README.md
      TUTORIAL_BE.md
      ... (Spring Boot 소스)

  frontend/
    mini-job-dashboard/
      README.md
      TUTORIAL_FE.md
      ... (React 소스)

  cpp/
    battle-game/
      README.md
      ... (C++ 콘솔 게임)

    echo-server/
      README.md
      ... (C++ 에코 서버)

    multi-chat-server/   # 선택 구현
      README.md
      ... (C++ 멀티 클라 채팅 서버)
````

---

## 2. 전역 규칙

### 2.1 Toy Project 명시 (필수)

모든 서브 프로젝트의 `README.md` 상단에는 **반드시** 아래 문구를 포함한다.

```markdown
> ⚠️ 이 리포지토리는 **학습용 토이 프로젝트**입니다.  
>   실서비스 운영/보안/장애 대응을 전제로 설계되지 않았습니다.
```

(백엔드 / 프론트 / 각 C++ 토이 모두 동일하게 적용)

### 2.2 Git 태그 규칙

각 “버전 단계”가 완전히 동작하는 시점에 **Git 태그**를 생성한다.

* 백엔드: `BE-v0.1` ~ `BE-v0.8`
* 프론트: `FE-F0.1` ~ `FE-F0.5`
* C++:

  * `CPP-C0.1` (battle-game)
  * `CPP-C0.2` (echo-server)
  * `CPP-C0.3` (multi-chat-server, 선택)

`ROOT_ROADMAP.md`와 각 `TUTORIAL_*.md`에서 이 태그를 사용한 checkout 예시를 제공해야 한다.

```bash
git checkout BE-v0.3
```

태그는 **해당 버전이 README / 튜토리얼 기준으로 정상 동작하는 커밋**에 달 것.

---

## 3. 코드 주석 규칙

### 3.1 적용 범위

아래 파일 유형에 대해 주석 규칙을 적용한다.

* 백엔드: `.java`
* 프론트: `.ts`, `.tsx`, `.js`, `.jsx`
* C++: `.cpp`, `.cc`, `.cxx`, `.h`, `.hpp`

테스트 코드나 매우 단순한 파일은 `[FILE]` 블록만 간소하게 써도 된다.

언어에 따라 `//` 또는 `/* ... */` 형식을 사용하되, **내용 구조**는 동일하게 맞춘다.

---

### 3.2 파일 상단 헤더 주석 – `[FILE]`, `[LEARN]`

각 소스 파일 최상단에 다음 형식의 주석을 단다.

```cpp
// [FILE]
// - 목적: 이 파일의 역할 한 줄 요약
// - 주요 역할: 이 파일이 하는 핵심 작업 2~3줄
// - 관련 토이 버전: [BE-v0.2], [FE-F0.3], [CPP-C0.1] 등
// - 권장 읽는 순서: 이 파일 안에서 어떤 함수/클래스를 먼저 볼지 안내
//
// [LEARN] 이 파일에서 배우게 되는 핵심 개념 1~3줄.
//         이전 단계(버전)과 어떤 점이 연결되는지 간단히 언급.
```

예시 – C++ `main.cpp`:

```cpp
// [FILE]
// - 목적: battle-game 토이의 진입점 (main 함수)
// - 주요 역할: Player/Monster를 생성하고 턴제 전투 루프를 실행
// - 관련 토이 버전: [CPP-C0.1]
// - 권장 읽는 순서: main() → setupPlayers() → battleLoop()
//
// [LEARN] C++에서 객체를 스택에 생성하고, 함수를 분리해 로직을 구성하는 기본 패턴을 익힌다.
//         C의 struct + 함수 묶음에서 "클래스 기반 설계"로 넘어가는 감각을 학습한다.
```

예시 – 백엔드 `JobController.java`:

```java
// [FILE]
// - 목적: Job 관련 HTTP 엔드포인트 집합
// - 주요 역할: /api/jobs 요청을 서비스 레이어로 위임하고, 응답 DTO로 매핑
// - 관련 토이 버전: [BE-v0.2], [BE-v0.3]
// - 권장 읽는 순서: constructor → createJob() → getJob() → listJobs()
//
// [LEARN] Spring MVC에서 @RestController, @RequestMapping, DTO 변환 패턴을 익힌다.
//         서비스와 컨트롤러를 분리하여 "웹 계층"을 구성하는 기본 구조를 학습한다.
```

예시 – 프론트 `JobList.tsx`:

```ts
// [FILE]
// - 목적: Job 목록을 표 형태로 보여주는 UI 컴포넌트
// - 주요 역할: props로 받은 Job 배열을 렌더링하고, 선택/클릭 이벤트를 상위 컴포넌트로 전달
// - 관련 토이 버전: [FE-F0.2], [FE-F0.4]
// - 권장 읽는 순서: 타입 정의 → 컴포넌트 선언 → JSX 렌더링 → 이벤트 핸들러
//
// [LEARN] React 함수형 컴포넌트에서 props를 받아 리스트를 렌더링하는 기본 패턴을 익힌다.
//         백엔드 Job 모델을 프론트 타입으로 매핑해 사용하는 흐름을 학습한다.
```

---

### 3.3 코드 내부 블록 주석 – `[Order N]`, `[LEARN]`

중요한 로직/함수 블록 앞에는 **반드시** `[Order N]` + `[LEARN]` 형식의 주석을 단다.

* `[Order N]`

  * 이 파일을 읽을 때 이 블록을 **몇 번째로 보면 좋은지**.
* `[LEARN]`

  * 이 블록에서 배우는 개념, 기존 단계와의 연결.

예시 – 백엔드:

```java
// [Order 1] Job 생성 엔드포인트
// - 클라이언트에서 들어온 DTO를 서비스로 전달하고, 생성된 Job을 응답 DTO로 변환
// - 토이 버전: [BE-v0.2]
// [LEARN] 컨트롤러는 비즈니스 로직을 직접 구현하지 않고,
//         DTO 변환과 HTTP 상태코드 처리에 집중한다.
@PostMapping
public ResponseEntity<JobResponse> createJob(@Valid @RequestBody CreateJobRequest request) {
    ...
}
```

예시 – C++:

```cpp
// [Order 2] 메인 수신 루프
// - 클라이언트로부터 문자열을 읽고, 그대로 다시 돌려주는 에코 서버 핵심 부분
// - 토이 버전: [CPP-C0.2]
// [LEARN] blocking recv/send 패턴을 통해 "요청 1건당 read/write 1회" 구조를 익힌다.
//         이후 비동기/멀티플렉싱(Boost.Asio 등)으로 확장할 때 비교 기준이 된다.
while (true) {
    ...
}
```

---

### 3.4 파일 하단 – `[Reader Notes]` (선택)

복잡한 파일(Spring Security 설정, C++ 네트워크 코드 등)은 파일 맨 아래에
**선택적으로** `[Reader Notes]` 블록을 붙인다.

```cpp
// [Reader Notes]
// ------------------------------------------------------------
// 이 파일에서 새로 등장한 개념:
// 1. blocking socket API (socket, bind, listen, accept, recv, send)
// 2. IPv4 주소 구조체(sockaddr_in)와 포트 바인딩
// 3. 간단한 에러 처리 패턴 (리턴값 확인 후 std::cerr로 출력)
//
// 다음에 보면 좋은 파일:
// - client.cpp: 클라이언트 관점에서 같은 프로토콜을 사용하는 코드
```

---

## 4. 전역 문서 작성 요구사항

### 4.1 `ROOT_ROADMAP.md`

리포지토리 최상위에 `ROOT_ROADMAP.md`를 생성한다.

**필수 내용:**

1. **전체 개요**

   * 이 리포가 “백엔드 / 프론트 / C++ 토이 프로젝트 세트”라는 점
2. **추천 진행 순서**

   * 예시 (형식만, 내용은 에이전트가 채운다):

     * 1단계: `BE-v0.1` ~ `BE-v0.3` – Spring + JPA 기본
     * 2단계: `FE-F0.1` ~ `FE-F0.3` – React + 컴포넌트/상태 기본
     * 3단계: `BE-v0.4` ~ `BE-v0.7` – Postgres, JWT, 비동기 Job
     * 4단계: `FE-F0.4` ~ `FE-F0.5` – 백엔드 연동 + 폴링
     * 5단계: `CPP-C0.1` ~ `CPP-C0.3` – C++ 언어 + 네트워크 기본
   * 각 단계마다 “이 단계를 끝내면 할 수 있는 것”을 2~3줄로 요약
3. **서브 프로젝트/문서 링크**

   * `backend/mini-job-service/README.md`
   * `frontend/mini-job-dashboard/README.md`
   * `cpp/.../README.md`
   * `ENV_SETUP.md`, 각 `TUTORIAL_*.md`

---

### 4.2 `ENV_SETUP.md`

리포지토리 최상위에 `ENV_SETUP.md`를 생성한다.

**필수 내용:**

1. **필수 도구 및 권장 버전**

   * JDK (예: 21)
   * Node.js (예: 20.x)
   * Docker / Docker Compose
   * C++ 컴파일러 (g++ 또는 clang++, `-std=c++17` 지원)
   * Git
2. **OS 안내**

   * 기준 OS (예: Linux / WSL) 명시
   * Windows / macOS 사용자는 패키지 매니저/경로만 다르고 개념은 동일하다고 서술
3. **설치 확인 명령어 + 기대 예시 출력**

   * `java -version`
   * `node -v`
   * `docker --version`
   * `g++ --version`
   * 각 명령어 아래, 예시 출력 1~2줄
4. **포트/리소스 사용 계획**

   * 백엔드: 8080
   * 프론트: 3000
   * Postgres: 5432
   * 포트 충돌 시 수정 방법 간략 설명

---

## 5. 백엔드 – `backend/mini-job-service`

### 5.1 기술 스택

* Java 21
* Spring Boot 3.x
* Spring Web
* Spring Data JPA
* Spring Security (JWT)
* DB: H2 (초기) → PostgreSQL (이후)
* Flyway (PostgreSQL 단계)
* Gradle

### 5.2 패키지 구조

```text
com.example.minijob
  ├── MiniJobApplication.java
  ├── config/        # Security, JWT, DB 등 설정
  ├── job/           # Job 도메인 (엔티티, 서비스, 컨트롤러)
  ├── user/          # User, Auth (회원가입/로그인, JWT)
  └── common/        # 공통 예외 처리, DTO, 유틸리티
```

### 5.3 도메인 개요

**Job**

* `id: Long`
* `type: String` – `"VIDEO_TRIM"`, `"VIDEO_THUMBNAIL"`, `"MATCHMAKING"` 등
* `status: String` – `"PENDING"`, `"RUNNING"`, `"DONE"`, `"FAILED"`
* `payload: String` – 옵션, JSON 문자열 허용
* `createdAt: LocalDateTime`
* (v0.6 이후) `createdBy: User` 연관 가능

**User**

* `id: Long`
* `email: String` (unique)
* `password: String` (BCrypt 해시)
* `role: String` (예: `"USER"`, `"ADMIN"`)

---

### 5.4 백엔드 Version Roadmap

이 내용은 `backend/mini-job-service/README.md`에도 포함한다.

* `BE-v0.1` – 헬스체크 엔드포인트만 있는 최소 Spring Boot 앱
* `BE-v0.2` – 메모리 기반 Job CRUD (DB 없음)
* `BE-v0.3` – H2 + JPA 기반 Job 영속화
* `BE-v0.4` – PostgreSQL + Flyway 도입
* `BE-v0.5` – 공통 예외 처리 + 검증 + 로깅
* `BE-v0.6` – User + JWT 인증/인가
* `BE-v0.7` – 비동기 Job 처리 시뮬레이션 (@Scheduled)
* `BE-v0.8` – 통합 테스트 + Docker Compose

---

### 5.5 Implementation Order (파일 작성 순서) – 예시

이 섹션을 `backend/mini-job-service/README.md`에 넣는다.
실제 파일 이름/패키지는 에이전트가 구체화하되, **순서는 유지**한다.

```markdown
## Implementation Order (Files)

### BE-v0.1

1. `MiniJobApplication.java`
2. `HealthController.java`

### BE-v0.2

1. `job/domain/Job.java`
2. `job/repository/InMemoryJobRepository.java`
3. `job/service/JobService.java`
4. `job/api/CreateJobRequest.java`, `JobResponse.java`
5. `job/api/JobController.java`

### BE-v0.3

1. `job/domain/Job.java` (JPA 엔티티로 변경)
2. `job/repository/JobJpaRepository.java`
3. `application.yml` (H2 설정)

### BE-v0.4

1. `docker-compose.yml` (Postgres)
2. `application.yml` (Postgres 설정)
3. `db/migration/V1__init_job.sql` (Flyway)

... (이후 v0.5 ~ v0.8도 같은 형식으로 나열)
```

구체 내용은 에이전트가 채워야 한다.

---

### 5.6 `TUTORIAL_BE.md` (CLI 튜토리얼)

`backend/mini-job-service/TUTORIAL_BE.md`는 **버전별로 CLI 튜토리얼**을 제공한다.

각 버전에 대해 다음 포맷을 따른다:

````markdown
## BE-v0.1 – 헬스체크까지

### 1. 사전 준비

- 필요한 선행 버전: 없음
- 필요한 도구: JDK, Gradle (ENV_SETUP.md 참고)

### 2. 코드 체크아웃 (선택)

```bash
git checkout BE-v0.1
````

### 3. 실행

```bash
cd backend/mini-job-service
./gradlew bootRun
```

### 4. 동작 확인

```bash
curl http://localhost:8080/health
# 기대 응답 예:
# { "status": "ok" }
```

### 5. 이 단계에서 배우는 것

* Spring Boot 앱 실행 방법
* 헬스체크 엔드포인트 구조

````

`BE-v0.2` ~ `BE-v0.8`도 같은 패턴으로 작성하되,  
각 버전마다 **실제 명령어, curl 예시, 기대 응답**을 반드시 채운다.

---

### 5.7 백엔드 README – Troubleshooting 섹션

`backend/mini-job-service/README.md`에는 최소한 다음 수준의 **Troubleshooting**을 포함한다.

```markdown
## Troubleshooting

- 서버가 바로 종료됨
  - `./gradlew bootRun` 로그를 확인하고, 스택트레이스를 본다.
  - BE-v0.4 이상 버전에서는 Postgres Docker 컨테이너가 떠 있는지 확인한다.

- DB 연결 오류 (connection refused / authentication failed)
  - `docker ps`로 postgres 컨테이너 실행 여부 확인
  - `application.yml` 또는 `.env`에 설정된 DB URL/계정을 재확인

- 포트 충돌 (8080 already in use)
  - 다른 프로세스가 8080을 사용 중인지 확인 후 종료하거나,
  - `application.yml`에서 서버 포트를 변경한 뒤 다시 실행한다.
````

---

## 6. 프론트 – `frontend/mini-job-dashboard`

### 6.1 기술 스택

* React
* (가능하면) TypeScript
* 빌드: Vite 또는 CRA
* HTTP: `fetch` 또는 `axios`
* 스타일: 자유 (CSS, Tailwind, UI 라이브러리 등)

백엔드는 `mini-job-service`를 사용한다.

### 6.2 도메인 타입 예시

```ts
type Job = {
  id: number;
  type: string;
  status: string;
  payload: string;
  createdAt: string;
};
```

### 6.3 프론트 Version Roadmap

`frontend/mini-job-dashboard/README.md`에도 포함한다.

* `FE-F0.1` – 프로젝트 뼈대, `/jobs` 기본 화면
* `FE-F0.2` – dummy 데이터 Job 리스트 렌더링
* `FE-F0.3` – Job 생성 폼 (로컬 상태)
* `FE-F0.4` – 백엔드 REST 연동 (GET/POST)
* `FE-F0.5` – Job 상태 폴링 (주기적 갱신)

---

### 6.4 Implementation Order (파일 작성 순서)

```markdown
## Implementation Order (Files)

### FE-F0.1

1. `main.tsx`
2. `App.tsx`

### FE-F0.2

1. `types/Job.ts`
2. `components/JobList.tsx`

### FE-F0.3

1. `components/JobForm.tsx`
2. `hooks/useLocalJobs.ts` (선택)

### FE-F0.4

1. `hooks/useJobsApi.ts`
2. `App.tsx` (백엔드 연동으로 업데이트)

### FE-F0.5

1. `hooks/useJobsPolling.ts`
2. `App.tsx` (폴링 로직 연결)
```

구체 구현은 에이전트가 채운다.

---

### 6.5 `TUTORIAL_FE.md` (CLI 튜토리얼)

`frontend/mini-job-dashboard/TUTORIAL_FE.md`는 각 버전에 대해 다음 구조를 따른다.

````markdown
## FE-F0.1 – 기본 화면까지

### 1. 사전 준비

- 필요한 선행 버전: BE-v0.1 (백엔드가 실행 가능하면 더 좋음)
- 필요한 도구: Node.js, npm (ENV_SETUP.md 참고)

### 2. 코드 체크아웃 (선택)

```bash
git checkout FE-F0.1
````

### 3. 의존성 설치 및 실행

```bash
cd frontend/mini-job-dashboard
npm install
npm run dev
```

### 4. 동작 확인

브라우저에서 `http://localhost:5173/jobs` (또는 dev 서버 포트) 접속.

* 페이지 상단에 "Jobs" 타이틀이 보여야 한다.
* 아직 Job 리스트는 비어 있어도 된다.

### 5. 이 단계에서 배우는 것

* React 개발 서버 실행
* 기본 라우트/화면 구조

````

`FE-F0.2` ~ `FE-F0.5`도 똑같은 패턴으로 작성한다.  
각 단계에서 화면에서 **무엇이 보이면 성공인지**를 구체적으로 적는다.

---

### 6.6 프론트 README – Troubleshooting 섹션

```markdown
## Troubleshooting

- `npm install` 에러
  - Node.js 버전이 ENV_SETUP.md에 명시된 버전 이상인지 확인한다.

- 개발 서버 포트 충돌 (3000 또는 5173 already in use)
  - 다른 개발 서버를 종료하거나, Vite 설정에서 포트를 변경한다.

- 백엔드 연동 시 CORS 에러
  - 백엔드 `mini-job-service`의 CORS 설정(config 패키지)을 확인한다.
  - 프론트에서 호출하는 API URL이 실제 백엔드 주소와 일치하는지 확인한다.
````

---

## 7. C++ 토이 – `cpp/`

### 7.1 프로젝트 목록

* `cpp/battle-game/` – 콘솔 턴제 배틀 (언어/구조)
* `cpp/echo-server/` – 단일 클라이언트 TCP 에코 서버/클라이언트
* `cpp/multi-chat-server/` – (선택) 다중 클라이언트 채팅 서버

빌드는 기본적으로:

```bash
g++ -std=c++17 -O2 -Wall main.cpp -o main
```

또는 CMake 사용 가능(에이전트가 선택).

---

### 7.2 Version Roadmap (C++)

이 내용은 `cpp/TUTORIAL_CPP.md`와 각 README에 포함한다.

* `CPP-C0.1` – battle-game: 콘솔 턴제 배틀
* `CPP-C0.2` – echo-server: 단일 클라이언트 TCP 에코
* `CPP-C0.3` – multi-chat-server: 다중 클라이언트 + 공유 상태 (선택)

---

### 7.3 Implementation Order (파일 작성 순서)

예시:

```markdown
## Implementation Order (Files)

### CPP-C0.1 – battle-game

1. `Player.h`, `Player.cpp`
2. `Monster.h`, `Monster.cpp`
3. `BattleSystem.h`, `BattleSystem.cpp`
4. `main.cpp`

### CPP-C0.2 – echo-server

1. `server.cpp`
2. `client.cpp`
3. `build.sh` 또는 `CMakeLists.txt`

### CPP-C0.3 – multi-chat-server (선택)

1. `Server.h`, `Server.cpp`
2. `ClientHandler.h`, `ClientHandler.cpp`
3. `main.cpp`
```

실제 설계/파일 분리는 에이전트가 구체화하되, **단계 진행 흐름**은 유지한다.

---

### 7.4 `TUTORIAL_CPP.md` (CLI 튜토리얼)

`cpp/TUTORIAL_CPP.md`에는 각 C++ 버전별로 다음 포맷을 따른다.

````markdown
## CPP-C0.1 – battle-game

### 1. 빌드

```bash
cd cpp/battle-game
g++ -std=c++17 -O2 -Wall *.cpp -o battle-game
````

### 2. 실행 예시

```bash
./battle-game
# 예시 입출력:
# Player HP: 100, Monster HP: 50
# > 공격을 선택하세요: ...
```

### 3. 이 단계에서 배우는 것

* C++ 클래스/헤더/소스 분리
* std::vector를 이용한 몬스터 목록 관리

````

`CPP-C0.2`, `CPP-C0.3`도 동일 패턴으로 작성한다.

---

### 7.5 C++ README – Troubleshooting 예시

각 C++ 프로젝트의 `README.md`에:

```markdown
## Troubleshooting

- 컴파일 에러 (std::string 관련)
  - 컴파일 옵션에 `-std=c++17`이 포함되어 있는지 확인한다.

- `bind`/`listen` 실패
  - 이미 해당 포트를 다른 프로세스가 사용 중인지 확인한다.
  - Linux 기준: `ss -lntp | grep <포트>`로 확인 가능.

- 클라이언트가 서버에 연결할 수 없음
  - 서버가 먼저 실행되어 있는지 확인한다.
  - 서버/클라이언트가 같은 호스트와 포트를 사용하도록 설정했는지 확인한다.
````

---

## 8. 정리 – 에이전트가 반드시 해야 할 일

코딩 에이전트는 아래 항목을 **모두** 수행해야 한다.

1. **리포지토리 구조**를 이 문서대로 구성
2. **모든 README**에:

   * Toy Project 경고 문구
   * Version Roadmap
   * Implementation Order (파일 작성 순서)
   * Troubleshooting 섹션
3. **전역 문서**:

   * `ROOT_ROADMAP.md`
   * `ENV_SETUP.md`
4. **튜토리얼 문서**:

   * `backend/mini-job-service/TUTORIAL_BE.md`
   * `frontend/mini-job-dashboard/TUTORIAL_FE.md`
   * `cpp/TUTORIAL_CPP.md`
   * 각 버전에 대해 CLI 명령어 + 기대 출력/화면을 명시
5. **코드 주석 규칙**:

   * 모든 주요 소스 파일에 `[FILE]`, `[LEARN]` 헤더
   * 중요한 블록마다 `[Order N]` + `[LEARN]`
   * 필요 시 `[Reader Notes]`
6. **Git 태그**:

   * 각 버전 완료 시 `BE-v0.x`, `FE-F0.x`, `CPP-C0.x` 태그 생성
7. 모든 예시는 실제 코드/구조에 맞게 **에이전트가 구체 값으로 채워 넣는다.**