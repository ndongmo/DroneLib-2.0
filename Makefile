BUILD_DIR = build
TEST_DIR = tests
DRONE_DIR = src/drone
CLIENT_DIR = src/client

.PHONY: all
.PHONY: tests

all: drone client

tests:
	$(MAKE) -C $(TEST_DIR)

sender:
	$(MAKE) -C $(DRONE_DIR)

receiver:
	$(MAKE) -C $(CLIENT_DIR)

.PHONY: clean

clean:
	rm -r $(BUILD_DIR)