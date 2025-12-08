package com.example.training;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.cache.annotation.EnableCaching;

// [FILE]
// - 목적: Spring Boot 애플리케이션의 진입점(Entry Point)
// - 주요 역할: 애플리케이션 부트스트랩 및 컴포넌트 스캔 시작점
// - 관련 클론 가이드 단계: [CG-v0.1.0] Bootstrap & CI
// - 권장 읽는 순서: 이 파일 → HealthController → GlobalExceptionHandler
//
// [LEARN] C 개발자를 위한 설명:
// C의 main() 함수와 유사하게, 이 클래스가 프로그램의 시작점이다.
// 하지만 Spring Boot는 단순히 main()을 호출하는 것 이상으로,
// @SpringBootApplication 어노테이션을 통해 다음을 자동으로 수행한다:
// 1. 컴포넌트 스캔: 이 패키지와 하위 패키지의 모든 @Component, @Service 등을 찾아 등록
// 2. 자동 설정: 클래스패스의 라이브러리를 감지하여 적절한 Bean 구성
// 3. 설정 파일 로드: application.yml 등의 설정 자동 적용
//
// C에서 여러 .c 파일을 링크하고 초기화 코드를 직접 작성해야 했다면,
// Spring Boot는 이 모든 "배선(wiring)" 작업을 프레임워크가 대신 해준다.

@SpringBootApplication
// [LEARN] @SpringBootApplication은 세 가지 어노테이션의 조합이다:
// - @Configuration: 이 클래스가 Bean 정의를 포함할 수 있음 (C의 config.h와 유사)
// - @EnableAutoConfiguration: 클래스패스 기반 자동 설정 활성화
// - @ComponentScan: 하위 패키지의 컴포넌트 자동 탐색

@EnableCaching
// [LEARN] @EnableCaching: 메서드 결과 캐싱 기능 활성화
// @Cacheable이 붙은 메서드의 반환값을 메모리/Redis 등에 캐시한다.
// C에서 전역 해시맵에 결과를 저장하고 조회하는 패턴과 유사하지만,
// 여기서는 어노테이션만으로 캐시 로직이 자동 적용된다.
public class Application {

    // [Order 1] 애플리케이션 진입점
    // C의 int main(int argc, char* argv[])와 동일한 역할
    public static void main(String[] args) {
        // [LEARN] SpringApplication.run()은 다음을 순차적으로 수행한다:
        // 1. ApplicationContext(IoC 컨테이너) 생성
        // 2. 모든 Bean 인스턴스화 및 의존성 주입
        // 3. 내장 웹 서버(Tomcat) 시작
        // 4. HTTP 요청 대기 상태 진입
        //
        // C에서 직접 소켓을 열고, accept() 루프를 돌리고,
        // 스레드 풀을 관리해야 했다면, 여기서는 이 한 줄로 모두 처리된다.
        SpringApplication.run(Application.class, args);
    }
}

// [Reader Notes]
// ═══════════════════════════════════════════════════════════════════════════════
// 이 파일에서 처음 등장한 개념: Spring Boot 부트스트랩, 어노테이션 기반 설정
//
// Spring Boot 핵심 개념 (C 개발자 관점):
// ───────────────────────────────────────────────────────────────────────────────
// 1. IoC (Inversion of Control) 컨테이너:
//    - C에서는 main()에서 필요한 모듈을 직접 초기화하고 함수 포인터를 전달했다면,
//    - Spring에서는 프레임워크가 객체의 생명주기를 관리하고 의존성을 주입한다.
//    - 개발자는 "무엇이 필요한지"만 선언하면, "어떻게 연결할지"는 프레임워크가 결정.
//
// 2. Bean:
//    - Spring이 관리하는 객체 인스턴스 (C의 전역 싱글톤 구조체와 유사)
//    - @Component, @Service, @Repository 등의 어노테이션으로 Bean 등록
//    - 생성자나 필드에 다른 Bean을 선언하면 자동으로 주입됨
//
// 3. 어노테이션(Annotation):
//    - 메타데이터를 코드에 부착하는 Java의 기능
//    - C의 #pragma나 __attribute__와 유사하지만, 런타임에도 접근 가능
//    - Spring은 어노테이션을 읽어 자동으로 동작을 결정
//
// 이 파일을 이해한 다음 볼 파일:
// ───────────────────────────────────────────────────────────────────────────────
// 1. common/controller/HealthController.java - 첫 번째 REST API 엔드포인트
// 2. common/exception/GlobalExceptionHandler.java - 전역 에러 처리
// 3. application.yml - 설정 파일
// ═══════════════════════════════════════════════════════════════════════════════
