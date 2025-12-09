import { defineConfig } from 'vite';
import react from '@vitejs/plugin-react';

// [FILE]
// - 목적: Vite 빌드/개발 서버 설정을 정의한다.
// - 주요 역할: React 플러그인 활성화, 3000 포트 dev 서버 기본값 유지.
// - 관련 토이 버전: [FE-F0.1]
// - 권장 읽는 순서: 플러그인 배열 -> 서버 설정.
//
// [LEARN] Vite 설정 파일 구조와 플러그인 사용법을 이해한다.
export default defineConfig({
  plugins: [react()],
});
