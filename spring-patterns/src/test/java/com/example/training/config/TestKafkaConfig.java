package com.example.training.config;

import com.example.training.order.event.OrderEvent;
import org.mockito.Mockito;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.context.annotation.Primary;
import org.springframework.boot.autoconfigure.condition.ConditionalOnMissingBean;
import org.springframework.boot.autoconfigure.condition.ConditionalOnProperty;
import org.springframework.kafka.core.KafkaTemplate;

@Configuration
public class TestKafkaConfig {

    @Bean
    @Primary
    @ConditionalOnMissingBean
    @ConditionalOnProperty(name = "test.kafka.mock", havingValue = "true", matchIfMissing = true)
    public KafkaTemplate<String, OrderEvent> kafkaTemplate() {
        return Mockito.mock(KafkaTemplate.class);
    }
}
