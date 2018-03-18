# LLVM-flowAnalysis

This is the repository for creating three dataflow analysis passes.
- ReachingDefinitionAnalysisPass.cpp
- LivenessAnalysisPass.cpp
- MayPointToAnalysisPass.cpp

# Run passes
#### Dependencies
1. install docker
2. Pull docker image using `sudo docker pull prodromou87/llvm:5.0.1` (assume you are using linux)
#### Run Passes
To run the passes, you need to do perform following steps:
1. run `./mount_and_launch.sh`, the script
2. once docker image is launched, `cd test/test-examples/`
3. you will find two folders `part3/`, `part2/` with test files.
4. `cd part3/`, then you can run live and maypointto pass using './run.sh'(do uncomment script to run the two passes.)
5. `cd part2/`, then `./run.sh` to run reachingdef pass.

#### Warning:
Please do not copy and paste the code, honor the integrity requirement. There might be bugs for part1.
