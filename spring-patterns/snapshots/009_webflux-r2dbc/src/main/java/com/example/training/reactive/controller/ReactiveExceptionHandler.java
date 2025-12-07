package com.example.training.reactive.controller;

import com.example.training.reactive.service.ReactiveProductService;
import org.springframework.context.annotation.Profile;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.ExceptionHandler;
import org.springframework.web.bind.annotation.RestControllerAdvice;
import reactor.core.publisher.Mono;

import java.time.LocalDateTime;
import java.util.Map;

/**
 * 리액티브 컨트롤러 예외 핸들러
 */
@RestControllerAdvice(basePackages = "com.example.training.reactive")
@Profile("reactive")
public class ReactiveExceptionHandler {

    @ExceptionHandler(ReactiveProductService.ResourceNotFoundException.class)
    public Mono<ResponseEntity<Map<String, Object>>> handleNotFound(
            ReactiveProductService.ResourceNotFoundException ex) {
        return Mono.just(ResponseEntity.status(HttpStatus.NOT_FOUND)
                .body(Map.of(
                        "status", 404,
                        "error", "Not Found",
                        "message", ex.getMessage(),
                        "timestamp", LocalDateTime.now().toString()
                )));
    }

    @ExceptionHandler(Exception.class)
    public Mono<ResponseEntity<Map<String, Object>>> handleGenericError(Exception ex) {
        return Mono.just(ResponseEntity.status(HttpStatus.INTERNAL_SERVER_ERROR)
                .body(Map.of(
                        "status", 500,
                        "error", "Internal Server Error",
                        "message", ex.getMessage(),
                        "timestamp", LocalDateTime.now().toString()
                )));
    }
}
