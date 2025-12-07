# 스냅샷 005: Elasticsearch (v1.3.0)

## 개요
- **대응 버전**: v1.3.0
- **패치 범위**: 1.3.1 ~ 1.3.5
- **빌드 가능**: ⚠️ Elasticsearch 인스턴스 필요
- **테스트 가능**: ⚠️ Elasticsearch 인스턴스 필요

## 포함된 기능
- v1.2.0의 모든 기능 (Batch Stats Cache)
- Product 도메인
- ProductDocument (Elasticsearch 문서)
- ProductSearchService

## 새로 추가된 컴포넌트
### Entity
- Product

### Document
- ProductDocument (@Document)

### Repository
- ProductRepository
- ProductSearchRepository (ElasticsearchRepository)

### Service
- ProductService
- ProductSearchService

### Controller
- ProductController
- ProductSearchController

## 주요 어노테이션
- `@Document`: Elasticsearch 문서 매핑
- `@Field`: 필드 타입 지정

## 빌드 방법
```bash
cd snapshots/005_elasticsearch
./gradlew build
```

## 테스트 방법
```bash
# Elasticsearch 실행 필요
docker run -d --name elasticsearch -p 9200:9200 -e "discovery.type=single-node" elasticsearch:8.11.0

./gradlew test
```

## 제한 사항
- 로컬 Elasticsearch 없으면 테스트 실패
- docker-compose.yml로 Elasticsearch 컨테이너 실행 권장
