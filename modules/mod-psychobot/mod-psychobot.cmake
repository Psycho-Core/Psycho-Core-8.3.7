# Psycho_Core - mod-psychobot optional CMake hook
# ------------------------------------------------
# This file is OPTIONAL and is include()d INLINE by modules/CMakeLists.txt during
# configuration. The module's sources (src/ + src/ai/) are auto-collected and the
# include directories auto-added by the module build engine, so nothing extra is
# required for Stage 1.
#
# Add extra find_package()/target_link_libraries(modules ...) here if a future
# stage needs an external dependency.

message(STATUS "  mod-psychobot: full module (engine + 12 classes + Phase D + socketless login) loaded.")
message(STATUS "  mod-psychobot: remember to apply sql/characters/psychobot_strategies.sql to the characters DB.")
