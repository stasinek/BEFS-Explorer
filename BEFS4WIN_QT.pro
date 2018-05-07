TEMPLATE = app
CONFIG += windows
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += src
INCLUDEPATH += C:/OpenSSL-Win32/include
#LIBS += L"../../../../../x86_libraries/BHAPI" -libbhapi
#LIBS += -L"C:/OpenSSL-Win32/lib" -lubsec
#LIBS += -L"C:/OpenSSL-Win32/lib/MinGW" -lssleay32
#LIBS += -L"C:/OpenSSL-Win32/lib/MinGW" -llibeay32
LIBS += -L"C:/OpenSSL-Win32/lib/MinGW/libcrypto.a"
LIBS += -L"C:/OpenSSL-Win32/lib/MinGW/llibssl.a"

SOURCES += \
    GUI_DiskFunc.cpp \
    GUI_TreeControl.cpp \
    src/NT_System.cpp \
    src/BEOS_SystemWrapper.cpp \
    src/SKYFS_Interface.cpp \
    src/add-ons/kernel/file_systems/befs/BEFS_Attribute.cpp \
    src/add-ons/kernel/file_systems/befs/BEFS_BlockAllocator.cpp \
    src/add-ons/kernel/file_systems/befs/BEFS_Cache.cpp \
    src/add-ons/kernel/file_systems/befs/BEFS_Debug.cpp \
    src/add-ons/kernel/file_systems/befs/BEFS_Hash.cpp \
    src/add-ons/kernel/file_systems/befs/BEFS_Index.cpp \
    src/add-ons/kernel/file_systems/befs/BEFS_Inode.cpp \
    src/add-ons/kernel/file_systems/befs/BEFS_Interface.cpp \
    src/add-ons/kernel/file_systems/befs/BEFS_Journal.cpp \
    src/add-ons/kernel/file_systems/befs/BEFS_PlusTree.cpp \
    src/add-ons/kernel/file_systems/befs/BEFS_Query.cpp \
    src/add-ons/kernel/file_systems/befs/BEFS_SupportFunctions.cpp \
    src/add-ons/kernel/file_systems/befs/BEFS_Utility.cpp \
    src/add-ons/kernel/file_systems/befs/BEFS_Volume.cpp \
	
include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    BEFS4WIN_private.h \
    GUI_DiskFunc.h \
    BEFS.h \
    NT_DDK.h \
    NT_Native.h \
    src/BEOS_Defs.h \
    src/BEOS_SystemWrapper.h \
    SKYFS_Interface.h \
    src/add-ons/kernel/file_systems/befs/BEFS_Attribute.h \
    src/add-ons/kernel/file_systems/befs/BEFS_AutoLocker.h \
    src/add-ons/kernel/file_systems/befs/BEFS_BlockAllocator.h \
    src/add-ons/kernel/file_systems/befs/BEFS_Cache.h \
    src/add-ons/kernel/file_systems/befs/BEFS_CachedBlock.h \
    src/add-ons/kernel/file_systems/befs/BEFS_Chain.h \
    src/add-ons/kernel/file_systems/befs/BEFS_Control.h \
    src/add-ons/kernel/file_systems/befs/BEFS_Debug.h \
    src/add-ons/kernel/file_systems/befs/BEFS_DoublyLinkedList.h \
    src/add-ons/kernel/file_systems/befs/BEFS_Endian.h \
    src/add-ons/kernel/file_systems/befs/BEFS_Hash.h \
    src/add-ons/kernel/file_systems/befs/BEFS_Index.h \
    src/add-ons/kernel/file_systems/befs/BEFS_Inode.h \
    src/add-ons/kernel/file_systems/befs/BEFS_Interface.h \
    src/add-ons/kernel/file_systems/befs/BEFS_Journal.h \
    src/add-ons/kernel/file_systems/befs/BEFS_Lock.h \
    src/add-ons/kernel/file_systems/befs/BEFS_Logging.h \
    src/add-ons/kernel/file_systems/befs/BEFS_PlusTree.h \
    src/add-ons/kernel/file_systems/befs/BEFS_Query.h \
    src/add-ons/kernel/file_systems/befs/BEFS_Stack.h \
    src/add-ons/kernel/file_systems/befs/BEFS_SupportFunctions.h \
    src/add-ons/kernel/file_systems/befs/BEFS_Utility.h \
    src/add-ons/kernel/file_systems/befs/BEFS_Volume.h

LIBS += -mwindows -mthreads -lntoskrnl -lws2_32 -lkernel32 -luser32
LIBS += -lole32 -lshell32 -lcomctl32 -lwinmm

contains(QMAKE_COMPILER_DEFINES, __GNUC__) {
QMAKE_CXXFLAGS += -Wno-write-strings -Wno-multichar
QMAKE_CFLAGS   += -Wno-write-strings -Wno-multichar
QMAKE_CXXFLAGS += -Wno-unused-variable -Wno-unused-parameter -Wno-unused-value -Wno-unused-label
QMAKE_CFLAGS   += -Wno-unused-variable -Wno-unused-parameter -Wno-unused-value -Wno-unused-label
QMAKE_CXXFLAGS += -Wno-unused-but-set-variable
QMAKE_CFLAGS   += -Wno-unused-but-set-variable
QMAKE_CXXFLAGS += -Wattributes -Winline -Wshadow -Wall
QMAKE_CFLAGS   += -Wattributes -Winline -Wshadow -Wall
QMAKE_CXXFLAGS += -Wunknown-pragmas
QMAKE_CFLAGS   += -Wunknown-pragmas
QMAKE_CXXFLAGS += -w
QMAKE_CFLAGS   += -w


QMAKE_CXXFLAGS -= -pipe
QMAKE_CXXFLAGS += -save-temps
QMAKE_CXXFLAGS += -fverbose-asm
QMAKE_CXXFLAGS += -fstrict-aliasing
QMAKE_CXXFLAGS += -dD
QMAKE_CXXFLAGS += -g
QMAKE_CXXFLAGS += -std=gnu++11 -pthread
QMAKE_CXXFLAGS += -malign-double
QMAKE_CXXFLAGS += -momit-leaf-frame-pointer
QMAKE_CXXFLAGS += -fwrapv
QMAKE_CXXFLAGS += -funroll-loops
QMAKE_CXXFLAGS += -m32 -mfpmath=sse -flto #-O1
QMAKE_CXXFLAGS += -mpreferred-stack-boundary=8
QMAKE_CXXFLAGS += -mmmx -msse -msse2 #-msse3

#QMAKE_CXXFLAGS += -fno-leading-underscore
#QMAKE_CFLAGS   += -fno-leading-underscore
}

contains(QMAKE_COMPILER_DEFINES, __clang__) {
QMAKE_CXXFLAGS += -Wno-write-strings -Wno-multichar
QMAKE_CFLAGS   += -Wno-write-strings -Wno-multichar
QMAKE_CXXFLAGS += -Wno-unused-variable -Wno-unused-parameter -Wno-unused-value -Wno-unused-label
QMAKE_CFLAGS   += -Wno-unused-variable -Wno-unused-parameter -Wno-unused-value -Wno-unused-label
QMAKE_CXXFLAGS += -Wno-unused-but-set-variable
QMAKE_CFLAGS   += -Wno-unused-but-set-variable
QMAKE_CXXFLAGS += -Wattributes -Winline -Wshadow -Wall
QMAKE_CFLAGS   += -Wattributes -Winline -Wshadow -Wall
QMAKE_CXXFLAGS += -Wunknown-pragmas
QMAKE_CFLAGS   += -Wunknown-pragmas

QMAKE_CXXFLAGS += -save-temps
QMAKE_CXXFLAGS += -fverbose-asm
QMAKE_CXXFLAGS += -fstrict-aliasing
QMAKE_CXXFLAGS += -dD
QMAKE_CXXFLAGS += -g
QMAKE_CXXFLAGS += -std=gnu++0x -pthread
QMAKE_CXXFLAGS += -malign-double
QMAKE_CXXFLAGS += -momit-leaf-frame-pointer
QMAKE_CXXFLAGS += -fwrapv
QMAKE_CXXFLAGS += -funroll-loops
QMAKE_CXXFLAGS += -m32 --32 -mfpmath=sse -flto #-O1
QMAKE_CXXFLAGS += -mpreferred-stack-boundary=8
QMAKE_CXXFLAGS += -mmmx -msse -msse2 #-msse3
QMAKE_CXXFLAGS += -Qunused-arguments -Wno-error=unused-command-line-argument-hard-error-in-future
QMAKE_CXXFLAGS -= -fno-keep-inline-dllexport
QMAKE_CXXFLAGS -= -finline-small-functions
QMAKE_CXXFLAGS -= -pipe

QMAKE_LFLAGS += -Qunused-arguments -Wno-error=unused-command-line-argument-hard-error-in-future
QMAKE_LFLAGS -= -mthreads
}

