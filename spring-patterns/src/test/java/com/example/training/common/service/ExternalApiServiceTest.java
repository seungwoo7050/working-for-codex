package com.example.training.common.service;

import okhttp3.mockwebserver.MockResponse;
import okhttp3.mockwebserver.MockWebServer;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.springframework.web.reactive.function.client.WebClient;

import java.io.IOException;
import java.util.Map;
import java.util.concurrent.TimeUnit;

import static org.assertj.core.api.Assertions.assertThat;

class ExternalApiServiceTest {

    private MockWebServer mockWebServer;
    private ExternalApiService externalApiService;

    @BeforeEach
    void setUp() throws IOException {
        mockWebServer = new MockWebServer();
        mockWebServer.start();

        WebClient webClient = WebClient.builder()
            .baseUrl(mockWebServer.url("/").toString())
            .build();

        externalApiService = new ExternalApiService(
            webClient,
            mockWebServer.url("/posts/1").toString()
        );
    }

    @AfterEach
    void tearDown() throws IOException {
        mockWebServer.shutdown();
    }

    @Test
    void fetchExternalData_shouldReturnFallback_onTimeout() {
        // Given - Response is delayed beyond the 3s timeout
        mockWebServer.enqueue(new MockResponse()
            .setBody("{\"id\": 99}")
            .setBodyDelay(5, TimeUnit.SECONDS));

        // When
        Map<String, Object> result = externalApiService.fetchExternalData();

        // Then - Should use fallback
        assertFallbackResponse(result);
    }

    @Test
    void fetchExternalData_shouldReturnFallback_onError() {
        // Given - Server returns error
        mockWebServer.enqueue(new MockResponse()
            .setResponseCode(404)
            .setBody("Not Found"));

        // When
        Map<String, Object> result = externalApiService.fetchExternalData();

        // Then
        assertFallbackResponse(result);
    }

    @Test
    void fetchExternalData_shouldRetryOnFailure() {
        // Given - First two requests fail, third succeeds
        mockWebServer.enqueue(new MockResponse().setResponseCode(500));
        mockWebServer.enqueue(new MockResponse().setResponseCode(500));
        mockWebServer.enqueue(new MockResponse()
            .setResponseCode(200)
            .setBody("{\"id\": 1, \"title\": \"test\"}")
            .addHeader("Content-Type", "application/json"));

        // When
        Map<String, Object> result = externalApiService.fetchExternalData();

        // Then
        assertThat(result)
            .containsEntry("id", 1)
            .containsEntry("title", "test")
            .doesNotContainKey("status");
        assertThat(mockWebServer.getRequestCount()).isEqualTo(3);
    }

    private void assertFallbackResponse(Map<String, Object> result) {
        assertThat(result).containsEntry("status", "fallback")
            .containsEntry("message", "External API is currently unavailable. Using cached or default data.")
            .containsEntry("userId", 1)
            .containsEntry("id", 1)
            .containsEntry("title", "Fallback Title")
            .containsEntry("body", "Fallback body content");
    }
}
