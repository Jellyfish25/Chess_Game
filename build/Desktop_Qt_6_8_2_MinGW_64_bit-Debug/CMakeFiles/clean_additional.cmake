# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\Chess_Game_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\Chess_Game_autogen.dir\\ParseCache.txt"
  "Chess_Game_autogen"
  )
endif()
