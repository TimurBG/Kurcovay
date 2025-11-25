CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wno-unused-parameter -O2
LDFLAGS = -lboost_program_options -lcrypto++ -lpthread

TARGET = vcalc_server
BUILD_DIR = build
SRC_DIR = src

SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

.PHONY: all clean install debug

all: $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

debug: CXXFLAGS += -g -DDEBUG
debug: $(BUILD_DIR)/$(TARGET)

install: $(BUILD_DIR)/$(TARGET)
	sudo cp $(BUILD_DIR)/$(TARGET) /usr/local/bin/
	sudo mkdir -p /etc /var/log
	sudo touch /etc/vcalc.conf /var/log/vcalc.log
	sudo chmod 644 /etc/vcalc.conf /var/log/vcalc.log

# Создание тестовой базы пользователей
test-db:
	@echo "Создание тестовой базы пользователей..."
	@echo "user1:password1" > test_users.conf
	@echo "user:P@ssW0rd" >> test_users.conf
	@echo "admin:admin123" >> test_users.conf
	@echo "Тестовая база создана: test_users.conf"
