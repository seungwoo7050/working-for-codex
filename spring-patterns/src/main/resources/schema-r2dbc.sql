-- R2DBC 스키마 초기화
-- products 테이블 생성

CREATE TABLE IF NOT EXISTS products (
    id BIGSERIAL PRIMARY KEY,
    name VARCHAR(255) NOT NULL,
    description TEXT,
    price DECIMAL(19, 2) NOT NULL,
    stock INTEGER DEFAULT 0,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- 샘플 데이터
INSERT INTO products (name, description, price, stock, created_at, updated_at)
VALUES 
    ('MacBook Pro', 'Apple MacBook Pro 14인치 M3', 2499000, 50, CURRENT_TIMESTAMP, CURRENT_TIMESTAMP),
    ('iPhone 15 Pro', 'Apple iPhone 15 Pro 256GB', 1550000, 100, CURRENT_TIMESTAMP, CURRENT_TIMESTAMP),
    ('AirPods Pro', 'Apple AirPods Pro 2세대', 359000, 200, CURRENT_TIMESTAMP, CURRENT_TIMESTAMP),
    ('iPad Air', 'Apple iPad Air 5세대', 929000, 75, CURRENT_TIMESTAMP, CURRENT_TIMESTAMP),
    ('Apple Watch', 'Apple Watch Series 9', 599000, 150, CURRENT_TIMESTAMP, CURRENT_TIMESTAMP);
