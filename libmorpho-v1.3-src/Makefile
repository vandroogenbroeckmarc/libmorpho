#--------------------------------------------------------------------------------------
# Olivier Barnich, Renaud Dardenne and Marc Van Droogenbroeck
#--------------------------------------------------------------------------------------

PROJECT_NAME = libmorpho-v1.2

CC        = gcc
AR        = ar -cqs
RANLIB    = ranlib
STRIP     = strip -X -g 
PROTO     = cproto -q

FLAGS     = $(CFLAGS) $(OPTFLAGS)
CFLAGS    = -g -Wall -pedantic -ggdb
OPTFLAGS  = -O2 

LIBS      = -lm
LIB_PATHS = 
INCLUDES  = 

SRC_DIR   = src
OBJ_DIR   = objects
BIN_DIR   = bin
MAINS_DIR = examples
DOC_DIR   = doc
LIB_DIR   = lib

SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
HDR_FILES = $(wildcard $(SRC_DIR)/*.h)
OBJ_FILES = $(addprefix $(OBJ_DIR)/, $(notdir $(SRC_FILES:.c=.o)))
MAINS_FILES = $(wildcard $(MAINS_DIR)/*.c)
MAIN_HDR_FILES =  $(wildcard $(MAINS_DIR)/*.h)
MAINS_OBJ_FILES = $(addprefix $(OBJ_DIR)/, $(notdir $(MAINS_FILES:.c=.o)))
BIN_FILES = $(addprefix $(BIN_DIR)/, $(notdir $(MAINS_FILES:.c=)))

INSTALL_DIR_LIB    = /usr/local/lib
INSTALL_DIR_HEADER = /usr/local/include
INSTALL_DIR_DOC    = /usr/local/share/doc/$(PROJECT_NAME)

WINDOWSFLAG = -DWIN32

#--------------------------------------------------------------------------------------
default: $(OBJ_FILES)

#--------------------------------------------------------------------------------------
$(OBJ_DIR)/%.o: $(MAINS_DIR)/%.c $(MAINS_DIR)/%.h $(OBJ_FILES) 
	@echo "Compiling [$(notdir $<)]"
	@$(CC) -c $< $(FLAGS) $(LIB_PATHS) $(INCLUDES) -o $@
	@echo "Linking objects to exec file [$(BIN_DIR)/$*]"
	@$(CC) -o $(BIN_DIR)/$* $(OBJ_FILES) $@ $(FLAGS) $(LIBS) $(LIB_PATHS) $(INCLUDES)

#--------------------------------------------------------------------------------------
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HDR_FILES)
	@echo "Compiling [$(notdir $<)]"
	@$(CC) -c $< $(FLAGS) $(LIB_PATHS) $(INCLUDES) -o $@ 

#--------------------------------------------------------------------------------------
examples: $(MAINS_OBJ_FILES)

#--------------------------------------------------------------------------------------
lib: $(OBJ_FILES)
	@echo
	@echo "Stripping objects"
	@for i in $(OBJ_FILES);\
	do $(STRIP) $(OBJ_FILES);\
	done
	@echo "Creating static library $(OBJ_DIR)/$(PROJECT_NAME).a" 
	@$(AR) $(LIB_DIR)/$(PROJECT_NAME).a $(OBJ_FILES);
	@echo "Copying header file in $(LIB_DIR)" 
	@cp $(SRC_DIR)/*.h $(LIB_DIR)
	@echo "Creating dynamic library $(LIB_DIR)/$(PROJECT_NAME).so"
	@$(CC) -o $(LIB_DIR)/$(PROJECT_NAME).so  -shared  $(OBJ_FILES)
	@echo -n "Result: "
	@ls $(LIB_DIR)/

#--------------------------------------------------------------------------------------
.PHONY: doc
doc: 
	@echo "Generating documentation using doxygen"
	@doxygen doc/libmorpho.doxygen

#--------------------------------------------------------------------------------------
install: lib  doc
	@echo "Creating Directories"
	@mkdir -p $(INSTALL_DIR_LIB)/
	@mkdir -p $(INSTALL_DIR_HEADER)/
	@mkdir -p $(INSTALL_DIR_DOC)/
	@echo "Copying files"
	@cp $(LIB_DIR)/*.h $(INSTALL_DIR_HEADER)/
	@cp $(LIB_DIR)/$(PROJECT_NAME).a $(INSTALL_DIR_LIB)/
	@cp $(DOC_DIR)/html/* $(INSTALL_DIR_DOC)/
	@echo "Done."

#--------------------------------------------------------------------------------------
clean:
	@echo "Flushing..."
	@rm -f $(OBJ_DIR)/*.o $(OBJ_DIR)/*.a  $(OBJ_DIR)/*.h
	@rm -f $(SRC_DIR)/*~
	@rm -f $(SRC_DIR)/\#*
	@rm -f $(BIN_DIR)/*
	@rm -f $(LIB_DIR)/*
	@rm -f $(MAINS_DIR)/*~
	@rm -f *~ *.jpg *.mpg core  *.tar.gz *.txt *.o *.ras gmon.out *.ppm *.pgm *.a *.out *.txt
	@rm -rf doc/man/ doc/*~ 


#--------------------------------------------------------------------------------------
tgz: clean
	@echo " Creating tarball [$(PROJECT_NAME)-$(shell date +%Y-%m-%d-%Hh%Mm).tar.gz]"
	@cd .. ; tar -czf $(PROJECT_NAME)-$(shell date +%Y-%m-%d-%Hh%Mm).tar.gz $(PROJECT_NAME)
	@echo " Done!"


