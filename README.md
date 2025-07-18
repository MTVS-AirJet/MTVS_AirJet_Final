# ✈️ AirJet

**AirJet**은 Unreal Engine 5.4를 기반으로 개발한 **멀티 비행 시뮬레이션** 프로젝트입니다.  
사용자는 직접 미션 경로를 제작하고 실제 시뮬레이션 장비와 연동하여 몰입감 있는 비행을 경험할 수 있으며,  
비행 결과는 AI 분석을 통해 정량적 피드백으로 제공됩니다.

---

## ✅ 프로젝트 소개

- **플랫폼**: PC / VR (Meta Quest 2)
- **장르**: 시뮬레이션 / 교육용 콘텐츠
- **개발 목표**:
  - 누구나 접근 가능한 훈련 환경 구축
  - 실제 장비 연동 및 멀티플레이 기반 시뮬레이션
  - 피드백 자동화를 통한 비행 실력 향상

---

## 📈 성과

### 🏆 외부 수상
- 제3기 메타버스 아카데미 성과공유회  
  → **한국전파진흥협회 우수상 수상**

---

## 👥 팀원 & 역할

| 역할 | 담당 업무 |
|------|-----------|
| 기획 (팀장) | - 비행 기능 및 미션/AI 전반 기획 |
| XR (부팀장) | - 세션 관리, UI<br>- VR 장비 연동<br>- Cesium 활용 좌표 처리 |
| XR | - 비행 매커니즘 및 HUD 개발<br>- RawInput 기반 장비 연동 |
| XR | - 미션 목표 설정 및 UI 구성 |
| TA | - Cockpit 내부 모델링<br>- 환경 구성 |
| BA | - 비행 루트 제작 기능<br>- 편집 UI |
| AI | - 비행 결과 분석 및 AI 피드백 반환 시스템 개발 |

---

## 🛠️ 기술 스택

- **엔진**: Unreal Engine 5.4  
- **언어**: C++  
- **하드웨어**: 키보드/마우스, HOTAS, Bravo Throttle, Meta Quest 2  
- **플러그인**:
  - `OnlineSubsystemSteam`: 세션 관리
  - `Cesium for Unreal`: 실세계 좌표 반영
  - `Raw Input`: HID 장비 입력 처리

---

## 🧩 프로젝트 구조

### 1. 로그인 및 로비 진입
> 로그인 → 서버 인증 → 로비 UI 진입 및 상태 동기화
<img width="1662" height="601" alt="Image" src="https://github.com/user-attachments/assets/d730a46f-30a1-4611-8da1-18b4dd715323" />

### 2. 미션 시스템 흐름
> 사용자 입력 → 목표 처리 → 서버 결과 응답 → UI 피드백
<img width="1832" height="502" alt="Image" src="https://github.com/user-attachments/assets/f163bd3e-c338-4571-8ca1-b853469b2d33" />

### 3. 플레이어 입력 및 무기 발사
> 입력 → 서버 전파 → 미션 상태 반영 및 동기화 처리
<img width="971" height="601" alt="Image" src="https://github.com/user-attachments/assets/d29a9909-05eb-482f-a2d1-8df8bda73f56" />

---

## ✈️ 주요 기능

- **비행 컨트롤**  
  → Pawn 기반 멀티플레이 비행 로직 구현

- **하드웨어 지원**  
  → HOTAS / Bravo 등 전문 장비 + MetaQuest2 VR 지원

- **미션 시스템**  
  → 비행 루트 제작 및 경로 이탈 감지, 미션 목표 설정

- **결과 처리**  
  → 서버 연동된 피드백 시스템 및 결과 UI 출력

- **HUD / UI**  
  → 속도, 고도, 방향, 미션 상태 등 시각화 구성


---

## 🔗 레퍼런스

- DCS World

![Image](https://github.com/user-attachments/assets/293a796c-7712-487c-81e3-2ee8f1da5335)
