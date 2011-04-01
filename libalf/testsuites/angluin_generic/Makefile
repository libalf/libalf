# $Id$
# testsuite for generic angluin L*
# (learning not just finite_automata but moore_machines with generic alphabet)

.PHONY: all clean nodata jpegs learn_generic

include ../../config.mk
CXXFLAGS+=-Xlinker -rpath=${LIBDIR}

CPPFLAGS+=-I../../include -I${INCLUDEDIR} -Wall -ggdb3

LDFLAGS+=-L../../src -L${LIBDIR} -lalf -lAMoRE++ -lreadline

all: learn_generic

clean: nodata
	-rm -f *.o learn_generic_L learn_generic_Lcol learn_generic_RV

nodata:
	-rm -f *.dot *.jpg *.angluin *.ser

jpegs:
	for EACH in *.dot; do dot -Tjpg "$$EACH" -o "$$EACH.jpg"; done;

learn_generic: learn_generic_L learn_generic_Lcol learn_generic_RV
	
learn_generic_L: CPPFLAGS += -DALGORITHM=1
learn_generic_L: learn_generic.cpp
	${CPP} ${CXXFLAGS} ${CPPFLAGS} ${LDFLAGS} learn_generic.cpp -o learn_generic_L

learn_generic_Lcol: CPPFLAGS += -DALGORITHM=2
learn_generic_Lcol: learn_generic.cpp
	${CPP} ${CXXFLAGS} ${CPPFLAGS} ${LDFLAGS} learn_generic.cpp -o learn_generic_Lcol

learn_generic_RV: CPPFLAGS += -DALGORITHM=3
learn_generic_RV: CPPFLAGS += -DDEBUG_RIVEST_SCHAPIRE
learn_generic_RV: learn_generic.cpp
	${CPP} ${CXXFLAGS} ${CPPFLAGS} ${LDFLAGS} learn_generic.cpp -o learn_generic_RV

