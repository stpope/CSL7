#
# Makefile for CSL SWIG Interfaces
#

SWIG_FLAGS = -python -c++ -module CSL6 -O 

DEF_FLAGS = -D_DEBUG=1 -DDEBUG=1 -DUSE_JUCE -DUSE_VBAP -DUSE_HRTF \
	-DUSE_AMBISONIC -DUSE_FFTREAL -DUSE_JSND -DUSE_JMIDI -DUSE_JTHREADS -DJUCE_DEBUG \
	-DJUCER_XCODE_MAC_F6D2F4CF=1 -DJUCE_APP_VERSION=6.0 -DJUCE_APP_VERSION_HEX=0x60000 \
	-DJucePlugin_Build_VST=0 -DJucePlugin_Build_VST3=0 -DJucePlugin_Build_AU=0 \
	-DJucePlugin_Build_AUv3=0 -DJucePlugin_Build_RTAS=0 -DJucePlugin_Build_AAX=0 \
	-DJucePlugin_Build_Standalone=0 -DJucePlugin_Build_Unity=0 -DJUCE_DISPLAY_SPLASH_SCREEN=0 \
	-DJUCE_REPORT_APP_USAGE=1 -DJUCE_USE_DARK_SPLASH_SCREEN=0 -DJUCE_PROJUCER_VERSION=0x60000 \
	-DJUCE_MODULE_AVAILABLE_juce_audio_basics=1 -DJUCE_MODULE_AVAILABLE_juce_audio_devices=1 \
	-DJUCE_MODULE_AVAILABLE_juce_audio_formats=1 -DJUCE_MODULE_AVAILABLE_juce_audio_processors=1 \
	-DJUCE_MODULE_AVAILABLE_juce_audio_utils=1 -DJUCE_MODULE_AVAILABLE_juce_core=1 \
	-DJUCE_MODULE_AVAILABLE_juce_cryptography=1 -DJUCE_MODULE_AVAILABLE_juce_data_structures=1 \
	-DJUCE_MODULE_AVAILABLE_juce_events=1 -DJUCE_MODULE_AVAILABLE_juce_graphics=1 \
	-DJUCE_MODULE_AVAILABLE_juce_gui_basics=1 -DJUCE_MODULE_AVAILABLE_juce_gui_extra=1 \
	-DJUCE_MODULE_AVAILABLE_juce_opengl=1 -DJUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1 \
	-DJUCE_STRICT_REFCOUNTEDPOINTER=1 -DJUCE_STANDALONE_APPLICATION=1

PYTHON_FLAGS = -I/opt/local/Library/Frameworks/Python.framework/Versions/3.6/include/python3.6m

GEN_FLAGS = -I$(CSL_ROOT)/Includes -I/usr/local/include -I. -I../../JuceLibraryCode -I/Content/Code/JUCE6/modules \
	$(PYTHON_FLAGS)

# -Wno-unused-result -Wsign-compare -Wunreachable-code -fno-common -dynamic -DNDEBUG -g -fwrapv -Wall -pipe

CSL_ROOT = ../../Src

CPPFLAGS = $(GEN_FLAGS) $(DEF_FLAGS) -Wc++11-extensions -Wc++11-long-long -w

.SUFFIXES: .cxx
CC = gcc
CXX = gcc
CFLAGS = -g -O2 -fno-common -dynamic

DLL_SRCS  = CSL6_wrap.cxx
DLL_OBJS  = $(DLL_SRCS:.cxx=.o)
DLL_OBJS := $(DLL_OBJS:.c=.o)

DLL_LDFLAGS = -dynamic -bundle -flat_namespace -bind_at_load \
	-bundle_loader /Users/stp/Code/Smalltalk/visual_Mac86.app/Contents/MacOS/visual \
	-undefined suppress -lgcc  -L/usr/local/lib -lCSL

DLL_NAME = CSL6_wrap.so


$(DLL_NAME): $(DLL_OBJS)
	gcc -bind_at_load -o $(DLL_NAME) $(DLL_OBJS) $(DLL_LDFLAGS) $(DLL_LDLIBS)

.cxx.o:
	$(CXX) -c $(DLL_CFLAGS) $(CPPFLAGS) $(CFLAGS) -o $@ $<

.c.o:
	$(CC) -c $(DLL_CFLAGS) $(CPPFLAGS) $(CFLAGS) -o $@ $<

swig:
	swig $(SWIG_FLAGS) $(DEF_FLAGS) $(GEN_FLAGS) CSL6.i

clean:
	-rm -f $(OBJS) $(DLL_NAME) $(DLL_EXP_DEF) example_wrap.c SwigType*.ssi ExampleVariables.ssi ExampleVariablesNI.ssi

CSL6_wrap.cxx: CSL6.i

CSL6_wrap.o: CSL6_wrap.cxx

