# Author: Adarsh Sharma 3/09/2024

# Compiler and compiler flags
CC = gcc

# Source files
SRCS = myshl.c parse_commands.c execute_command.c pinfo.c utils.c ls.c history_helper.c additional_feature.c process_handler.c
# Object files
OBJS = $(SRCS:.c=.o)
# Output executable
TARGET = mypshell


# Color codes
WHITE = \033[97m
CYAN = \033[96m
RESET = \033[0m
BLUE = \033[94m
GBG = \033[42m
BOLD = \033[1m
LYELLOW = \033[93m
ORG = \033[91m

# Default target
all: $(TARGET)
	@echo "$(BLUE)---------------------------------------------------------------$(RESET)"
	@echo "$(GBG)$(WHITE)$(BOLD)Build complete. $(RESET)"
	@echo "$(CYAN)Target executable name is:$(WHITE)$(BOLD) $(TARGET)$(RESET)"
	@echo "$(ORG)Run it as: $(LYELLOW)$(BOLD) ./$(TARGET)$(RESET)"
	@echo "$(BLUE)---------------------------------------------------------------$(RESET)"

# Rule to link the object files into the final executable
$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

# Rule to compile each source file into an object file
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Clean up build artifacts
clean:
	rm -f $(TARGET) $(OBJS)

# Phony targets
.PHONY: all clean
