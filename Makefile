NAME = scop

CXX = c++

CXXFLAGS = -Wall -Wextra -Werror -std=c++17 -Iinclude -Ilibs/glad/include

CPP_SRC = src/main.cpp \
		src/Shader.cpp \
		src/Math.cpp \
		src/Model.cpp

C_SRC = libs/glad/src/gl.c

OBJ = $(CPP_SRC:.cpp=.o) $(C_SRC:.c=.o)

LIBS = -lglfw -lGL -ldl

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME) $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re