# Portfolio_Dx11_HR

김민혁의 포트폴리오용 Repository 입니다.

리소스가 포함되어 있지 않아 제대로 작동하지 않을 가능성이 높으며, 코드 공개를 위한 Repository 입니다. 

참고하여 만든 작품은 Hi-Fi Rush 라는 작품입니다.

![download](https://github.com/user-attachments/assets/ccdb8d28-3c07-42f2-99e5-864fc60a2767)

---

## 시연 영상 링크 

[Hi-Fi Rush 전체 프로젝트 영상](https://www.youtube.com/watch?v=rwTkUgP4ITA)

[Hi-Fi Rush 프로젝트 개인 작업물 영상](https://www.youtube.com/watch?v=_NlaHaI_s50)

---

# Extend Library Dependancy

> PhysX 5.3  
> FMOD  
> Assimp 5.3.1  
> Imgui  
> Imguizmo
> Effects11  
> DirectXTK  
> DirectMath

<hr/>

# DX11 모작 포트폴리오: Hi-Fi Rush

- **프로젝트명:** DX11 모작 포트폴리오 [Hi-Fi Rush]  
- **기간:** 2024 .09 ~ 2024 .11  
- **팀 프로젝트 여부:** O
- **맡은 파트:** : 프레임워크, 이펙트, 쉐이더, 케일 보스전, 터널 맵
- **기술 스택:**  
  C, C++, STL, FMOD, Win API, OpenCV, DLL, DirectX 11, vcpkg, HLSL, Effects11, Assimp, ImGui, ImGuizmo, DirectXTK, DirectMath, Nvidia PhysX 5.3 , DLL
- **IDE:** Visual Studio  

---

## 목적 및 설명
TPS 리듬 액션 게임 HI-Fi Rush 의 팀 포트폴리오 프레임워크, 이펙트, 쉐이더, 케일 보스전, 터널 맵을 만드는 역할을 수행하였습니다.
상용 엔진을 이용하지 않고 DirectX11을 이용한 엔진 부분을 DLL로 작업해 클라이언트와 엔진을 분리하여 작업하는 과정을 거친 작업물입니다.

---

## 주요 구현 기능

## 1. DirectX 11 기반 시스템
### 렌더링
- **디퍼드 라이팅**: 연산 부하를 줄이기 위해 G-Buffer 활용.
  - G-Buffer 데이터: `Color`, `Id`, `Position`, `Normal`, `Emissive`.
- **카툰 렌더링**: 
  - 셀 셰이딩을 사용해 단계별 음영 처리.
  - Animated 모델: 전면 컬링 방식.
  - Non-Animated 모델: 라플라스 필터로 외곽선 처리.

---

## 2. 최적화 작업
### Assimp 라이브러리 활용
- **FBX 데이터 처리**:
  - 모델 데이터를 Struct로 변환 후 **이진 직렬화**로 저장.
  - Texture Caching 및 멀티스레딩 최적화.
- **로딩 시간 단축**:
  - 최적화 전: 30분 → 최적화 후: 5분 이내 (Release 모드: 2분).

---

## 3. 물리 연산 및 충돌 처리
### PhysX 엔진
- 지형과 캐릭터의 상호작용 및 충돌 처리.
- **충돌 관리**:
  - `Collision Manager`에서 그룹 기반으로 충돌 비교 수행.

---

## 4. 이펙트 프레임워크
- **객체 관리**:
  - LifeCycle 관리, 재생 속도 조절.
  - `Union Effect`로 여러 이펙트를 통합 및 관리.
- **툴 지원**:
  - ImGui 및 Imguizmo 라이브러리를 사용한 편집 기능 제공.
- **UV 제어**:
  - Flow 효과와 Wrap Addressing 모드 구현.

---

## 5. 특수 효과
### Screen Space Decal
- 데칼 이펙트 및 Radial Blur, UV 제어 가능.
- Decal 상자를 World → Local 공간 변환 후 내부에만 렌더링.

### 파티클 효과
- **Point Instancing** 활용:
  - 각 Point의 생성 범위와 Pivot을 설정.
  - Randomize 옵션으로 다양한 효과 지원.

---

## 6. 보스전 및 게임 연출
### Kale 보스전
- **4단계 페이즈**:
  - 특정 캐릭터와 상호작용 및 약점 공략 가능.
  - Union Effect로 키프레임 기반 이펙트 관리.
- **AI 설계**:
  - 거리 및 방향 벡터 기반 이동 로직.
  - 슈퍼아머 게이지와 그로기 상태 구현.

### 무대 연출
- **Cube Map 반사**: 픽셀 법선 벡터와 카메라 위치를 사용해 구현.
- **미러볼 효과**:
  - Lissajous 곡선으로 랜덤한 그래프 생성.
  - 비트에 맞춘 레이저 효과.

---

## 7. 카메라와 2D Map 제어
- **Look 벡터 계산**:
  - Yaw, Pitch, Roll 각도로 정의.
- **특정 이벤트**:
  - 충돌 시 카메라 고정 및 부드러운 이동 구현.

---

## 8. 터널 맵과 환경 효과
### 터널 맵
- **터널 구성**:
  - Instancing으로 구성된 터널을 z축 이동 후 재배치.
  - 안개 효과 적용: G-Buffer에서 카메라 거리 기반 Blend.

### 트램 애니메이션
- Bone 분리 제어:
  - 각 Bone이 독립적으로 애니메이션 수행.

---

## 9. 기타 기술
### Shadow Mapping
- **Perspective Shadow Mapping**:
  - 가상의 광원 기준으로 깊이를 저장 후 그림자 판단.

### Screen Space Reflection
- View Space의 반사 벡터로 색상 값을 샘플링.

### UI 관리
- **Active UI**:
  - 생성/삭제 없이 Pool 방식으로 상태 전환.

---

## 트러블슈팅

### 1. **스마트 포인터**  
이번 작업을 하면서, 기존에 제가 사용하던 프레임워크를 거의 그대로 쓰게 되었는데, 작업을 하면서 제일 크게 느꼈던 문제가 레퍼런스 카운트를 수동 관리함으로써 스마트 포인터와 비슷한 기능을 수행하고 있었는데, 

협업 중에는 관리가 여의치 않을 것 같아
프레임워크의 모든 변수를 스마트포인터로 변환하는 작업과, 
다른 사람들도 스마트포인터를 사용하기 쉽게 만드는 매크로를 작성하는 작업을 거쳤습니다. 

이전까지는 레퍼런스 카운트를 누군가 실수하면 찾아 올라가기도 힘들고 의도치 않은 곳에서 누수도 한번씩 발생했는데,

스마트 포인터로 변경한 후에는 기존의 문제가 10건도 발생하지 않아 작업 효율을 엄청나게 올릴 수 있었습니다.

### 2. **이펙트 구현**  
제가 주로 제일 많이 작업한 파트가 이펙트 파트인데, 이펙트에 미묘하게 자잘한 효과도 많이 들어가고, 충돌 판정이나 사운드 같은 것들을 넣기 힘든 부분들이 많았는데, 그런 부분들을 
제가 만든 구조인 Union Effect로 구현해 이펙트의 생산성을 정말 많이 올릴 수 있었습니다. 

Union Effect에 대한 설명은 개인 작업물 영상에 있습니다.

---

## 후기
DirectX 11로 작업하면서 만든 팀 포트폴리오입니다.

2달동안 밤낮없이 달려온 결과물이고, 힘든 시간이었지만 그만큼 좋은 결과물을 볼 수 있었습니다.

제가 지금까지 사용했던 Ui 구조들과 최적화 기법, 아는 모든 것을 써서 만든 정리본 같은 느낌이 되었습니다.

모쪼록 후회 없이 불태웠던 프로젝트였습니다.



