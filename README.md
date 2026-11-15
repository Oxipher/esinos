# ESINOS - CPU Scheduling Simulator

## Overview
ESINOS is a C-based CPU scheduling simulator developed for our Week 1 Team Kickoff. It implements and evaluates various process scheduling algorithms based on the CM4 specifications, including:
* First-Come, First-Served (FCFS)
* Shortest Job First (SJF)
* Round Robin (RR)
* Priority Scheduling

The simulator calculates and verifies key performance metrics, specifically average waiting time and average turnaround time, ensuring all algorithms match the expected theoretical numbers.

## Team Members
* **Member A:** [Name] - [Assigned Tasks/Schedulers]
* **Member B:** Yasmine Senhaji - [Assigned Tasks/Schedulers]
* **Member C:** Lilia Jaloun - Workloads, Metrics Calculator, and Test Automation

## Project Structure
* `workload.c`: Defines the simulated process tables (arrival time, burst time, priority).
* `util/metrics.c`: Central calculator for per-process waiting and turnaround times.
* `tests/run.sh`: Automated testing script to build the project and compare output against expected CM4 results.

## How to Build and Run

To compile the project and run the automated test suite, open your terminal in the root project directory and execute the test script:

```bash
# Run the automated build and test script
./tests/run.sh