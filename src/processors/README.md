# 🔄 Руководство по добавлению процессора

## 📋 Общая информация

Процессоры - это ключевые компоненты системы, отвечающие за обработку данных.
## 🛠️ Шаги для создания нового процессора

### 1️⃣ Создание директории и файлов

1. Создайте новую директорию в `src/processors/` с названием вашего процессора
2. Создайте файл для реализации процессора (`your_processor.cpp`)
3. При необходимости создайте файлы для метрик (`metrics.cpp`, `metrics.hpp`)

### 2️⃣ Реализация процессора

Базовая структура процессора:

```cpp
#include <common/util/assert.hpp>
#include <postboy/base/registry.hpp>

// Базовые классы для процессоров
// PutableProcessor - если процессор передает данные дальше
#include <postboy/base/putable_processor.hpp>
// TakeableProcessor - если процессор принимает данные
#include <postboy/base/takeable_processor.hpp>

// Дополнительные заголовки по необходимости

namespace postboy {

namespace {

// Класс процессора наследуется от TakeableProcessor и/или PutableProcessor
// Шаблонные параметры определяют типы входных и выходных данных
// Процессор может иметь несколько входных и выходных потоков разных типов
class YourProcessor :
    public TakeableProcessor<DataType1, DataType2>,
    public PutableProcessor<DataType1, DataType2>
{
public:
    // Основной метод обработки данных
    void Process() override {
        auto input1 = Take<1>();
        auto input2 = Take<2>();

        // ...

        Put<1>(*input1);
        Put<2>(*input2);
    }

    // Инициализация из конфигурации
    void Init(const config::Process& config) override {
        ASSERT(config.Processes_case() == config::Process::kYourProcessor,
               fmt::format("Process case for [YourProcessor] from config is not [kYourProcessor]"));

        // Настройка процессора из конфигурации
    }

    // Инициализация метрик (при необходимости)
    void InitMetrics(const std::string& name) override {
        metrics_.InitMetrics()(*registry_, name);
    }
};

} // namespace

// Регистрация процессора в системе
REGISTER_PROCESSOR(YourProcessor, config::Process::ProcessesCase::kYourProcessor);

} // namespace postboy
```

### 3️⃣ Создание метрик (опционально)

Если вашему процессору требуются метрики, создайте файлы `metrics.cpp` и `metrics.hpp`:

#### 📊 metrics.hpp:
```cpp
#pragma once
#include <postboy/base/metrics.hpp>

namespace postboy {

struct YourProcessorMetrics: public IMetricInitializer {
    LgbmAggressivePredictorMetrics() : IMetricInitializer() {
        initMetrics_ = [this](prometheus::Registry& registry, const std::string& name) -> void {
            counter = BuildCounter()
                .Name("cnt")
                .Register(registry)
                .Add({});
            histogram = BuildHistogram()
                .Name("histogram")
                .Register(registry)
                .Add({{"Packet", "Feature"}}, Histogram::BucketBoundaries{500, 1000, 1500, 2000})
        }
    }
    prometheus::HistogramPtr histogram;
    prometheus::CounterPtr counter;
}

} // namespace postboy
```

### 4️⃣ Настройка сборки

Создайте файл `CMakeLists.txt` в директории вашего процессора:

```cmake
# Если у вас есть отдельная библиотека метрик
add_library(
    your_processor_metrics
    metrics.cpp
)

target_link_libraries(
    your_processor_metrics

    postboy_base
    postboy_prometheus_metrics
)

# Регистрация процессора
add_processor(
    your_processor.cpp

    # Зависимости
    common_util
    fmt
    postboy_base
    your_processor_metrics
    # Другие зависимости
)
```

### 5️⃣ Обновление конфигурации Proto

Обновите файл конфигурации Proto, добавив новый тип процессора:

1. Найдите файл с определением `Process` (обычно в `src/proto/`)
2. Добавьте новый тип сообщения для конфигурации вашего процессора
3. Добавьте новое поле в сообщение `Process`

Пример:

```protobuf
message YourProcessorConfig {
    string param1 = 1;
    int32 param2 = 2;
    // другие параметры
}

message Process {
    // Существующие процессоры
    oneof Processes {
        // ...
        YourProcessorConfig your_processor = N; // Используйте следующий доступный номер
    }
}
```

## 📚 Примеры существующих процессоров

В проекте уже реализованы следующие процессоры, которые можно использовать как образцы:

| Процессор | Путь | Описание |
|-----------|------|----------|
| Dummy | `src/processors/common/dummy.cpp` | Простой проходной процессор |
| WebSocket | `src/processors/websocket/websocket.cpp` | Процессор для работы с WebSocket |
| FileReader | `src/processors/common/file_reader.cpp` | Процессор для чтения файлов |
| FileWriter | `src/processors/common/file_writer.cpp` | Процессор для записи в файлы |
