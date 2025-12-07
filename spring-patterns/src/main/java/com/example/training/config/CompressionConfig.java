package com.example.training.config;

import org.springframework.boot.web.server.Compression;
import org.springframework.boot.web.server.WebServerFactoryCustomizer;
import org.springframework.boot.web.servlet.server.ConfigurableServletWebServerFactory;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.util.unit.DataSize;

/**
 * HTTP 압축 설정
 * Gzip 압축으로 응답 크기 최적화
 */
@Configuration
public class CompressionConfig {

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
            compression.setMinResponseSize(DataSize.ofKilobytes(1)); // 1KB 이상만 압축
            compression.setMimeTypes(new String[]{
                    "application/json",
                    "application/xml",
                    "text/html",
                    "text/xml",
                    "text/plain",
                    "text/css",
                    "text/javascript",
                    "application/javascript"
            });
            factory.setCompression(compression);
        };
    }
}
