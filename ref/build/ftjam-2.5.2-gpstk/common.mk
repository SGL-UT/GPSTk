# Common Makefile rules
#

# the Jam sources needed to build "jam0"
#
SOURCES = \
	builtins.c command.c compile.c execnt.c execunix.c execvms.c expand.c \
	filent.c fileos2.c fileunix.c filevms.c glob.c hash.c \
	hdrmacro.c headers.c jam.c jambase.c jamgram.c lists.c make.c make1.c \
	newstr.c option.c parse.c pathunix.c pathvms.c regexp.c \
	rules.c scan.c search.c timestamp.c variable.c

# the bootstrap "jam0" build tool
#
jam0:
	$(CC) $(TARGET) $(CFLAGS) $(SOURCES) $(LINKLIBS)
