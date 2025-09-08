# WeatherAPP QMLFrontend 架构图

## 1. 程序架构图 (Program Architecture)

```mermaid
graph TB
    subgraph "Main Application"
        Main["Main.qml<br/>应用程序入口"]
    end
    
    subgraph "Models Layer 数据模型层"
        ASM["AppStateManager<br/>全局状态管理器"]
        WDM["WeatherDataModel<br/>天气数据模型"]
    end
    
    subgraph "Services Layer 服务层"
        WDS["WeatherDataService<br/>天气数据服务"]
    end
    
    subgraph "ViewModels Layer 视图模型层"
        WVM["WeatherViewModel<br/>天气视图模型"]
        NVM["NavigationViewModel<br/>导航视图模型"]
    end
    
    subgraph "Layouts Layer 布局层"
        SB["SideBar<br/>侧边栏"]
        CA["ContentArea<br/>内容区域"]
        RCP["RecentCitiesPager<br/>最近城市分页器"]
    end
    
    subgraph "Views Layer 视图层"
        BV["BaseView<br/>视图基类"]
        TWV["TodayWeatherView<br/>今日天气视图"]
        TTV["TemperatureTrendView<br/>温度趋势视图"]
        DIV["DetailedInfoView<br/>详细信息视图"]
        SSV["SunriseSunsetView<br/>日出日落视图"]
    end
    
    subgraph "Components Layer 组件层"
        subgraph "Common Components"
            WC["WindowControls<br/>窗口控制"]
            SBar["SearchBar<br/>搜索栏"]
            DA["DragArea<br/>拖拽区域"]
        end
        
        subgraph "Navigation Components"
            NM["NavigationMenu<br/>导航菜单"]
            MI["MenuItem<br/>菜单项"]
        end
        
        subgraph "Weather Components"
            TWI["TodaysWeatherItem<br/>今日天气项"]
            TTI["TempratureTrendItem<br/>温度趋势项"]
            DII["DetailedInfoItem<br/>详细信息项"]
        end
        
        subgraph "City Pager Components"
            RCM["RecentCitiesManager<br/>最近城市管理器"]
            CDC["CityDisplayCard<br/>城市显示卡片"]
            PI["PageIndicator<br/>分页指示器"]
        end
    end
    
    subgraph "Animations Layer 动画层"
        GE["GlassEffect<br/>玻璃效果"]
        EBE["EdgeBlurEffect<br/>边缘模糊效果"]
        GB["GradientBackground<br/>渐变背景"]
        CAS["ContentAreaStyle<br/>内容区域样式"]
        SS["SidebarStyle<br/>侧边栏样式"]
        RCPS["RecentCitiesPagerStyle<br/>分页器样式"]
    end
    
    %% 依赖关系
    Main --> ASM
    Main --> WVM
    Main --> NVM
    Main --> SB
    Main --> CA
    Main --> RCP
    
    WVM --> ASM
    WVM --> WDS
    WVM --> WDM
    NVM --> ASM
    
    SB --> NVM
    SB --> WVM
    SB --> WC
    SB --> SBar
    SB --> DA
    SB --> NM
    SB --> SS
    
    CA --> NVM
    CA --> WVM
    CA --> BV
    CA --> CAS
    
    RCP --> RCM
    RCP --> RCPS
    
    BV --> TWV
    BV --> TTV
    BV --> DIV
    BV --> SSV
    
    TWV --> TWI
    TTV --> TTI
    DIV --> DII
    
    RCM --> CDC
    RCM --> PI
    RCM --> GE
    
    NM --> MI
    
    %% 样式依赖
    SB -.-> SS
    CA -.-> CAS
    RCP -.-> RCPS
    RCM -.-> GE
```

## 2. 数据流结构图 (Data Flow Architecture)

```mermaid
flowchart TD
    subgraph "Data Sources 数据源"
        MockData["Mock Weather Data<br/>模拟天气数据"]
        UserInput["User Input<br/>用户输入"]
    end
    
    subgraph "Service Layer 服务层"
        WDS["WeatherDataService<br/>天气数据服务"]
    end
    
    subgraph "State Management 状态管理"
        ASM["AppStateManager<br/>全局状态管理器"]
    end
    
    subgraph "ViewModels 视图模型"
        WVM["WeatherViewModel<br/>天气视图模型"]
        NVM["NavigationViewModel<br/>导航视图模型"]
    end
    
    subgraph "UI Components 界面组件"
        SB["SideBar<br/>侧边栏"]
        CA["ContentArea<br/>内容区域"]
        RCP["RecentCitiesPager<br/>最近城市分页器"]
        Views["Views<br/>各种视图"]
    end
    
    subgraph "Data Models 数据模型"
        WDM["WeatherDataModel<br/>天气数据模型"]
        CityData["City Data<br/>城市数据"]
        ViewData["View-specific Data<br/>视图特定数据"]
    end
    
    %% 数据流向
    MockData --> WDS
    UserInput --> WVM
    UserInput --> NVM
    
    WDS -->|"天气数据"| WVM
    WVM -->|"状态更新"| ASM
    NVM -->|"导航状态"| ASM
    
    ASM -->|"城市变更信号"| WVM
    ASM -->|"视图模式变更"| NVM
    ASM -->|"当前城市数据"| CityData
    
    WVM -->|"处理后的数据"| WDM
    WDM -->|"结构化数据"| ViewData
    
    ViewData -->|"今日天气数据"| Views
    ViewData -->|"温度趋势数据"| Views
    ViewData -->|"详细信息数据"| Views
    ViewData -->|"日出日落数据"| Views
    
    WVM -->|"当前天气"| SB
    WVM -->|"视图数据"| CA
    WVM -->|"城市列表"| RCP
    
    NVM -->|"当前视图"| SB
    NVM -->|"导航状态"| CA
    
    %% 反馈循环
    SB -->|"搜索请求"| WVM
    SB -->|"导航请求"| NVM
    CA -->|"视图切换"| NVM
    RCP -->|"城市切换"| WVM
    Views -->|"用户交互"| WVM
    
    %% 数据流样式
    classDef dataSource fill:#e1f5fe
    classDef service fill:#f3e5f5
    classDef state fill:#fff3e0
    classDef viewmodel fill:#e8f5e8
    classDef ui fill:#fce4ec
    classDef model fill:#f1f8e9
    
    class MockData,UserInput dataSource
    class WDS service
    class ASM state
    class WVM,NVM viewmodel
    class SB,CA,RCP,Views ui
    class WDM,CityData,ViewData model
```

## 3. 信号传递图 (Signal Flow Diagram)

```mermaid
sequenceDiagram
    participant User as 用户
    participant SB as SideBar
    participant NVM as NavigationViewModel
    participant WVM as WeatherViewModel
    participant ASM as AppStateManager
    participant WDS as WeatherDataService
    participant CA as ContentArea
    participant RCP as RecentCitiesPager
    participant Views as Views
    
    Note over User,Views: 用户导航操作流程
    User->>SB: 点击导航菜单项
    SB->>NVM: menuItemClicked(itemId)
    NVM->>NVM: navigateToView(viewId)
    NVM->>ASM: setViewMode(viewId)
    ASM->>ASM: viewModeChanged(viewMode)
    ASM->>WVM: onViewModeChanged()
    ASM->>CA: viewModeChanged signal
    CA->>Views: 切换视图并传递数据
    
    Note over User,Views: 城市搜索和切换流程
    User->>SB: 输入搜索内容
    SB->>WVM: searchRequested(searchText)
    WVM->>WDS: searchCities(query)
    WDS->>WVM: 返回搜索结果
    WVM->>SB: 显示搜索结果
    
    User->>RCP: 点击城市卡片
    RCP->>WVM: switchToCity(index)
    WVM->>ASM: setCurrentCity(cityData)
    ASM->>ASM: cityChanged(cityData)
    ASM->>WVM: onCityChanged()
    ASM->>CA: cityChanged signal
    ASM->>RCP: cityChanged signal
    
    Note over User,Views: 天气数据加载流程
    WVM->>WDS: getCityWeather(cityName)
    WDS->>WDS: generateMockWeatherData()
    WDS->>WVM: dataLoaded(weatherData)
    WVM->>WVM: onDataLoaded()
    WVM->>ASM: weatherDataUpdated(data)
    WVM->>Views: weatherDataChanged(data)
    Views->>Views: updateCityData(data)
    
    Note over User,Views: 键盘导航流程
    User->>User: 按下方向键
    Note right of User: Main.qml 全局键盘事件
    alt 左右键
        User->>WVM: switchToPreviousCity() / switchToNextCity()
        WVM->>ASM: 城市切换逻辑
    else 上下键
        User->>NVM: navigateToNext()
        NVM->>ASM: 视图切换逻辑
    end
    
    Note over User,Views: 页面切换动画流程
    CA->>CA: 检测视图变化
    CA->>CA: 启动淡出动画 (currentView)
    CA->>CA: 加载新视图 (nextView)
    CA->>CA: 启动淡入动画 (nextView)
    CA->>CA: 清理旧视图
    
    Note over User,Views: 错误处理流程
    WDS->>WVM: dataLoadError(error)
    WVM->>WVM: onDataLoadError()
    WVM->>Views: errorOccurred(error)
    Views->>Views: 显示错误信息
```

## 架构说明

### 1. 程序架构特点
- **分层架构**: 采用经典的分层架构模式，从底层的数据模型到顶层的UI组件
- **MVVM模式**: 使用Model-View-ViewModel模式，实现数据与视图的分离
- **组件化设计**: 高度模块化的组件设计，便于维护和复用
- **状态集中管理**: 通过AppStateManager实现全局状态的统一管理

### 2. 数据流特点
- **单向数据流**: 数据从服务层流向视图层，保证数据流向清晰
- **响应式更新**: 基于QML信号槽机制实现响应式数据更新
- **视图特定数据**: 根据不同视图模式提供相应的数据结构

### 3. 信号传递特点
- **事件驱动**: 基于QML信号槽机制实现组件间通信
- **松耦合**: 组件间通过信号进行通信，降低耦合度
- **异步处理**: 数据加载和UI更新采用异步方式，提升用户体验