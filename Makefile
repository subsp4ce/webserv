CC =		c++
CFLAGS =	-std=c++98 -Wall -Wextra -Werror -pedantic
NAME =		webserv
INC_DIR =	./includes/
SRC_DIR = 	./src/
OBJ_DIR = 	./obj/
HEADER =	$(INC_DIR)AResponse.hpp \
			$(INC_DIR)BlockAlgorithm.hpp \
			$(INC_DIR)Cgi.hpp \
			$(INC_DIR)CgiResponse.hpp \
			$(INC_DIR)ConfigParser.hpp \
			$(INC_DIR)Connection.hpp\
			$(INC_DIR)DeleteResource.hpp \
			$(INC_DIR)DirectoryListing.hpp \
			$(INC_DIR)ErrorResponse.hpp \
			$(INC_DIR)GenericResponse.hpp \
			$(INC_DIR)Kqueue.hpp \
			$(INC_DIR)ListeningSocket.hpp \
			$(INC_DIR)LocationBlock.hpp \
			$(INC_DIR)Request.hpp \
			$(INC_DIR)ResolvePath.hpp \
			$(INC_DIR)ServerBlock.hpp \
			$(INC_DIR)utils.hpp

OBJ_FILES =	$(OBJ_DIR)main.o \
			$(OBJ_DIR)AResponse.o \
			$(OBJ_DIR)BlockAlgorithm.o \
			$(OBJ_DIR)Cgi.o \
			$(OBJ_DIR)CgiResponse.o \
			$(OBJ_DIR)ConfigParser.o \
			$(OBJ_DIR)Connection.o \
			$(OBJ_DIR)DeleteResource.o \
			$(OBJ_DIR)DirectoryListing.o \
			$(OBJ_DIR)ErrorResponse.o \
			$(OBJ_DIR)GenericResponse.o \
			$(OBJ_DIR)Kqueue.o \
			$(OBJ_DIR)ListeningSocket.o \
			$(OBJ_DIR)LocationBlock.o \
			$(OBJ_DIR)Request.o \
			$(OBJ_DIR)ResolvePath.o \
			$(OBJ_DIR)ServerBlock.o \
			$(OBJ_DIR)utils.o

all: $(NAME)

$(NAME): $(OBJ_FILES)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp $(HEADER)
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	@echo "clean..."
	rm -rf $(OBJ_DIR)

fclean: clean
	@echo "fclean..."
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
