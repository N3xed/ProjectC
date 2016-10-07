#pragma once

#include "RoutineArchive.h"

#define DLLAPI __declspec(dllexport)
#define DLLENTRY extern "C" DLLAPI void initialize(ProjectC::Execution::RoutineArchive& archive)
