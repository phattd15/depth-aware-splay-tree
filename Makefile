# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall
BUILD_DIR = build

# Default target: build and run main.cpp
run: main
	@echo "Running main..."
	./$(BUILD_DIR)/main

# Compile main.cpp into the executable 'main'
main: main.cpp
	$(CXX) $(CXXFLAGS) -o $(BUILD_DIR)/main main.cpp

# Default target: build and run worst_case_experiment.cpp
worst: worst_case_experiment
	@echo "Running worst_case_experiment..."
	./worst_case_experiment

# Compile worst_case_experiment.cpp into the executable 'worst_case_experiment'
worst_case_experiment: worst_case_experiment.cpp
	$(CXX) $(CXXFLAGS) -o $(BUILD_DIR)/worst_case_experiment worst_case_experiment.cpp

dast: dast_analysis
	@echo "Running dast_analysis..."
	./$(BUILD_DIR)/dast_analysis

dast_analysis: dast_analysis.cpp
	$(CXX) $(CXXFLAGS) -o $(BUILD_DIR)/dast_analysis dast_analysis.cpp

cache: cache_benchmark
	@echo "Running cache_benchmark..."
	./$(BUILD_DIR)/cache_benchmark

cache_benchmark: cache_benchmark.cpp
	$(CXX) $(CXXFLAGS) -o $(BUILD_DIR)/cache_benchmark cache_benchmark.cpp

rand: random_benchmark
	@echo "Running random_benchmark..."
	./$(BUILD_DIR)/random_benchmark

random_benchmark: random_benchmark.cpp
	$(CXX) $(CXXFLAGS) -o $(BUILD_DIR)/random_benchmark random_benchmark.cpp

# Clean up generated files
clean:
	rm -rf build
