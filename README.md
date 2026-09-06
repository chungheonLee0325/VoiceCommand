# **Dororong World — AI 음성 명령으로 즐기는 3D 자원 수집 게임 (UE, Whisper, Gemini)**

"도로롱, 제발 내 말좀 들어!!!" — 자연어 음성 명령을 실제 게임 행동으로 연결한 2일 AI 게임잼 프로젝트

### **💡 프로젝트 탐색 가이드**

이 README는 프로젝트의 핵심을 요약한 쇼케이스입니다. 전체 프로젝트는 상위 Project Gallery에서 함께 확인할 수 있습니다.

| 문서 | 역할 | 내용 |
| :---- | :---- | :---- |
| 📋 [Project Gallery](https://github.com/chungheonLee0325#-%EC%A3%BC%EC%9A%94-%ED%94%84%EB%A1%9C%EC%A0%9D%ED%8A%B8-main-projects) | Root | 전체 프로젝트 목록과 핵심 역량 |
| 📁 **Repository README** | What & Why | 프로젝트 목표, 데모, 역할, 주요 구현과 아키텍처 |

---

## **▶️ 데모 영상 (Gameplay Demo)**

<p align="center">
<a href="https://www.youtube.com/watch?v=cXoGXADRAa8">
<img src="https://github.com/chungheonLee0325/VoiceCommand/raw/main/Document/Dororong World_resize.gif" alt="프로젝트 하이라이트 영상 GIF" width="100%">
</a>
</p>

<p align="center">
<a href="https://www.youtube.com/watch?v=cXoGXADRAa8"><b>▶ YouTube에서 고화질로 시청하기</b></a>
</p>

---

## **프로젝트 개요 (Overview)**

**Dororong World**는 `AI와 게임의 융합`을 주제로 진행된 2일 게임잼에서 만든 3D 자원 수집 게임입니다.

저는 **Unreal Engine 클라이언트와 핵심 게임플레이 구현**을 담당했습니다.  
프로젝트 초기에 자연어 음성 명령으로 캐릭터에게 작업을 지시하는 아이디어를 제안했고, AI 개발자와 함께 게임에서 필요한 명령 데이터의 구조와 의미를 정했습니다.

이후 Unreal Engine에서 음성 데이터를 AI 서버로 전달하고, 구조화된 JSON 명령을 받아 실제 게임 대상과 행동으로 연결하는 Routing 시스템을 구현했습니다. 자원 수집, 성장, 목표 달성, 엔딩까지 이어지는 최소 게임 루프도 함께 구현해 2일 안에 플레이 가능한 형태로 완성했습니다.

**결과: AI 융합 게임잼 1등**

---

## **담당 역할 및 주요 기여 (My Role & Contributions)**

### **1. Unreal Engine Client / Gameplay 구현**

- 게임 내 음성 데이터를 녹음하여 AI 서버로 전송
- AI 서버가 반환한 JSON 명령 수신 및 해석
- `모두 / 아무나 / 개인(특정 대상)` 명령 Routing
- 플레이어 이동 및 기본 조작
- 나무 / 돌 / 광석 자원 시스템
- Pal의 작업 수행 흐름 연결
- 레벨업과 엔딩까지 이어지는 핵심 Game Loop 구현

### **2. AI-Game Command 데이터 구조와 실행 규칙 설계**

게임이 AI로부터 어떤 정보를 받아야 실제 행동으로 연결할 수 있는지 먼저 정의했습니다.

예를 들어 명령에는 다음과 같은 정보가 필요하도록 구성했습니다.

```text
Target    : 누가 명령을 수행하는가
Action    : 무엇을 하는가
Resource  : 어떤 대상/자원에 행동하는가
Modifier  : 행동 방식을 어떻게 바꾸는가
```

단순히 JSON 필드만 정한 것이 아니라, 각 값이 게임에서 어떤 의미와 동작으로 이어지는지도 함께 정했습니다.

```text
Target = Any
-> 현재 명령을 수행할 수 있는 Pal 중 하나 선택

Resource = Stone
-> Stone Gathering Task로 연결

Modifier = Fast
-> 이동 속도 증가
-> 우는 표정
-> 땀 Particle
```

즉, AI의 구조화된 결과를 Unreal Engine의 실제 객체와 행동으로 연결하기 위한 **데이터 구조와 Runtime Rule**을 설계했습니다.

### **3. 초기 아이디어 / 게임 규칙 제안**

게임잼 초기에 자연어 음성 명령을 핵심 상호작용으로 사용하는 아이디어를 제안했습니다.

단순히 음성을 텍스트로 바꾸는 기능이 아니라,

```text
Voice Command
-> AI가 의미를 구조화
-> 게임이 대상을 선택
-> 실제 행동 수행
-> 화면에서 결과 확인
```

으로 이어지는 플레이 흐름을 구성했습니다.

게임잼 특성상 별도의 기획 역할로 분리되기보다는, 개발 과정에서 게임의 최소 루프와 명령 동작 규칙을 함께 정했습니다.

### **4. AI 개발자와 Integration 협업**

AI 개발자는 Whisper / Gemini 기반 음성·자연어 처리 영역을 담당했고, 저는 게임에서 필요한 명령 구조와 실행 규칙, Unreal Engine 측 Integration을 담당했습니다.

서로의 영역을 다음 경계에서 맞췄습니다.

```text
Player Voice
    |
    v
Unreal Engine
- Audio Capture
- WAV Request
    |
    v
AI Server
- Whisper
- LangChain / Gemini 1.5 Pro
- Structured Command
    |
    v
Unreal Engine
- JSON Parse
- Target / Action Routing
- Gameplay Rule
- Player Feedback
```

AI가 어떤 형태로 결과를 반환해야 게임이 처리할 수 있는지, 게임에서는 그 결과를 어떤 상태와 행동으로 해석할지를 협의해 연결했습니다.

---

## **주요 기능 (Key Features)**

### **1. 자연어 음성 명령을 Gameplay Action으로 연결**

예를 들어 플레이어가 다음과 같이 말할 수 있습니다.

> "저기 아무나 돌 좀 빨리 캐줘"

AI의 결과를 그대로 실행하지 않고, 게임에서 사용할 수 있는 명령으로 해석합니다.

```text
Voice
"저기 아무나 돌 좀 빨리 캐줘"

        |
        v

Structured Command
Target   = Any
Resource = Stone
Modifier = Fast

        |
        v

Game Interpretation
Any
-> 수행 가능한 Pal 중 하나 선택

Stone
-> Stone Gathering Task

Fast
-> 이동 속도 증가
-> 표정 변화
-> 땀 Particle

        |
        v

Gameplay
선택된 Pal이 빠르게 이동해 돌을 채집
```

자연어의 의미가 실제 게임 규칙과 시각적 피드백으로 이어지도록 구현했습니다.

---

### **2. 대상 범위에 따른 Command Routing**

AI 서버가 반환한 JSON을 분석한 뒤 대상 범위에 따라 명령을 전달합니다.

#### **All**
모든 Pal에게 같은 작업을 전달합니다.

#### **Any**
현재 명령을 수행할 수 있는 Pal 중 하나를 선택해 작업을 전달합니다.

#### **Target**
플레이어가 특정 대상을 지칭한 경우 해당 Pal에게 작업을 전달합니다.

이 Routing 계층을 통해 자연어 분석 결과와 실제 게임 객체의 행동을 분리했습니다.

---

### **3. AI-Game 통신 흐름**

전체 명령 처리 흐름은 다음과 같습니다.

1. **Voice Input**
   - 플레이어가 게임 안에서 음성 명령을 입력합니다.

2. **Audio Request**
   - Unreal Engine에서 녹음한 WAV 데이터를 AI 서버로 전달합니다.

3. **Speech / Language Processing**
   - Whisper가 음성을 텍스트로 변환합니다.
   - LangChain과 Gemini 1.5 Pro를 통해 게임에서 사용할 구조화된 명령으로 변환합니다.

4. **Structured Response**
   - Unreal Engine이 JSON 형태의 명령 데이터를 수신합니다.

5. **Command Routing**
   - Target / Action / Resource / Modifier를 해석해 적절한 Pal과 Gameplay Task에 연결합니다.

6. **Player Feedback**
   - Pal의 행동, 이동 속도, 표정, Particle 등으로 결과를 플레이어에게 보여줍니다.

---

### **4. 2일 안에 완성한 최소 Game Loop**

AI 통신 Demo에 그치지 않고 처음부터 끝까지 플레이할 수 있는 최소 루프를 만들었습니다.

```text
Voice Command
    |
    v
Pal Task
    |
    v
Resource Collection
- Wood
- Stone
- Ore
    |
    v
Player Progression
    |
    v
Goal Completion
    |
    v
Ending
```

구현 범위:

- 플레이어 이동 / 조작
- 음성 명령 입력
- AI 서버 통신
- JSON 명령 수신 / 해석
- Command Routing
- Pal 작업 수행
- 자원 수집
- 레벨업
- 목표 달성
- 엔딩

---

## **구현에서 중요했던 점**

### **AI 출력과 게임 규칙 사이에 경계를 두었습니다**

게임 클라이언트는 자연어 모델의 결과를 그대로 실행할 수 없습니다.

실제 플레이로 연결하려면 다음을 정해야 했습니다.

- 게임이 AI에게 어떤 정보를 요구하는가
- 어떤 데이터 구조로 받을 것인가
- 각 값이 게임에서 무엇을 의미하는가
- 어떤 객체가 명령을 수행하는가
- 어떤 행동으로 연결되는가
- 결과를 플레이어에게 어떻게 보여주는가

그래서 AI의 결과를 게임 객체와 직접 결합하기보다,

```text
AI Output
-> Structured Command
-> Command Routing
-> Game Rule
-> Player Feedback
```

의 흐름으로 분리했습니다.

---

## **프로젝트 범위와 한계**

이 프로젝트는 **2일 게임잼 Prototype**입니다.

따라서 게임의 핵심 상호작용과 전체 플레이 루프를 우선 완성했고, Production 수준의 AI Integration에서 필요한 다음 항목들은 구현 범위에 포함하지 않았습니다.

- 단계별 Failure Logging
- Structured Command Schema Validation
- 잘못되거나 불완전한 AI Output의 Fallback
- Timeout / Retry / Cancel 정책
- 입력별 기대 결과를 관리하는 Eval Set
- Model / Prompt 변경에 대한 Gameplay Regression Test
- 응답 Latency 측정 및 플레이 흐름에 대한 영향 분석

지금 다시 확장한다면, 기능을 먼저 늘리기보다 `Voice -> STT -> Structured Output -> Routing -> Gameplay Action` 각 단계를 관찰하고 검증할 수 있는 구조부터 보강할 계획입니다.

---

## **Result**

- **AI 융합 게임잼 1등**
- **개발 기간: 2일**
- Unreal Engine Client / Gameplay 구현
- AI-Game Command 데이터 구조와 실행 규칙 설계
- AI 개발자와 음성·자연어 처리 ↔ 게임 실행 경계 협업

---

## **게임 Full Play 영상**

<a href="https://youtu.be/3WRTNIxKzws">
<img src="https://img.youtube.com/vi/3WRTNIxKzws/maxresdefault.jpg" alt="프로젝트 풀 영상" width="100%">
</a>

<sub>썸네일을 클릭하면 YouTube로 이동합니다.</sub>

---

## **🔗 Related**

- Repository: https://github.com/chungheonLee0325/VoiceCommand
- Project Gallery: https://github.com/chungheonLee0325
