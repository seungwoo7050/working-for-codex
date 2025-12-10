package com.example.training.config;

import org.springframework.boot.web.server.Compression;
import org.springframework.boot.web.server.WebServerFactoryCustomizer;
import org.springframework.boot.web.servlet.server.ConfigurableServletWebServerFactory;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.util.unit.DataSize;

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * [FILE] HTTP 응답 압축 설정
 * ─────────────────────────────────────────────────────────────────────────────────
 * - 목적: Gzip 압축으로 네트워크 대역폭 절감
 * - 관련 클론 가이드 단계: [CG-v2.3.0] 2.3.1 HTTP 압축 설정
 * 
 * [LEARN] C 개발자를 위한 HTTP 압축 개념:
 * 
 * 압축 없이:
 *   서버 → [100KB JSON] → 클라이언트
 *   네트워크 사용량: 100KB
 * 
 * Gzip 압축:
 *   서버 → [100KB JSON] → Gzip → [15KB] → 클라이언트 → Gunzip → [100KB]
 *   네트워크 사용량: 15KB (85% 절감)
 * 
 * C 관점:
 *   #include <zlib.h>
 *   compress(dest, &destLen, src, srcLen);  // 압축
 *   uncompress(dest, &destLen, src, srcLen); // 해제
 * 
 * Spring Boot: 설정만으로 자동 압축/해제
 * ═══════════════════════════════════════════════════════════════════════════════
 */
@Configuration
public class CompressionConfig {

    // ─────────────────────────────────────────────────────────────────────────
    // [Order 1] 웹 서버 압축 커스터마이저
    // ─────────────────────────────────────────────────────────────────────────
    /**
     * 웹 서버 압축 설정
     * - Gzip 압축 활성화
     * - 최소 1KB 이상 응답만 압축
     * - JSON, HTML, Text, JavaScript 압축
     */
    @Bean
    public WebServerFactoryCustomizer<ConfigurableServletWebServerFactory> compressionCustomizer() {
        return factory -> {
            Compression compression = new Compression();
            
            compression.setEnabled(true);
            // [LEARN] 압축 활성화: 요청 헤더에 Accept-Encoding: gzip 있으면 압축
            
            compression.setMinResponseSize(DataSize.ofKilobytes(1)); // 1KB 이상만 압축
            // [LEARN] 최소 크기 설정: 작은 응답은 압축 오버헤드가 더 클 수 있음
            // 일반적으로 1~2KB 이상부터 압축 효과 있음
            
            compression.setMimeTypes(new String[]{
                    "application/json",       // REST API 응답 (주요 대상)
                    "application/xml",        // XML 응답
                    "text/html",              // HTML 페이지
                    "text/xml",               // XML 텍스트
                    "text/plain",             // 일반 텍스트
                    "text/css",               // CSS 파일
                    "text/javascript",        // JS 파일
                    "application/javascript"  // JS 파일 (MIME 타입 변형)
            });
            // [LEARN] 압축 대상 MIME 타입:
            // - 텍스트 기반 콘텐츠만 압축 (JSON, HTML, JS, CSS)
            // - 이미지, 동영상은 이미 압축되어 있어 제외 (jpeg, png, mp4 등)
            
            factory.setCompression(compression);
        };
    }
    // ─────────────────────────────────────────────────────────────────────────
    // [LEARN] 압축 관련 HTTP 헤더:
    // 
    // 요청:
    //   Accept-Encoding: gzip, deflate, br
    //   (클라이언트가 지원하는 압축 알고리즘)
    // 
    // 응답 (압축된 경우):
    //   Content-Encoding: gzip
    //   (서버가 사용한 압축 알고리즘)
    // 
    // 트레이드오프:
    // - 장점: 네트워크 대역폭 절감, 전송 시간 단축
    // - 단점: CPU 사용량 증가 (압축/해제)
    // - 권장: 대부분의 웹 서비스에서 활성화
    // ─────────────────────────────────────────────────────────────────────────
}
