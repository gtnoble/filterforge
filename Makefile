CFLAGS=-g -Wall -Wpedantic -Wextra -Werror -std=c11 `pkg-config --cflags guile-3.0` -shared -fPIC -Iinclude
CC=gcc

MODULE_NAME=filtopt
MODULE_INSTALL_DIR=/usr/share/guile/site/3.0/${MODULE_NAME}
EXTENSION_INSTALL_DIR=/usr/lib/x86_64-linux-gnu/guile/3.0/extensions

C_LIBRARY_DIR=lib
C_LIBRARY=${C_LIBRARY_DIR}/filtopt.so

C_SOURCE_DIR=src
C_SOURCE=$(wildcard ${C_SOURCE_DIR}/*.c)

GUILE_SOURCE_DIR=guile
GUILE_SOURCE=$(wildcard ${GUILE_SOURCE_DIR}/*.scm)

${C_LIBRARY}: ${C_SOURCE}
	$(CC) $(CFLAGS) ${C_SOURCE} -o ${C_LIBRARY}

.PHONY: install
install: ${C_LIBRARY} ${GUILE_SOURCE}
	cp -f ${C_LIBRARY} ${EXTENSION_INSTALL_DIR}
	mkdir -p ${MODULE_INSTALL_DIR}
	cp -f ${GUILE_SOURCE} ${MODULE_INSTALL_DIR}