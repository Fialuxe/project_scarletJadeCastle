# Makefile for Scarlet Jade Palace
TARGET = scarlet_jade
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Wno-deprecated-declarations
SRCS = main.c scene.c camera.c models.c shader.c
OBJS = $(SRCS:.c=.o)

# OS detection
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), Darwin)
	# macOS
	LIBS = -framework OpenGL -framework GLUT
else
	# Linux / Others
	LIBS = -lGL -lGLU -lglut -lm
endif

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
