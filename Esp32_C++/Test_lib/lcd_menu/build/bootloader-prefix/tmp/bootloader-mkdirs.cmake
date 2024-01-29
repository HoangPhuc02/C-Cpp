# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "E:/Workspace/libary_emb/esp_framework/container/esp-idf/components/bootloader/subproject"
  "E:/Workspace/libary_emb/C-C++/Embedded_C99/Microcontroller/stm32f10x/test_lib/Aquarists/sample_project/build/bootloader"
  "E:/Workspace/libary_emb/C-C++/Embedded_C99/Microcontroller/stm32f10x/test_lib/Aquarists/sample_project/build/bootloader-prefix"
  "E:/Workspace/libary_emb/C-C++/Embedded_C99/Microcontroller/stm32f10x/test_lib/Aquarists/sample_project/build/bootloader-prefix/tmp"
  "E:/Workspace/libary_emb/C-C++/Embedded_C99/Microcontroller/stm32f10x/test_lib/Aquarists/sample_project/build/bootloader-prefix/src/bootloader-stamp"
  "E:/Workspace/libary_emb/C-C++/Embedded_C99/Microcontroller/stm32f10x/test_lib/Aquarists/sample_project/build/bootloader-prefix/src"
  "E:/Workspace/libary_emb/C-C++/Embedded_C99/Microcontroller/stm32f10x/test_lib/Aquarists/sample_project/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "E:/Workspace/libary_emb/C-C++/Embedded_C99/Microcontroller/stm32f10x/test_lib/Aquarists/sample_project/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "E:/Workspace/libary_emb/C-C++/Embedded_C99/Microcontroller/stm32f10x/test_lib/Aquarists/sample_project/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
