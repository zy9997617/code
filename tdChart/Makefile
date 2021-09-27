TOP=.
OUTPUT?=../../output
include $(OUTPUT)/rules.mk

#是否编译Debug版本
DEBUG := N

EXENAME := asa
EXECFLAGS :=
EXELDFLAGS := -ltdWidget -ltdX11 -ltdCore -ltdPaint -lcairo -ltdCairo -lm
EXESRCS := src/main.c
EXESRCS += src/tdChart.c
EXESRCS += src/tdAxis.c
EXESRCS += src/showData.c
EXESRCS += src/tools.c
EXESRCS += src/changeframe.c
# EXESRCS += src/test.c

all: $(EXENAME)

clean: rules_clean

install:

include $(OUTPUT)/rules.mk
#--------------------------------------------------------------#
