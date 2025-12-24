CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wno-unused-parameter -O2
TEST_CXXFLAGS = -std=c++17 -Wall -Wextra -Wno-unused-parameter -g -O0
LDFLAGS = -lboost_program_options -lcrypto++ -lpthread
TEST_LDFLAGS = -lboost_program_options -lcrypto++ -lpthread -lUnitTest++

TARGET = vcalc_server
TEST_TARGET = vcalc_tests
BUILD_DIR = build
SRC_DIR = src
TEST_DIR = tests

SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
TEST_SOURCES = $(wildcard $(TEST_DIR)/*.cpp)
TEST_OBJECTS = $(TEST_SOURCES:$(TEST_DIR)/%.cpp=$(BUILD_DIR)/test_%.o)

.PHONY: all clean install debug test test-run test-clean test-db coverage help

all: $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)
	rm -f test_*.conf test_*.log test_*.txt coverage.info

debug: CXXFLAGS += -g -DDEBUG
debug: $(BUILD_DIR)/$(TARGET)

install: $(BUILD_DIR)/$(TARGET)
	sudo mkdir -p etc ./var/log
	sudo touch ./etc/vcalc.conf ./var/log/vcalc.log
	sudo chmod 644 ./etc/vcalc.conf 
	sudo chmod 666 ./var/log/vcalc.log

# Создание тестовой базы пользователей
test-db:
	@echo "Создание тестовой базы пользователей..."
	@echo "user1:password1" > test_users.conf
	@echo "testuser:testpass" >> test_users.conf
	@echo "admin:admin123" >> test_users.conf
	@echo "Тестовая база создана: test_users.conf"

# ==================== ТЕСТИРОВАНИЕ ====================

# Сборка тестов
test-build: $(BUILD_DIR)/$(TEST_TARGET)

# Запуск тестов
test-run: test-build
	@echo "Запуск модульных тестов..."
	@./$(BUILD_DIR)/$(TEST_TARGET)

# Сборка исполняемого файла тестов
$(BUILD_DIR)/$(TEST_TARGET): $(TEST_OBJECTS) $(filter-out $(BUILD_DIR)/main.o, $(OBJECTS))
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(TEST_OBJECTS) $(filter-out $(BUILD_DIR)/main.o, $(OBJECTS)) -o $@ $(TEST_LDFLAGS)

# Компиляция тестовых файлов
$(BUILD_DIR)/test_%.o: $(TEST_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(TEST_CXXFLAGS) -I$(SRC_DIR) -c $< -o $@

# Очистка только тестовых файлов
test-clean:
	rm -f $(BUILD_DIR)/test_*.o
	rm -f $(BUILD_DIR)/$(TEST_TARGET)
	rm -f test_*.log test_*.txt

# Генерация покрытия кода (требуется lcov и gcov)
coverage: TEST_CXXFLAGS += --coverage
coverage: CXXFLAGS += --coverage
coverage: LDFLAGS += --coverage
coverage: test-run
	@echo "Генерация отчета о покрытии кода..."
	@lcov --capture --directory $(BUILD_DIR) --output-file coverage.info
	@lcov --remove coverage.info "/usr/*" "*/tests/*" --output-file coverage.info
	@genhtml coverage.info --output-directory coverage_report
	@echo "Отчет доступен в coverage_report/index.html"

# Быстрые команды для удобства
check: test-run
test: test-run

# Список всех целей
help:
	@echo "Доступные цели:"
	@echo "  all           - сборка основной программы"
	@echo "  clean         - очистка всех собранных файлов"
	@echo "  debug         - сборка с отладочной информацией"
	@echo "  install       - установка директорий с файлами по умолчанию"
	@echo "  test-db       - создание тестовой базы пользователей"
	@echo ""
	@echo "Тестирование:"
	@echo "  test-build    - сборка тестов"
	@echo "  test-run      - сборка и запуск тестов"
	@echo "  check         - синоним для test-run"
	@echo "  test-clean    - очистка тестовых файлов"
	@echo "  coverage      - генерация отчета о покрытии кода"
	@echo "  help          - показать эту справку"