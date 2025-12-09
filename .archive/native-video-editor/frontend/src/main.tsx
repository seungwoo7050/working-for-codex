// [FILE]
// - 목적: React 애플리케이션의 진입점 (Entry Point)
// - 주요 역할: DOM에 React 앱을 마운트(렌더링)하는 부트스트랩 파일
// - 관련 클론 가이드 단계: [CG-v0.1.0] 프로젝트 부트스트랩
// - 권장 읽는 순서: 이 파일 → App.tsx → 개별 컴포넌트
//
// [LEARN] C 개발자를 위한 설명:
// - C에서 main() 함수가 프로그램 진입점이듯, 이 파일이 React 앱의 진입점이다.
// - C의 main()은 OS가 호출하지만, 여기서는 브라우저가 index.html을 로드하고
//   Vite가 이 파일을 번들링하여 실행한다.
// - import 문은 C의 #include와 유사하지만, ES Module 시스템으로 동작한다.

import React from 'react'
import ReactDOM from 'react-dom/client'
import App from './App.tsx'
import './index.css'

// [Order 1] 애플리케이션 마운트
// - document.getElementById('root')로 HTML의 <div id="root">를 찾는다.
// - ReactDOM.createRoot()로 React 18의 Concurrent Mode 루트를 생성한다.
// - C로 비유하면: 메모리 영역(root div)을 할당받아 UI 렌더링 컨텍스트를 초기화하는 것
//
// [LEARN] React.StrictMode란?
// - 개발 모드에서만 동작하는 "검사 모드"로, 잠재적 문제를 감지한다.
// - 컴포넌트를 의도적으로 두 번 렌더링하여 부작용(side effect)을 검출한다.
// - C의 -Wall -Werror 컴파일 옵션처럼, 더 엄격한 검사를 활성화하는 것과 유사하다.
ReactDOM.createRoot(document.getElementById('root')!).render(
  <React.StrictMode>
    <App />
  </React.StrictMode>,
)

// [Reader Notes]
// =============================================================================
// 이 파일에서 처음 등장한 개념: React, ReactDOM, JSX, ES Modules
//
// ## React 기본 개념 (C 개발자 관점)
// - React는 "선언적 UI 라이브러리"로, UI를 함수처럼 정의한다.
// - C에서 printf로 출력을 만들듯, React에서는 컴포넌트가 JSX를 반환한다.
// - JSX (JavaScript XML): HTML처럼 보이지만 실제로는 JavaScript 함수 호출로 변환된다.
//   예: <App /> → React.createElement(App, null)
//
// ## 파일 확장자 .tsx
// - TypeScript + JSX의 조합이다.
// - .c와 .h의 관계처럼, .ts는 순수 TypeScript, .tsx는 JSX를 포함한 TypeScript이다.
//
// ## 이 파일을 이해한 다음, 이어서 보면 좋은 파일:
// 1. App.tsx - 실제 애플리케이션 구조와 상태 관리
// 2. components/VideoUpload.tsx - 첫 번째 핵심 UI 컴포넌트
// =============================================================================
