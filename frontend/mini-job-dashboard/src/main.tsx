import React from 'react';
import ReactDOM from 'react-dom/client';
import App from './App';

// [FILE]
// - 목적: React 애플리케이션 엔트리 포인트.
// - 주요 역할: root DOM에 App 컴포넌트를 마운트한다.
// - 관련 토이 버전: [FE-F0.1]
// - 권장 읽는 순서: ReactDOM.createRoot 호출 -> App 렌더링.
//
// [LEARN] Vite + React 환경에서의 기본 엔트리 파일 구성을 익힌다.
ReactDOM.createRoot(document.getElementById('root') as HTMLElement).render(
  <React.StrictMode>
    <App />
  </React.StrictMode>,
);
