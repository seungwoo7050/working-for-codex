package com.example.minijob;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;

// [FILE]
// - 목적: mini-job-service의 진입점으로 Spring Boot 컨텍스트를 구동한다.
// - 주요 역할: 애플리케이션 부트스트랩, 컴포넌트 스캔, 설정 초기화.
// - 관련 토이 버전: [BE-v0.1]
// - 권장 읽는 순서: main -> 구성 요소(Controller/Service) 순으로 확인.
//
// [LEARN] Spring Boot 기본 부트스트랩 구조와 `SpringApplication.run` 호출 흐름을 학습한다.
@SpringBootApplication
public class MiniJobApplication {

    public static void main(String[] args) {
        SpringApplication.run(MiniJobApplication.class, args);
    }
}
