/**
 *  \file benchmark/command_line.h
 *  \brief Various utilities for benchmarking
 *
 *  Copyright 2007-2012 IMP Inventors. All rights reserved.
 *
 */

#ifndef IMPBENCHMARK_COMMAND_LINE_H
#define IMPBENCHMARK_COMMAND_LINE_H

#include "benchmark_config.h"

IMPBENCHMARK_BEGIN_NAMESPACE

extern IMPBENCHMARKEXPORT bool help;
extern IMPBENCHMARKEXPORT bool profile_benchmark;
extern IMPBENCHMARKEXPORT bool profile_all;
extern IMPBENCHMARKEXPORT bool heap_profile_benchmark;
extern IMPBENCHMARKEXPORT bool heap_profile_all;
extern IMPBENCHMARKEXPORT bool leak_check_benchmark;
extern IMPBENCHMARKEXPORT bool leak_check_all;
extern IMPBENCHMARKEXPORT int log_level;
extern IMPBENCHMARKEXPORT int run_only;
extern IMPBENCHMARKEXPORT int next_benchmark;
extern IMPBENCHMARKEXPORT std::string benchmarks_name;


IMPBENCHMARK_END_NAMESPACE


#endif  /* IMPBENCHMARK_COMMAND_LINE_H */
