#!/bin/bash
#
# 스냅샷 생성 스크립트
# 현재 최종 소스코드를 기반으로 특정 버전의 스냅샷을 생성합니다.
#
# 사용법:
#   ./generate-snapshot.sh <version>
#
# 예시:
#   ./generate-snapshot.sh v0.1.0
#   ./generate-snapshot.sh v1.0.0
#   ./generate-snapshot.sh v1.1.0
#

set -e

VERSION=${1:-""}
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(dirname "$SCRIPT_DIR")"
SNAPSHOT_DIR="$SCRIPT_DIR"

if [ -z "$VERSION" ]; then
    echo "사용법: $0 <version>"
    echo ""
    echo "사용 가능한 버전:"
    echo "  v0.1.0  - Bootstrap"
    echo "  v1.0.0  - Basic Infrastructure"
    echo "  v1.1.0  - Trim & Split"
    echo "  v1.2.0  - Subtitle & Speed"
    echo "  v1.3.0  - WebSocket & Persistence"
    echo "  v2.0.0  - Native Addon Setup"
    echo "  v2.1.0  - Thumbnail Extraction"
    echo "  v2.2.0  - Metadata Analysis"
    echo "  v2.3.0  - Performance Monitoring"
    exit 1
fi

# 버전별 스냅샷 디렉토리 매핑
case $VERSION in
    "v0.1.0")
        TARGET_DIR="004_v0.1.0-bootstrap"
        ;;
    "v1.0.0")
        TARGET_DIR="010_v1.0.0-basic-infrastructure"
        ;;
    "v1.1.0")
        TARGET_DIR="015_v1.1.0-trim-split"
        ;;
    "v1.2.0")
        TARGET_DIR="019_v1.2.0-subtitle-speed"
        ;;
    "v1.3.0")
        TARGET_DIR="026_v1.3.0-websocket-persistence"
        ;;
    "v2.0.0")
        TARGET_DIR="031_v2.0.0-native-addon-setup"
        ;;
    "v2.1.0")
        TARGET_DIR="036_v2.1.0-thumbnail-extraction"
        ;;
    "v2.2.0")
        TARGET_DIR="039_v2.2.0-metadata-analysis"
        ;;
    "v2.3.0")
        TARGET_DIR="042_v2.3.0-performance-monitoring"
        ;;
    *)
        echo "알 수 없는 버전: $VERSION"
        exit 1
        ;;
esac

echo "🔧 $VERSION 스냅샷 생성 중..."
echo "   대상 디렉토리: $SNAPSHOT_DIR/$TARGET_DIR"

# 디렉토리 생성
mkdir -p "$SNAPSHOT_DIR/$TARGET_DIR"

# 버전별 파일 복사 함수
copy_frontend_base() {
    local dest="$1"
    mkdir -p "$dest/frontend/src"
    
    # 기본 설정 파일
    cp "$ROOT_DIR/frontend/package.json" "$dest/frontend/"
    cp "$ROOT_DIR/frontend/vite.config.ts" "$dest/frontend/"
    cp "$ROOT_DIR/frontend/tsconfig.json" "$dest/frontend/"
    cp "$ROOT_DIR/frontend/tsconfig.node.json" "$dest/frontend/"
    cp "$ROOT_DIR/frontend/postcss.config.js" "$dest/frontend/"
    cp "$ROOT_DIR/frontend/tailwind.config.js" "$dest/frontend/"
    cp "$ROOT_DIR/frontend/index.html" "$dest/frontend/"
    
    # 기본 소스
    cp "$ROOT_DIR/frontend/src/main.tsx" "$dest/frontend/src/"
    cp "$ROOT_DIR/frontend/src/index.css" "$dest/frontend/src/"
    cp "$ROOT_DIR/frontend/src/vite-env.d.ts" "$dest/frontend/src/"
}

copy_backend_base() {
    local dest="$1"
    mkdir -p "$dest/backend/src"
    
    cp "$ROOT_DIR/backend/package.json" "$dest/backend/"
    cp "$ROOT_DIR/backend/tsconfig.json" "$dest/backend/"
}

# v0.1.0: Bootstrap
generate_v0_1_0() {
    local dest="$SNAPSHOT_DIR/$TARGET_DIR"
    
    copy_frontend_base "$dest"
    copy_backend_base "$dest"
    
    # 최소 App.tsx 생성
    cat > "$dest/frontend/src/App.tsx" << 'EOF'
function App() {
  return (
    <div className="min-h-screen bg-gray-900 text-white">
      <div className="container mx-auto p-8">
        <h1 className="text-4xl font-bold mb-2">Video Editor</h1>
        <p className="text-gray-400">v0.1.0 - Bootstrap</p>
      </div>
    </div>
  )
}
export default App
EOF
    
    # 최소 server.ts 생성
    cat > "$dest/backend/src/server.ts" << 'EOF'
import express from 'express';
import cors from 'cors';

const app = express();
const PORT = 3001;

app.use(cors());
app.use(express.json());

app.get('/health', (_req, res) => {
  res.json({ status: 'ok', version: '0.1.0' });
});

app.listen(PORT, () => {
  console.log(`Server running on http://localhost:${PORT}`);
});
EOF
    
    echo "✅ v0.1.0 스냅샷 생성 완료"
}

# v1.0.0: Basic Infrastructure
generate_v1_0_0() {
    local dest="$SNAPSHOT_DIR/$TARGET_DIR"
    
    copy_frontend_base "$dest"
    copy_backend_base "$dest"
    
    # Frontend 컴포넌트
    mkdir -p "$dest/frontend/src/components"
    mkdir -p "$dest/frontend/src/hooks"
    mkdir -p "$dest/frontend/src/types"
    
    cp "$ROOT_DIR/frontend/src/App.tsx" "$dest/frontend/src/"
    cp "$ROOT_DIR/frontend/src/components/VideoUpload.tsx" "$dest/frontend/src/components/"
    cp "$ROOT_DIR/frontend/src/components/VideoPlayer.tsx" "$dest/frontend/src/components/"
    cp "$ROOT_DIR/frontend/src/components/Timeline.tsx" "$dest/frontend/src/components/"
    cp "$ROOT_DIR/frontend/src/hooks/useVideoUpload.ts" "$dest/frontend/src/hooks/"
    cp "$ROOT_DIR/frontend/src/types/video.ts" "$dest/frontend/src/types/"
    
    # Backend 라우트
    mkdir -p "$dest/backend/src/routes"
    mkdir -p "$dest/backend/src/services"
    
    cp "$ROOT_DIR/backend/src/server.ts" "$dest/backend/src/"
    cp "$ROOT_DIR/backend/src/routes/upload.routes.ts" "$dest/backend/src/routes/"
    cp "$ROOT_DIR/backend/src/services/storage.service.ts" "$dest/backend/src/services/"
    
    echo "✅ v1.0.0 스냅샷 생성 완료"
}

# v1.1.0: Trim & Split
generate_v1_1_0() {
    generate_v1_0_0  # 이전 버전 포함
    
    local dest="$SNAPSHOT_DIR/$TARGET_DIR"
    
    # 추가 파일
    cp "$ROOT_DIR/frontend/src/components/EditPanel.tsx" "$dest/frontend/src/components/"
    cp "$ROOT_DIR/frontend/src/hooks/useVideoEdit.ts" "$dest/frontend/src/hooks/"
    cp "$ROOT_DIR/backend/src/services/ffmpeg.service.ts" "$dest/backend/src/services/"
    cp "$ROOT_DIR/backend/src/routes/edit.routes.ts" "$dest/backend/src/routes/"
    
    # types/edit.ts 복사 (있으면)
    if [ -f "$ROOT_DIR/frontend/src/types/edit.ts" ]; then
        cp "$ROOT_DIR/frontend/src/types/edit.ts" "$dest/frontend/src/types/"
    fi
    
    echo "✅ v1.1.0 스냅샷 생성 완료"
}

# 버전별 생성 함수 호출
case $VERSION in
    "v0.1.0")
        generate_v0_1_0
        ;;
    "v1.0.0")
        generate_v1_0_0
        ;;
    "v1.1.0")
        generate_v1_1_0
        ;;
    *)
        echo "⚠️  $VERSION 스냅샷 생성기가 아직 구현되지 않았습니다."
        echo "   SNAPSHOT-NOTES.md를 참조하여 수동으로 파일을 구성하세요."
        ;;
esac

echo ""
echo "📁 스냅샷 위치: $SNAPSHOT_DIR/$TARGET_DIR"
echo "📖 자세한 내용: $SNAPSHOT_DIR/$TARGET_DIR/SNAPSHOT-NOTES.md"
