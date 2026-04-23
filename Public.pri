# 避免重复包含
!isEmpty(PUBLIC_PRI_INCLUDED):error("Public.pri already included")
PUBLIC_PRI_INCLUDED = 1

CONFIG += c++14
CONFIG += resource_big  # 防止资源文件过大而导致程序崩溃
CONFIG -= app_bundle
CONFIG += depend_includepath  # 确保 INCLUDEPATH 中的路径用于依赖关系分析。
CONFIG += skip_target_version_ext unversioned_libname  # 取消so文件版本后缀


# 使Qt系统的日志信息包含更多的上下文信息，例如文件名、行号、函数名等
DEFINES += QT_MESSAGELOGCONTEXT
# 启用Qt的弃用警告。
DEFINES += QT_DEPRECATED_WARNINGS

# 返回某个库在当前平台和构建模式下应使用的名称
# 例如：库名 foo，则qtLibraryTargetName(foo)会依据平台返回
# macOS Debug: foo_debug, Windows Debug: food, Release: foo
defineReplace(qtLibraryTargetName) {
   unset(LIBRARY_NAME)
   LIBRARY_NAME = $$1
   CONFIG(debug, debug|release) {
      !debug_and_release|build_pass {
          mac:RET = $$member(LIBRARY_NAME, 0)_debug
          else:win32:RET = $$member(LIBRARY_NAME, 0)d
      }
   }
   isEmpty(RET):RET = $$LIBRARY_NAME
   return($$RET)
}

# 函数定义，判断当前编译使用的 Qt 版本是否至少为传入的版本
defineTest(minQtVersion) {
    maj = $$1
    min = $$2
    patch = $$3
    isEqual(QT_MAJOR_VERSION, $$maj) {
        isEqual(QT_MINOR_VERSION, $$min) {
            isEqual(QT_PATCH_VERSION, $$patch) {
                return(true)
            }
            greaterThan(QT_PATCH_VERSION, $$patch) {
                return(true)
            }
        }
        greaterThan(QT_MINOR_VERSION, $$min) {
            return(true)
        }
    }
    greaterThan(QT_MAJOR_VERSION, $$maj) {
        return(true)
    }
    return(false)
}

# 函数定义，去除路径中包含的源目录前缀，返回相对路径(相对于_PRO_FILE_PWD_)
defineReplace(stripSrcDir) {
    return($$relative_path($$absolute_path($$1, $$OUT_PWD), $$_PRO_FILE_PWD_))
}

# 设置库文件保存的文件夹名
isEmpty(MGS_LIBRARY_BASENAME) {
    MGS_LIBRARY_BASENAME = lib
}
# 第三方库存储路径
THIRD_PARTY_LIBRARY_PATH = $$PWD/3rdparty
# 项目共用文件存储路径
LANE_PUBLIC_PATH = $$PWD/src/Public
# 当前项目的根目录
MGS_SOURCE_TREE = $$PWD
# 统一处理构建输出路径，返回构建目录的根路径，与pro文件同一级，一般为名为build
isEmpty(MGS_BUILD_TREE) {
    sub_dir = $$_PRO_FILE_PWD_
    sub_dir ~= s,^$$re_escape($$PWD),,
    MGS_BUILD_TREE = $$clean_path($$OUT_PWD)
    MGS_BUILD_TREE ~= s,$$re_escape($$sub_dir)$,,
}
# 设置可执行文件输出路径 ./build/bin与./build/lib
MGS_OUTPUT_PATH = $$PWD
!macx {
    isEmpty(MGS_OUTPUT_PATH): MGS_OUTPUT_PATH = $$MGS_BUILD_TREE

    # 库文件的输出目录
    MGS_LIBRARY_PATH = $$MGS_OUTPUT_PATH/$$MGS_LIBRARY_BASENAME
    # 可执行文件的输出目录
    MGS_BIN_PATH = $$MGS_OUTPUT_PATH/bin
}

# 函数定义，拷贝指定的动态库到目标目录 DESTDIR
defineTest(copyLibsToDestdir) {
    win32 {
        LIB_EXT = dll
        FILE_NAME = $${1}.$${LIB_EXT}
        src = $$MGS_LIBRARY_PATH/win/$$FILE_NAME
        dst = $$DESTDIR/$$FILE_NAME

        exists($$src) {
            message("Will copy library: $$src -> $$dst")

            # 确保目标目录存在（mkdir 已存在也不报错）
            DEST_WIN = $$DESTDIR
            DEST_WIN ~= s,/,\\,g
            QMAKE_POST_LINK += if not exist $$shell_quote($$DEST_WIN) mkdir $$shell_quote($$DEST_WIN) $$escape_expand(\\n\\t)

            # 转换路径为 Windows 风格
            SRC_WIN = $$src
            DST_WIN = $$dst
            SRC_WIN ~= s,/,\\,g
            DST_WIN ~= s,/,\\,g

            QMAKE_POST_LINK += cmd /c copy /y $$shell_quote($$SRC_WIN) $$shell_quote($$DST_WIN) $$escape_expand(\\n\\t)
        }
        else {
            message("Warning: library not found in $$MGS_LIBRARY_PATH: $$FILE_NAME")
        }

        export(QMAKE_POST_LINK)
    }
    else:unix {
        LIB_EXT = so
        FILE_NAME = lib$${1}.$${LIB_EXT}   # Linux 下通常是 libxxx.so
        SRC_DIR = $$MGS_LIBRARY_PATH/linux
        DST_DIR = $$DESTDIR

        exists($$SRC_DIR/$$FILE_NAME*) {
            message("Will copy library set: $$SRC_DIR/$$FILE_NAME* -> $$DST_DIR/")

            QMAKE_POST_LINK += mkdir -p $$shell_quote($$DST_DIR) $$escape_expand(\\n\\t)

            # 使用 cp -a 保留软链接
            QMAKE_POST_LINK += cp -a $$SRC_DIR/$${FILE_NAME}* $$DST_DIR/ $$escape_expand(\\n\\t)
        } else {
            message("Warning: library not found in $$SRC_DIR: $$FILE_NAME*")
        }
        export(QMAKE_POST_LINK)
    }
    return(true)
}

# GCC 编译器（且不是 Clang），则添加编译选项 -Wno-noexcept-type。
# 这个选项用于抑制 GCC 关于 noexcept 类型比较的警告
gcc:!clang: QMAKE_CXXFLAGS += -Wno-noexcept-type

!macx {
    exists($$MGS_LIBRARY_PATH) {
        win32 {
            LIBS *= -L$$MGS_LIBRARY_PATH/win
        }
        unix {
            LIBS *= -L$$MGS_LIBRARY_PATH/linux
        }
    }
}

# Output directory，OUT_PWD表示构建输出目录
CONFIG(debug, debug|release) {
    output = $$OUT_PWD/debug
}
CONFIG(release, debug|release) {
    output = $$OUT_PWD/release
}

MOC_DIR     = $$output/moc
RCC_DIR     = $$output/rcc
UI_DIR      = $$output/ui
OBJECTS_DIR = $$output/obj

qt {
    contains(QT, core): QT *= concurrent
    contains(QT, gui): QT *= widgets
}

