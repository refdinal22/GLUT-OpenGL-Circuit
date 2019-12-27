# Example 13
EXE=final

# Main target
all: $(EXE)

#  MinGW
ifeq "$(OS)" "Windows_NT"
CFLG=-O3 -Wall
LIBS=-lglut32cu -lglu32 -lopengl32
CLEAN=del *.exe *.o *.a
else
#  OSX
ifeq "$(shell uname)" "Darwin"
CFLG=-O3 -Wall -Wno-deprecated-declarations
LIBS=-framework GLUT -framework OpenGL
#  Linux/Unix/Solaris
else
CFLG=-O3 -Wall
LIBS=-lglut -lGLU -lGL -lm
endif
#  OSX/Linux/Unix/Solaris
CLEAN=rm -f $(EXE) *.o *.a
endif

# Dependencies
ex13.o: ex13.c JTKPOLBAN.h
fatal.o: fatal.c JTKPOLBAN.h
loadtexbmp.o: loadtexbmp.c JTKPOLBAN.h
print.o: print.c JTKPOLBAN.h
project.o: project.c JTKPOLBAN.h
errcheck.o: errcheck.c JTKPOLBAN.h
object.o: object.c JTKPOLBAN.h

#  Create archive
JTKPOLBAN.a:fatal.o loadtexbmp.o print.o project.o errcheck.o object.o
	ar -rcs $@ $^

# Compile rules
.c.o:
	gcc -c $(CFLG) $<
.cpp.o:
	g++ -c $(CFLG) $<

#  Link
final:final.o JTKPOLBAN.a
	gcc -O3 -o $@ $^   $(LIBS)

#  Clean
clean:
	$(CLEAN)
